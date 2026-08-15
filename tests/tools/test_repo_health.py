from __future__ import annotations

import importlib.util
import sys
import tempfile
import unittest
from pathlib import Path

SCRIPT = Path(__file__).resolve().parents[2] / "tools" / "repo_health.py"
SPEC = importlib.util.spec_from_file_location("qtm3_repo_health", SCRIPT)
assert SPEC is not None and SPEC.loader is not None
repo_health = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = repo_health
SPEC.loader.exec_module(repo_health)


class WorkflowStructureTests(unittest.TestCase):
    def write_workflow(self, text: str) -> Path:
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        path = Path(temp.name) / "quality.yml"
        path.write_text(text, encoding="utf-8")
        return path

    def test_accepts_complete_workflow_structure(self) -> None:
        path = self.write_workflow(
            "name: Quality\n"
            "on:\n"
            "  pull_request:\n"
            "jobs:\n"
            "  health:\n"
            "    runs-on: ubuntu-latest\n"
        )
        result = repo_health.check_quality_workflow_structure(path)
        self.assertTrue(result.ok, result.detail)

    def test_rejects_current_failure_shape_without_workflow_wrapper(self) -> None:
        path = self.write_workflow(
            "docs:\n"
            "  name: docs\n"
            "  runs-on: ubuntu-latest\n"
        )
        result = repo_health.check_quality_workflow_structure(path)
        self.assertFalse(result.ok)
        self.assertIn("name", result.detail)
        self.assertIn("jobs", result.detail)

    def test_rejects_empty_jobs_mapping(self) -> None:
        path = self.write_workflow(
            "name: Quality\n"
            "on:\n"
            "  pull_request:\n"
            "jobs:\n"
        )
        result = repo_health.check_quality_workflow_structure(path)
        self.assertFalse(result.ok)
        self.assertIn("empty", result.detail)


class CommandCheckTests(unittest.TestCase):
    def test_command_exit_code_is_propagated(self) -> None:
        ok = repo_health.run_command_check(
            "ok", [sys.executable, "-c", "raise SystemExit(0)"], cwd=Path.cwd()
        )
        bad = repo_health.run_command_check(
            "bad", [sys.executable, "-c", "raise SystemExit(7)"], cwd=Path.cwd()
        )
        self.assertTrue(ok.ok)
        self.assertFalse(bad.ok)
        self.assertEqual("exit=7", bad.detail)


if __name__ == "__main__":
    unittest.main()
