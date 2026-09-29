from __future__ import annotations

import importlib.util
import json
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def load_module():
    spec = importlib.util.spec_from_file_location(
        "qtm3_api_signature_baseline",
        ROOT / "tools/api_signature_baseline.py",
    )
    module = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    spec.loader.exec_module(module)
    return module


api = load_module()


class ApiSignatureBaselineTests(unittest.TestCase):
    def write_xml(
        self,
        root: Path,
        *,
        args: str = "(int value) const",
        enum_b_initializer: str = "= 2",
    ) -> None:
        root.mkdir(parents=True, exist_ok=True)
        (root / "index.xml").write_text(
            '<doxygenindex>'
            '<compound refid="classFoo" kind="class"><name>QtMaterial::Foo</name></compound>'
            '<compound refid="namespaceQtMaterial" kind="namespace"><name>QtMaterial</name></compound>'
            '</doxygenindex>',
            encoding="utf-8",
        )
        (root / "classFoo.xml").write_text(
            '<doxygen><compounddef id="classFoo" kind="class">'
            '<compoundname>QtMaterial::Foo</compoundname>'
            '<location file="/workspace/include/qtmaterial/foo.h"/>'
            '<sectiondef kind="public-func">'
            '<memberdef kind="function" prot="public" const="yes" static="no" virt="non-virtual">'
            '<type>int</type><definition>int QtMaterial::Foo::bar</definition>'
            '<qualifiedname>QtMaterial::Foo::bar</qualifiedname>'
            f'<argsstring>{args}</argsstring><name>bar</name>'
            '<location file="/workspace/include/qtmaterial/foo.h"/>'
            '</memberdef></sectiondef>'
            '<sectiondef kind="private-attrib">'
            '<memberdef kind="variable" prot="private"><type>int</type><name>hidden</name></memberdef>'
            '</sectiondef>'
            '</compounddef></doxygen>',
            encoding="utf-8",
        )
        (root / "namespaceQtMaterial.xml").write_text(
            '<doxygen><compounddef id="namespaceQtMaterial" kind="namespace">'
            '<compoundname>QtMaterial</compoundname>'
            '<sectiondef kind="enum">'
            '<memberdef kind="enum" prot="public">'
            '<name>Mode</name><qualifiedname>QtMaterial::Mode</qualifiedname>'
            '<enumvalue><name>A</name><initializer>= 1</initializer></enumvalue>'
            f'<enumvalue><name>B</name><initializer>{enum_b_initializer}</initializer></enumvalue>'
            '<location file="/workspace/include/qtmaterial/mode.h"/>'
            '</memberdef></sectiondef>'
            '<sectiondef kind="func">'
            '<memberdef kind="function" prot="public" static="no">'
            '<type>bool</type><name>isModeEnabled</name>'
            '<qualifiedname>QtMaterial::isModeEnabled</qualifiedname>'
            '<argsstring>(Mode mode)</argsstring>'
            '<location file="/workspace/include/qtmaterial/mode.h"/>'
            '</memberdef></sectiondef>'
            '<sectiondef kind="typedef">'
            '<memberdef kind="typedef" prot="public">'
            '<type>unsigned int</type><name>ModeMask</name>'
            '<qualifiedname>QtMaterial::ModeMask</qualifiedname>'
            '<location file="/workspace/include/qtmaterial/mode.h"/>'
            '</memberdef></sectiondef>'
            '</compounddef></doxygen>',
            encoding="utf-8",
        )

    def test_extracts_public_surface_header_ownership_and_namespace_api(self):
        with tempfile.TemporaryDirectory() as tmp:
            xml = Path(tmp) / "xml"
            self.write_xml(xml)
            signatures = api.extract_signatures(xml)
            self.assertEqual(len(signatures), 5)
            self.assertTrue(
                any(
                    "QtMaterial::Foo::bar|public-func|function|int|(int value)const"
                    in item
                    and "file=include/qtmaterial/foo.h" in item
                    for item in signatures
                )
            )
            self.assertTrue(
                any(
                    "QtMaterial::Foo|compound|class|" in item
                    and "file=include/qtmaterial/foo.h" in item
                    for item in signatures
                )
            )
            self.assertTrue(
                any(
                    "QtMaterial::Mode|enum|enum|" in item
                    and "enum=A=1,B=2" in item
                    and "file=include/qtmaterial/mode.h" in item
                    for item in signatures
                )
            )
            self.assertTrue(any("QtMaterial::isModeEnabled|func|function|bool|(Mode mode)" in item for item in signatures))
            self.assertTrue(any("QtMaterial::ModeMask|typedef|typedef|unsigned int|" in item for item in signatures))
            self.assertFalse(any("hidden" in item for item in signatures))

    def test_public_header_filter_excludes_uninstalled_surface(self):
        with tempfile.TemporaryDirectory() as tmp:
            xml = Path(tmp) / "xml"
            self.write_xml(xml)
            signatures = api.extract_signatures(
                xml,
                {"qtmaterial/foo.h"},
            )
            self.assertEqual(len(signatures), 2)
            self.assertTrue(all("file=include/qtmaterial/foo.h" in item for item in signatures))
            self.assertFalse(any("QtMaterial::Mode" in item for item in signatures))
            self.assertFalse(any("QtMaterial::ModeMask" in item for item in signatures))
            self.assertFalse(any("isModeEnabled" in item for item in signatures))

    def test_canonical_manifest_contains_only_installable_headers(self):
        public = api.load_public_headers()
        self.assertIn(
            "include/qtmaterial/widgets/buttons/qtmaterialfab.h",
            public,
        )
        self.assertNotIn(
            "include/qtmaterial/core/private/qtmaterialaccessibilityhelper_p.h",
            public,
        )
        self.assertNotIn(
            "include/qtmaterial/specs/qtmaterialbuttonspecresolver.h",
            public,
        )

    def test_enum_initializer_change_is_breaking(self):
        with tempfile.TemporaryDirectory() as before_tmp, tempfile.TemporaryDirectory() as after_tmp:
            before = Path(before_tmp) / "xml"
            after = Path(after_tmp) / "xml"
            self.write_xml(before, enum_b_initializer="= 2")
            self.write_xml(after, enum_b_initializer="= 20")
            removed, added = api.compare(
                api.extract_signatures(after),
                api.extract_signatures(before),
            )
            self.assertTrue(any("enum=A=1,B=2" in item for item in removed))
            self.assertTrue(any("enum=A=1,B=20" in item for item in added))
            self.assertFalse(
                api.is_source_compatible(
                    api.extract_signatures(after),
                    api.extract_signatures(before),
                )
            )

    def test_additive_api_growth_is_compatible(self):
        self.assertTrue(api.is_source_compatible(["a", "b"], ["a"]))
        removed, added = api.compare(["a", "b"], ["a"])
        self.assertEqual(removed, [])
        self.assertEqual(added, ["b"])

    def test_compare_detects_signature_drift(self):
        removed, added = api.compare(["a", "b"], ["a", "c"])
        self.assertEqual(removed, ["c"])
        self.assertEqual(added, ["b"])

    def test_baseline_round_trip_schema(self):
        with tempfile.TemporaryDirectory() as tmp:
            xml = Path(tmp) / "xml"
            self.write_xml(xml)
            baseline = api.make_baseline(
                xml,
                1,
                {
                    "include/qtmaterial/foo.h",
                    "include/qtmaterial/mode.h",
                },
            )
            path = Path(tmp) / "baseline.json"
            path.write_text(json.dumps(baseline), encoding="utf-8")
            loaded = api.load_baseline(path)
            self.assertEqual(loaded["baselineMajor"], 1)
            self.assertEqual(loaded["signatures"], baseline["signatures"])


if __name__ == "__main__":
    unittest.main()
