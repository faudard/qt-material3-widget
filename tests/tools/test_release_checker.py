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

    def test_api_freeze_rejects_duplicate_widget_header_ownership(self):
        root, rules = self.make_root()
        rules["api_freeze"]["public_support_headers"] = [
            "qtmaterial/widgets/qtmaterialfoo.h"
        ]
        errors = check_release.validate_api_freeze(root, rules)
        self.assertTrue(
            any(
                "component-registry-owned and support-allowlisted" in error
                for error in errors
            ),
            errors,
        )


class EnterpriseReleaseCheckerTests(unittest.TestCase):
    def make_component(self) -> dict:
        axes = {
            axis: 4
            for axis in check_release.component_registry.AXES
        }
        axes.update({
            "lastReviewed": "2026-10-04",
            "gaps": [],
            "nextActions": ["keep evidence current"],
            "evidence": {
                axis: [f"{axis} evidence"]
                for axis in check_release.component_registry.AXES
            },
        })
        return {
            "id": "selection.checkbox",
            "maturity": "complete",
            "maturityPolicy": "derived",
            "releaseScope": True,
            "testTarget": "tst_checkbox",
            "galleryRoute": "/selection",
            "docsPath": "docs/public-api/selection.md",
            "maturityAxes": axes,
        }

    def make_stable(self) -> dict:
        prefix = "selection_matrix"
        return {
            "component_visual_prefixes": {
                "selection.checkbox": prefix,
            },
            "visual_goldens": [
                "tests/visual/goldens/" + prefix + suffix
                for suffix in check_release.ENTERPRISE_VISUAL_SUFFIXES
            ],
        }

    def test_enterprise_component_accepts_complete_4_of_4_evidence(self):
        self.assertEqual(
            [],
            check_release.validate_enterprise_components(
                [self.make_component()],
                self.make_stable(),
            ),
        )

    def test_enterprise_component_rejects_usable_or_sub_4_axis(self):
        component = self.make_component()
        component["maturity"] = "usable"
        component["maturityAxes"]["rendering"] = 3
        errors = check_release.validate_enterprise_components(
            [component],
            self.make_stable(),
        )
        self.assertTrue(any("requires complete" in error for error in errors))
        self.assertTrue(any("rendering=3" in error for error in errors))

    def test_enterprise_component_rejects_open_gaps(self):
        component = self.make_component()
        component["maturityAxes"]["gaps"] = ["visual review pending"]
        errors = check_release.validate_enterprise_components(
            [component],
            self.make_stable(),
        )
        self.assertTrue(any("gaps=[]" in error for error in errors))

    def test_enterprise_component_requires_registered_visual_evidence(self):
        stable = self.make_stable()
        stable["visual_goldens"].pop()
        errors = check_release.validate_enterprise_components(
            [self.make_component()],
            stable,
        )
        self.assertTrue(
            any("reviewed visual golden registration" in error for error in errors)
        )

    def test_enterprise_component_allows_not_applicable_axis(self):
        component = self.make_component()
        component["maturityAxes"]["keyboard"] = "N/A"
        self.assertEqual(
            [],
            check_release.validate_enterprise_components(
                [component],
                self.make_stable(),
            ),
        )

    def make_accessibility_evidence(self, root: Path, *, complete: bool) -> dict:
        relative = "docs/components/enterprise-accessibility-1.5.json"
        (root / "docs/components").mkdir(parents=True, exist_ok=True)
        result = "pass" if complete else "pending"
        platforms = {}
        for platform_id, reader in check_release.ENTERPRISE_AT_PLATFORMS.items():
            platforms[platform_id] = {
                "screenReader": reader,
                "environment": "test",
                "status": result,
                "reviewer": "reviewer" if complete else "",
                "reviewedAt": "2026-10-04" if complete else "",
                "evidence": "artifact://evidence" if complete else "",
                "notes": "",
                "components": {
                    component_id: {
                        check: result
                        for check in check_release.ENTERPRISE_AT_CHECKS
                    }
                    for component_id in check_release.ENTERPRISE_AT_COMPONENTS
                },
            }
        payload = {
            "schemaVersion": 1,
            "certification": "QtMaterial3 1.5 Enterprise accessibility",
            "requiredComponents": list(check_release.ENTERPRISE_AT_COMPONENTS),
            "platforms": platforms,
        }
        (root / relative).write_text(
            json.dumps(payload),
            encoding="utf-8",
        )
        return {"accessibility_evidence": relative}

    def test_enterprise_accessibility_pending_is_valid_before_closure(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            stable = self.make_accessibility_evidence(root, complete=False)
            self.assertEqual(
                [],
                check_release.validate_enterprise_accessibility_evidence(
                    root,
                    stable,
                    require_complete=False,
                ),
            )

    def test_enterprise_accessibility_pending_blocks_final_closure(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            stable = self.make_accessibility_evidence(root, complete=False)
            errors = check_release.validate_enterprise_accessibility_evidence(
                root,
                stable,
                require_complete=True,
            )
            self.assertTrue(
                any("requires pass" in error for error in errors),
                errors,
            )

    def test_enterprise_accessibility_complete_evidence_is_accepted(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            stable = self.make_accessibility_evidence(root, complete=True)
            self.assertEqual(
                [],
                check_release.validate_enterprise_accessibility_evidence(
                    root,
                    stable,
                    require_complete=True,
                ),
            )

    def test_enterprise_accessibility_pass_requires_all_component_checks(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            stable = self.make_accessibility_evidence(root, complete=True)
            path = root / stable["accessibility_evidence"]
            payload = json.loads(path.read_text(encoding="utf-8"))
            payload["platforms"]["windows-nvda"]["components"][
                "navigation.tabs"
            ]["focus"] = "pending"
            path.write_text(json.dumps(payload), encoding="utf-8")
            errors = check_release.validate_enterprise_accessibility_evidence(
                root,
                stable,
                require_complete=True,
            )
            self.assertTrue(
                any("cannot be pass" in error for error in errors),
                errors,
            )


if __name__ == "__main__":
    unittest.main()
