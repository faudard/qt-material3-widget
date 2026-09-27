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
    def write_xml(self, root: Path, *, args: str = "(int value) const") -> None:
        root.mkdir(parents=True, exist_ok=True)
        (root / "index.xml").write_text(
            '<doxygenindex><compound refid="classFoo" kind="class">'
            '<name>Foo</name></compound></doxygenindex>',
            encoding="utf-8",
        )
        (root / "classFoo.xml").write_text(
            '<doxygen><compounddef id="classFoo" kind="class">'
            '<compoundname>Foo</compoundname>'
            '<sectiondef kind="public-func">'
            '<memberdef kind="function" prot="public" const="yes" static="no" virt="non-virtual">'
            '<type>int</type><definition>int Foo::bar</definition>'
            f'<argsstring>{args}</argsstring><name>bar</name>'
            '</memberdef></sectiondef>'
            '<sectiondef kind="private-attrib">'
            '<memberdef kind="variable" prot="private"><type>int</type><name>hidden</name></memberdef>'
            '</sectiondef>'
            '</compounddef></doxygen>',
            encoding="utf-8",
        )

    def test_extracts_public_not_private_surface(self):
        with tempfile.TemporaryDirectory() as tmp:
            xml = Path(tmp) / "xml"
            self.write_xml(xml)
            signatures = api.extract_signatures(xml)
            self.assertEqual(len(signatures), 1)
            self.assertIn("Foo|public-func|function|bar|int|(int value)const", signatures[0])
            self.assertNotIn("hidden", signatures[0])

    def test_compare_detects_signature_drift(self):
        removed, added = api.compare(["a", "b"], ["a", "c"])
        self.assertEqual(removed, ["c"])
        self.assertEqual(added, ["b"])

    def test_baseline_round_trip_schema(self):
        with tempfile.TemporaryDirectory() as tmp:
            xml = Path(tmp) / "xml"
            self.write_xml(xml)
            baseline = api.make_baseline(xml, 1)
            path = Path(tmp) / "baseline.json"
            path.write_text(json.dumps(baseline), encoding="utf-8")
            loaded = api.load_baseline(path)
            self.assertEqual(loaded["baselineMajor"], 1)
            self.assertEqual(loaded["signatures"], baseline["signatures"])


if __name__ == "__main__":
    unittest.main()
