from __future__ import annotations
import importlib.util, sys, tempfile, unittest
from pathlib import Path

SCRIPT = Path(__file__).resolve().parents[2]/"tools/check_theme_runtime.py"
SPEC = importlib.util.spec_from_file_location("qtm3_runtime_checker", SCRIPT)
checker = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = checker
SPEC.loader.exec_module(checker)

class ThemeRuntimeCheckerTests(unittest.TestCase):
    def root(self):
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        r = Path(tmp.name)
        for d in ("src/theme","src/core","include/qtmaterial/theme"):
            (r/d).mkdir(parents=True, exist_ok=True)
        return r

    def populate(self, r):
        (r/"src/theme/CMakeLists.txt").write_text(
            "qtmaterialthemecontextdefaults.cpp\n"
            "qtmaterialthemecontextdefaults.h\n"
            "target_link_libraries(qtmaterial3_theme_runtime PUBLIC "
            "qtmaterial3_theme_model Qt6::Core Qt6::Gui)\n",
            encoding="utf-8")
        for n in checker.RUNTIME_HEADERS:
            (r/"include/qtmaterial/theme"/n).write_text("// runtime\n", encoding="utf-8")
        for n in checker.RUNTIME_SOURCES:
            (r/"src/theme"/n).write_text("// runtime\n", encoding="utf-8")
        (r/"src/theme/qtmaterialthememanager.cpp").write_text(
            "return m_defaultContext->revision();\n"
            "theme == m_defaultContext->theme()\n"
            "m_defaultContext->setTheme(theme)\n", encoding="utf-8")
        (r/"src/theme/qtmaterialthemecontextdefaults.cpp").write_text(
            "ThemeManager::instance().defaultContext()\n", encoding="utf-8")
        (r/"src/core/qtmaterialthemecontextbinding.cpp").write_text(
            "ThemeContextDefaults::defaultContext()\nQEvent::ParentChange\n"
            "ThemeContextPropagation::eventType()\n", encoding="utf-8")
        (r/"src/theme/qtmaterialthemeobserver.cpp").write_text(
            "ThemeContextDefaults::defaultContext()\n", encoding="utf-8")
        (r/"src/core/CMakeLists.txt").write_text(
            "        qtmaterial3_theme_model\n        qtmaterial3_theme_runtime\n",
            encoding="utf-8")

    def test_valid(self):
        r = self.root(); self.populate(r)
        self.assertEqual([], checker.validate(r))

    def test_runtime_io_link_fails(self):
        r = self.root(); self.populate(r)
        p = r/"src/theme/CMakeLists.txt"
        p.write_text(p.read_text().replace(
            "qtmaterial3_theme_model",
            "qtmaterial3_theme_model qtmaterial3_theme_io"), encoding="utf-8")
        self.assertTrue(any("must not link ThemeIO" in e for e in checker.validate(r)))

    def test_duplicate_revision_fails(self):
        r = self.root(); self.populate(r)
        (r/"include/qtmaterial/theme/qtmaterialthememanager.h").write_text(
            "quint64 m_revision;\n", encoding="utf-8")
        self.assertTrue(any("second revision" in e for e in checker.validate(r)))

    def test_core_manager_dependency_fails(self):
        r = self.root(); self.populate(r)
        p = r/"src/core/qtmaterialthemecontextbinding.cpp"
        p.write_text(p.read_text()+"ThemeManager::instance();\n", encoding="utf-8")
        self.assertTrue(any("directly on ThemeManager" in e for e in checker.validate(r)))

    def test_observer_manager_dependency_fails(self):
        r = self.root(); self.populate(r)
        (r/"src/theme/qtmaterialthemeobserver.cpp").write_text(
            "ThemeManager::instance();\n", encoding="utf-8")
        self.assertTrue(any("observe ThemeContext" in e for e in checker.validate(r)))

if __name__ == "__main__":
    unittest.main()
