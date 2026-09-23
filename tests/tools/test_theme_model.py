from __future__ import annotations

import importlib.util
import sys
import tempfile
import unittest
from pathlib import Path

SCRIPT = Path(__file__).resolve().parents[2] / "tools/theme_contracts/model.py"
SPEC = importlib.util.spec_from_file_location("qtm3_theme_model_checker", SCRIPT)
assert SPEC is not None and SPEC.loader is not None
checker = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = checker
SPEC.loader.exec_module(checker)


class ThemeModelCheckerTests(unittest.TestCase):
    def make_root(self) -> Path:
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        root = Path(temp.name)
        (root/"src/theme").mkdir(parents=True)
        (root/"include/qtmaterial/theme").mkdir(parents=True)
        return root

    def populate_valid(self, root: Path) -> None:
        for name in checker.MODEL_SOURCE_NAMES:
            text = "// ThemeModel source\n"
            if name == "qtmaterialthemeidentity.cpp":
                text += (
                    "equalColorScheme equalTypography equalComponentOverrides "
                    "lhs.options() == rhs.options()\n"
                )
            (root/"src/theme"/name).write_text(text, encoding="utf-8")
        for name in checker.MODEL_HEADER_NAMES:
            text = "// ThemeModel header\n"
            if name == "qtmaterialtheme.h":
                text += (
                    "Theme(const Theme&) = default;\n"
                    "Theme(Theme&&) = default;\n"
                    "operator==(const Theme& lhs, const Theme& rhs)\n"
                    "operator!=(const Theme& lhs, const Theme& rhs)\n"
                )
            (root/"include/qtmaterial/theme"/name).write_text(text, encoding="utf-8")
        (root/"src/theme/CMakeLists.txt").write_text(
            "qtmaterialthemeidentity.cpp qtmaterialthemeidentity.h\n",
            encoding="utf-8",
        )
        (root/"src/theme/qtmaterialthemecontext.cpp").write_text(
            "if (theme == m_theme) {}\n", encoding="utf-8"
        )
        (root/"include/qtmaterial/theme/qtmaterialthemecontext.h").write_text(
            "Theme m_theme;\n", encoding="utf-8"
        )

    def test_valid_contract_passes(self):
        root = self.make_root()
        self.populate_valid(root)
        self.assertEqual([], checker.validate_contract(root))

    def test_qobject_in_model_fails(self):
        root = self.make_root()
        self.populate_valid(root)
        p = root/"include/qtmaterial/theme/qtmaterialtheme.h"
        p.write_text(p.read_text(encoding="utf-8") + "QObject *bad;\n", encoding="utf-8")
        errors = checker.validate_contract(root)
        self.assertTrue(any("forbidden ThemeModel token QObject" in e for e in errors))

    def test_serializer_in_model_fails(self):
        root = self.make_root()
        self.populate_valid(root)
        p = root/"src/theme/qtmaterialtheme.cpp"
        p.write_text('#include "qtmaterialthemeserializer.h"\nThemeSerializer\n', encoding="utf-8")
        errors = checker.validate_contract(root)
        self.assertTrue(any("ThemeSerializer" in e for e in errors))

    def test_context_serializer_identity_fails(self):
        root = self.make_root()
        self.populate_valid(root)
        p = root/"src/theme/qtmaterialthemecontext.cpp"
        p.write_text("ThemeSerializer stableFingerprint\n", encoding="utf-8")
        errors = checker.validate_contract(root)
        self.assertTrue(any("IO identity" in e for e in errors))

    def test_missing_move_semantics_fails(self):
        root = self.make_root()
        self.populate_valid(root)
        p = root/"include/qtmaterial/theme/qtmaterialtheme.h"
        p.write_text(
            p.read_text(encoding="utf-8").replace("Theme(Theme&&) = default;\n", ""),
            encoding="utf-8",
        )
        errors = checker.validate_contract(root)
        self.assertTrue(any("Theme(Theme&&)" in e for e in errors))


if __name__ == "__main__":
    unittest.main()
