from __future__ import annotations

import json
import sys
import tempfile
import unittest
from pathlib import Path

TOOLS = Path(__file__).resolve().parents[2] / "tools"
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))

import check_release


class ApiFreezeReleaseCheckerTests(unittest.TestCase):
    def make_root(self) -> tuple[Path, dict]:
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        root = Path(temp.name)

        for directory in (
            "cmake",
            "docs/components",
            "include/qtmaterial/widgets",
            "packaging",
            "tests/widgets",
        ):
            (root / directory).mkdir(parents=True, exist_ok=True)

        (root / "include/qtmaterial/widgets/qtmaterialfoo.h").write_text(
            "#pragma once\nclass Foo {};\n",
            encoding="utf-8",
        )
        (root / "cmake/QtMaterial3HeaderSurfaceManifest.cmake").write_text(
            'set(QTMATERIAL3_PUBLIC_HEADERS\n'
            '    "qtmaterial/widgets/qtmaterialfoo.h"\n'
            ')\n\n'
            'set(QTMATERIAL3_PRIVATE_HEADERS\n'
            ')\n',
            encoding="utf-8",
        )
        (root / "docs/components/component-registry.json").write_text(
            json.dumps([
                {"publicHeader": "qtmaterial/widgets/qtmaterialfoo.h"}
            ]),
            encoding="utf-8",
        )
        (root / "packaging/QtMaterial3WidgetsConfig.cmake.in").write_text(
            "set(_QtMaterial3Widgets_supported_components\n"
            "    ThemeModel\n"
            "    ThemeIO\n"
            "    ThemeRuntime\n"
            "    Widgets\n"
            "    Integration\n"
            ")\n",
            encoding="utf-8",
        )
        (root / "tests/widgets/tst_foo.cpp").write_text(
            "int main() { return 0; }\n",
            encoding="utf-8",
        )
        (root / "tests/CMakeLists.txt").write_text(
            "qtm3_add_test(tst_foo widgets/tst_foo.cpp qtmaterial3_widgets)\n",
            encoding="utf-8",
        )

        rules = {
            "api_freeze": {
                "package_components": [
                    "ThemeModel", "ThemeIO", "ThemeRuntime",
                    "Widgets", "Integration",
                ],
                "forbidden_public_header_suffixes": ["specresolver.h"],
                "forbidden_public_text": [
                    "Backward-compatible", "compatibility-only",
                ],
                "public_support_headers": [],
                "standalone_cpp_tests": [],
            }
        }
        return root, rules

    def test_all_scope_includes_api_freeze(self):
        self.assertIn(
            "api-freeze",
            check_release.normalize_scopes(["all"]),
        )

    def test_api_freeze_accepts_owned_registered_surface(self):
        root, rules = self.make_root()
        self.assertEqual(check_release.validate_api_freeze(root, rules), [])

    def test_api_freeze_rejects_unregistered_cpp_test(self):
        root, rules = self.make_root()
        (root / "tests/widgets/tst_orphan.cpp").write_text(
            "int main() { return 0; }\n",
            encoding="utf-8",
        )
        errors = check_release.validate_api_freeze(root, rules)
        self.assertTrue(
            any("tst_orphan.cpp" in error for error in errors),
            errors,
        )

    def test_api_freeze_rejects_unowned_public_widget_header(self):
        root, rules = self.make_root()
        (root / "include/qtmaterial/widgets/qtmaterialextra.h").write_text(
            "#pragma once\n",
            encoding="utf-8",
        )
        manifest = root / "cmake/QtMaterial3HeaderSurfaceManifest.cmake"
        text = manifest.read_text(encoding="utf-8")
        text = text.replace(
            '    "qtmaterial/widgets/qtmaterialfoo.h"\n',
            '    "qtmaterial/widgets/qtmaterialextra.h"\n'
            '    "qtmaterial/widgets/qtmaterialfoo.h"\n',
        )
        manifest.write_text(text, encoding="utf-8")
        errors = check_release.validate_api_freeze(root, rules)
        self.assertTrue(
            any("qtmaterialextra.h" in error for error in errors),
            errors,
        )


if __name__ == "__main__":
    unittest.main()
