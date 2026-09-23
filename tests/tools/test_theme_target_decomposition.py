from __future__ import annotations

import importlib.util
import sys
import tempfile
import unittest
from pathlib import Path

SCRIPT = Path(__file__).resolve().parents[2] / "tools/theme_contracts/targets.py"
SPEC = importlib.util.spec_from_file_location("qtm3_theme_decomp", SCRIPT)
assert SPEC is not None and SPEC.loader is not None
checker = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = checker
SPEC.loader.exec_module(checker)


class ThemeTargetDecompositionTests(unittest.TestCase):
    def root(self) -> Path:
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        root = Path(temp.name)
        for d in [
            "src/theme", "src/specs", "include/qtmaterial/theme",
            "include/qtmaterial/specs", "include/qtmaterial",
            "packaging"
        ]:
            (root/d).mkdir(parents=True, exist_ok=True)
        return root

    def write_minimum_valid_tree(self, root: Path) -> None:
        (root/"src/theme/CMakeLists.txt").write_text(
            "qtmaterial3_theme_model\nThemeModel\n"
            "qtmaterial3_theme_io\nThemeIO\n"
            "qtmaterial3_theme_runtime\nThemeRuntime\n",
            encoding="utf-8",
        )
        (root/"src/specs/CMakeLists.txt").write_text(
            "target_link_libraries(qtmaterial3_specs PUBLIC qtmaterial3_theme_model)\n",
            encoding="utf-8",
        )
        (root/"CMakeLists.txt").write_text(
            "qtmaterial3_theme_model\nqtmaterial3_theme_io\n"
            "qtmaterial3_theme_runtime\n",
            encoding="utf-8",
        )
        (root/"packaging/QtMaterial3WidgetsConfig.cmake.in").write_text(
            "ThemeModel\nThemeIO\nThemeRuntime\n",
            encoding="utf-8",
        )
        (root/"include/qtmaterial/qtmaterialglobal.h").write_text(
            "QTMATERIAL3_THEME_MODEL_EXPORT\nQTMATERIAL3_THEME_IO_EXPORT\n"
            "QTMATERIAL3_THEME_RUNTIME_EXPORT\n",
            encoding="utf-8",
        )
        for name in checker.MODEL_HEADERS:
            (root/"include/qtmaterial/theme"/name).write_text(
                "QTMATERIAL3_THEME_MODEL_EXPORT\n", encoding="utf-8"
            )
        for name in checker.IO_HEADERS:
            (root/"include/qtmaterial/theme"/name).write_text(
                "QTMATERIAL3_THEME_IO_EXPORT\n", encoding="utf-8"
            )
        for name in checker.RUNTIME_HEADERS:
            (root/"include/qtmaterial/theme"/name).write_text(
                "QTMATERIAL3_THEME_RUNTIME_EXPORT\n", encoding="utf-8"
            )

    def test_valid_decomposition_passes(self):
        root = self.root()
        self.write_minimum_valid_tree(root)
        self.assertEqual([], checker.validate_contract(root))

    def test_reintroduced_theme_umbrella_fails(self):
        root = self.root()
        self.write_minimum_valid_tree(root)
        path = root/"src/theme/CMakeLists.txt"
        path.write_text(
            path.read_text(encoding="utf-8")
            + "add_library(qtmaterial3_theme INTERFACE)\n"
            + "add_library(QtMaterial3::Theme ALIAS qtmaterial3_theme)\n",
            encoding="utf-8",
        )
        errors = checker.validate_contract(root)
        self.assertTrue(any("umbrella target" in e for e in errors))

    def test_specs_including_serializer_fails(self):
        root = self.root()
        self.write_minimum_valid_tree(root)
        (root/"src/specs/bad.cpp").write_text(
            '#include "qtmaterial/theme/qtmaterialthemeserializer.h"\n',
            encoding="utf-8",
        )
        errors = checker.validate_contract(root)
        self.assertTrue(any("crosses Specs boundary" in e for e in errors))

    def test_io_header_using_old_export_fails(self):
        root = self.root()
        self.write_minimum_valid_tree(root)
        (root/"include/qtmaterial/theme/qtmaterialthemeserializer.h").write_text(
            "QTMATERIAL3_THEME_EXPORT\n", encoding="utf-8"
        )
        errors = checker.validate_contract(root)
        self.assertTrue(any("still uses QTMATERIAL3_THEME_EXPORT" in e for e in errors))

    def test_missing_runtime_target_fails(self):
        root = self.root()
        self.write_minimum_valid_tree(root)
        path = root/"src/theme/CMakeLists.txt"
        path.write_text(
            path.read_text(encoding="utf-8").replace("qtmaterial3_theme_runtime", ""),
            encoding="utf-8",
        )
        errors = checker.validate_contract(root)
        self.assertTrue(any("qtmaterial3_theme_runtime" in e for e in errors))


if __name__ == "__main__":
    unittest.main()
