from __future__ import annotations
import importlib.util
import sys
import unittest
from pathlib import Path

MIG = Path(__file__).resolve().parents[2]/"tools/migrate_serializer_token_names.py"
SPEC = importlib.util.spec_from_file_location("qtm3_token_migrator", MIG)
migrate = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = migrate
SPEC.loader.exec_module(migrate)

class SerializerTokenMigrationTests(unittest.TestCase):
    def test_color_table_and_loop_migrate(self):
        source = """
#include "qtmaterial/theme/qtmaterialthemeserializer.h"
const std::array kColorRoles = {
    EnumNamePair<ColorRole>{ ColorRole::Primary, "Primary" },
};
QJsonObject colorSchemeToJson(const ColorScheme& scheme)
{
    QJsonObject object;
    for (const auto& [role, name] : kColorRoles) {
        object.insert(QString::fromLatin1(name), 1);
    }
    return object;
}
"""
        converted, errors = migrate.migrate_text(source)
        self.assertEqual([], errors)
        self.assertNotIn("kColorRoles", converted)
        self.assertIn("allColorRoles()", converted)
        self.assertIn(
            "ThemeTextCodec::tokenIdToString(tokenId(role))", converted)

    def test_unknown_shape_fails_closed(self):
        source = """
#include "qtmaterial/theme/qtmaterialthemeserializer.h"
const std::array kColorRoles =
    makeSomethingGeneratedAtRuntime();
QJsonObject colorSchemeToJson(const ColorScheme& scheme)
{
    return {};
}
"""
        _converted, errors = migrate.migrate_text(source)
        self.assertTrue(errors)

if __name__ == "__main__":
    unittest.main()
