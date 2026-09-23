from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TOOLS = ROOT / "tools"
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))

SPEC = importlib.util.spec_from_file_location(
    "qtm3_theme_checker",
    TOOLS / "check_theme.py",
)
assert SPEC is not None and SPEC.loader is not None
checker = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = checker
SPEC.loader.exec_module(checker)


class ThemeCheckerTests(unittest.TestCase):
    def test_current_checkout_satisfies_all_theme_contracts(self):
        results = checker.validate(ROOT)
        self.assertEqual(set(checker.VALIDATORS), set(results))
        self.assertEqual(
            {},
            {name: errors for name, errors in results.items() if errors},
        )

    def test_scope_selection(self):
        results = checker.validate(ROOT, ["model", "tokens"])
        self.assertEqual(["model", "tokens"], list(results))


if __name__ == "__main__":
    unittest.main()
