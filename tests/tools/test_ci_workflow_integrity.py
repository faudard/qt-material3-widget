from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[2]


class CiWorkflowIntegrityTests(unittest.TestCase):
    def test_ci_jobs_are_unique_and_keep_the_supported_lanes(self):
        workflow = (ROOT / ".github/workflows/ci.yml").read_text()
        jobs = re.findall(r"^  ([a-z][a-z0-9-]*):\s*$", workflow.split("jobs:\n", 1)[1], re.MULTILINE)
        self.assertEqual(len(jobs), len(set(jobs)), "Duplicate CI job identifiers")
        self.assertTrue({"build-test", "designer-plugin", "designer-plugin-qt5-windows", "consumer-contract",
                         "release-package", "examples-benchmarks", "sanitizers", "architecture"}.issubset(jobs))

    def test_workflow_has_no_unindented_script_fragments(self):
        for path in (ROOT / ".github/workflows").glob("*.yml"):
            for number, line in enumerate(path.read_text().splitlines(), 1):
                if line and not line.startswith((" ", "#", "---")):
                    self.assertRegex(line, r"^[A-Za-z][A-Za-z0-9_-]*:", f"{path}:{number}: leaked script text")

    def test_qt514_designer_uses_the_base_archive(self):
        workflow = (ROOT / ".github/workflows/ci.yml").read_text()
        designer = workflow.split("  designer-plugin-qt5-windows:\n", 1)[1].split("\n  consumer-contract:", 1)[0]
        self.assertNotRegex(designer, r"(?m)^\s+modules:\s*.*\bqttools\b",
                            "Qt 5.14.2 does not publish a separate qttools add-on")
        self.assertIn("-DQTMATERIAL3_BUILD_DESIGNER_PLUGIN=ON", designer)

    def test_designer_builds_all_installable_targets_before_staging(self):
        workflow = (ROOT / ".github/workflows/ci.yml").read_text()
        designer = workflow.split("  designer-plugin:\n", 1)[1].split("\n  designer-plugin-qt5-windows:", 1)[0]
        build = designer.split("      - name: Build Designer plugin\n", 1)[1].split("\n      - name:", 1)[0]
        self.assertIn("cmake --build build-designer --parallel", build)
        self.assertNotIn("--target", build,
                         "A plugin-only build omits installable siblings such as ThemeIO")

    def test_powershell_preserves_the_exact_qt_version_argument(self):
        workflow = (ROOT / ".github/workflows/ci.yml").read_text()
        designer = workflow.split("  designer-plugin-qt5-windows:\n", 1)[1].split("\n  consumer-contract:", 1)[0]
        self.assertIn('"-DQTMATERIAL3_EXPECT_QT_VERSION=5.14.2"', designer)

    def test_designer_windows_checks_each_native_exit_code(self):
        workflow = (ROOT / ".github/workflows/ci.yml").read_text()
        designer = workflow.split("      - name: Build and test Designer plugin\n", 1)[1].split("\n  consumer-contract:", 1)[0]
        lines = designer.splitlines()
        for index, line in enumerate(lines):
            if line.strip().startswith(("cmake --build", "ctest ", "cmake --install")):
                self.assertRegex(lines[index + 1], r"if \(\$LASTEXITCODE -ne 0\) \{ throw ",
                                 "A later successful command must not hide Designer failures")

    def test_visual_jobs_wait_for_the_window_manager(self):
        for workflow_name in ("ci.yml", "release-readiness.yml"):
            workflow = (ROOT / ".github/workflows" / workflow_name).read_text()
            self.assertNotIn("bash -c 'openbox", workflow,
                             "CTest must not race the background window-manager startup")
            self.assertIn("python3 scripts/ci/run-with-openbox.py -- ctest", workflow)
        workflow = (ROOT / ".github/workflows/ci.yml").read_text()
        family = workflow.split("      - name: Generate family visual candidate goldens\n", 1)[1].split("\n      - name:", 1)[0]
        self.assertIn("python3 scripts/ci/run-with-openbox.py -- ctest", family)
        for setting in ('QT_QPA_PLATFORM: xcb', 'QT_SCALE_FACTOR: "1"', 'QT_FONT_DPI: "96"', 'LC_ALL: C.UTF-8'):
            self.assertIn(setting, family)
        self.assertIn("x11-utils", (ROOT / "scripts/ci/install-ubuntu-deps.sh").read_text())
