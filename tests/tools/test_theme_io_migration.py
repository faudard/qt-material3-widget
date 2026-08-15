from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path

MIG = Path(__file__).resolve().parents[2]/"tools/migrate_theme_text_keys.py"
spec = importlib.util.spec_from_file_location("qtm3_migrate_keys", MIG)
migrate = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = migrate
spec.loader.exec_module(migrate)


class ThemeTextMigrationTests(unittest.TestCase):
    def test_button_aliases_become_typed_ids(self):
        source = """applyButtonComponentTokens(
            theme,
            QStringList{
                QStringLiteral("button"),
                QStringLiteral("button.filled"),
                QStringLiteral("FilledButton")},
            &spec);
        """
        converted, unknown = migrate.convert_text(source)
        self.assertEqual([], unknown)
        self.assertIn("ComponentId::Button", converted)
        self.assertIn("ComponentId::ButtonFilled", converted)
        self.assertNotIn("QStringList", converted)

    def test_textfield_aliases_deduplicate(self):
        source = """QStringList{
            QStringLiteral("textField.outlined"),
            QStringLiteral("OutlinedTextField")}"""
        converted, unknown = migrate.convert_text(source)
        self.assertEqual([], unknown)
        self.assertEqual(1, converted.count("ComponentId::TextFieldOutlined"))

    def test_unknown_alias_fails_closed(self):
        source = 'QStringList{QStringLiteral("futureWidget.magic")}'
        converted, unknown = migrate.convert_text(source)
        self.assertEqual(["futureWidget.magic"], unknown)
        self.assertIn("QStringList", converted)

    def test_autocomplete_umbrella_and_popup(self):
        source = """QStringList{
          QStringLiteral("input"),
          QStringLiteral("autocomplete"),
          QStringLiteral("autocompletePopup"),
          QStringLiteral("AutocompletePopup")}"""
        converted, unknown = migrate.convert_text(source)
        self.assertEqual([], unknown)
        self.assertIn("ComponentId::Input", converted)
        self.assertIn("ComponentId::AutoComplete", converted)
        self.assertIn("ComponentId::AutoCompletePopup", converted)


if __name__ == "__main__":
    unittest.main()
