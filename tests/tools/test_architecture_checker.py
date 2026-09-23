from __future__ import annotations

import importlib.util
import json
import sys
import tempfile
import unittest
from pathlib import Path

SCRIPT = (
    Path(__file__).resolve().parents[2]
    / "scripts"
    / "architecture"
    / "check_architecture.py"
)
SPEC = importlib.util.spec_from_file_location("qtm3_architecture", SCRIPT)
assert SPEC is not None and SPEC.loader is not None
checker = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = checker
SPEC.loader.exec_module(checker)


class ArchitectureCheckerTests(unittest.TestCase):
    def test_scoped_cpp_symbols_ignore_comments_and_strings(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            source = root / "src" / "example.cpp"
            source.parent.mkdir(parents=True)
            source.write_text(
                '// QWidget in a comment\n'
                'const char *name = "ThemeManager";\n'
                "int value = 0;\n",
                encoding="utf-8",
            )
            rule = {
                "name": "symbols",
                "paths": ["src/**"],
                "checks": [
                    {
                        "input": "symbols",
                        "regex": r"\b(?:QWidget|ThemeManager)\b",
                    }
                ],
            }
            self.assertEqual(
                [],
                checker.check_scoped_cpp_rule(root.resolve(), rule),
            )

            source.write_text(
                "QWidget *widget = nullptr;\n",
                encoding="utf-8",
            )
            violations = checker.check_scoped_cpp_rule(
                root.resolve(), rule
            )
            self.assertEqual(1, len(violations))

    def test_repository_text_rule_detects_suppression_marker(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            source = root / "src" / "example.cpp"
            source.parent.mkdir(parents=True)
            source.write_text(
                "// ARCH-IGNORE: temporary\n",
                encoding="utf-8",
            )
            rule = {
                "name": "no-suppressions",
                "paths": ["src"],
                "forbidden_regex": [r"ARCH[-_ ]?IGNORE"],
            }
            violations = checker.check_repository_text_rule(
                root.resolve(), rule
            )
            self.assertEqual(1, len(violations))

    def test_cmake_target_graph_checks_required_and_forbidden_edges(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            (root / "CMakeLists.txt").write_text(
                "add_library(layer_a INTERFACE)\n"
                "add_library(layer_b INTERFACE)\n"
                "target_link_libraries(layer_a INTERFACE layer_b)\n",
                encoding="utf-8",
            )
            rule = {
                "name": "target-graph",
                "required_targets": [
                    "layer_a",
                    "layer_b",
                    "missing_layer",
                ],
                "forbidden_links": {
                    "layer_a": ["layer_b"],
                },
            }
            violations = checker.check_cmake_target_rule(
                root.resolve(), rule
            )
            messages = [item.message for item in violations]
            self.assertTrue(
                any("missing_layer" in message for message in messages)
            )
            self.assertTrue(
                any("must not link layer_b" in message for message in messages)
            )

    def test_rules_loader_accepts_consolidated_rule_categories(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "rules.json"
            path.write_text(
                json.dumps(
                    {
                        "schema_version": 1,
                        "scoped_cpp_rules": [],
                        "repository_text_rules": [],
                        "cmake_target_rules": [],
                    }
                ),
                encoding="utf-8",
            )
            loaded = checker.load_rules(path)
            self.assertEqual(1, loaded["schema_version"])


if __name__ == "__main__":
    unittest.main()
