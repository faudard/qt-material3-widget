from __future__ import annotations

import importlib.util
import sys
import tempfile
import unittest
from pathlib import Path


def load(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


ROOT = Path(__file__).resolve().parents[2]
manifest = load(
    "qtm3_header_surface_domain_test",
    ROOT / "tools/header_surface.py",
)
surface = load(
    "qtm3_api_surface_test",
    ROOT / "tools/check_api_surface.py",
)


class HeaderSurfaceTests(unittest.TestCase):
    def make_root(self) -> Path:
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        root = Path(temp.name)
        for d in [
            "include/qtmaterial/core/private",
            "include/qtmaterial/widgets",
            "cmake",
            "scripts/ci",
            "tests/consumer",
            "tests/consumers/installed",
            "tools",
        ]:
            (root / d).mkdir(parents=True, exist_ok=True)

        (root / "tools/header_surface.py").write_text(
            (ROOT / "tools/header_surface.py").read_text(
                encoding="utf-8"
            ),
            encoding="utf-8",
        )
        return root

    def write_valid_install_contract(self, root: Path) -> None:
        (root / "CMakeLists.txt").write_text(
            'include("cmake/QtMaterial3ApiChecks.cmake")\n'
            "qtmaterial3_install_public_headers()\n"
            "qtmaterial3_add_api_checks()\n"
            "install(EXPORT QtMaterial3WidgetsTargets)\n"
            "configure_package_config_file(input output)\n"
            "write_basic_package_version_file(output)\n",
            encoding="utf-8",
        )
        (root / "cmake/QtMaterial3ApiChecks.cmake").write_text(
            "set(QTMATERIAL3_PUBLIC_HEADERS public.h)\n"
            "function(qtmaterial3_install_public_headers)\n"
            "  install(FILES public.h DESTINATION include)\n"
            "endfunction()\n"
            "foreach(header IN LISTS QTMATERIAL3_PUBLIC_HEADERS)\n"
            "endforeach()\n",
            encoding="utf-8",
        )
        (root / "scripts/ci/run-consumer-matrix.py").write_text(
            'checker = "check_api_surface.py"\n'
            'scope = "--scope installed"\n'
            'prefix_option = "--prefix"\n',
            encoding="utf-8",
        )
        (root / "tests/consumer/run_installed_package_test.cmake").write_text(
            "set(checker check_api_surface.py)\n"
            "set(scope installed)\n"
            "set(prefix ${INSTALL_PREFIX})\n",
            encoding="utf-8",
        )
        (root / "tests/CMakeLists.txt").write_text(
            "set(REPO_SOURCE_DIR source)\n"
            "set(PYTHON_EXECUTABLE python)\n",
            encoding="utf-8",
        )
        (root / "tests/consumers/installed/CMakeLists.txt").write_text(
            "find_package(QtMaterial3Widgets REQUIRED CONFIG)\n"
            "target_link_libraries(consumer PRIVATE QtMaterial3::Widgets)\n",
            encoding="utf-8",
        )

    def test_classification(self):
        root = self.make_root()
        (root / "include/qtmaterial/widgets/public.h").write_text(
            "#pragma once\n", encoding="utf-8"
        )
        (root / "include/qtmaterial/core/private/helper_p.h").write_text(
            "#pragma once\n", encoding="utf-8"
        )

        public, private = manifest.scan(root)
        self.assertEqual(["qtmaterial/widgets/public.h"], public)
        self.assertEqual(
            ["qtmaterial/core/private/helper_p.h"],
            private,
        )

    def test_manifest_detects_new_unclassified_header(self):
        root = self.make_root()
        pub = root / "include/qtmaterial/widgets/public.h"
        pub.write_text("#pragma once\n", encoding="utf-8")

        manifest_path = root / "cmake/QtMaterial3HeaderSurfaceManifest.cmake"
        manifest_path.write_text(
            manifest.render([], []),
            encoding="utf-8",
        )
        errors = manifest.validate_manifest(root, manifest_path)
        self.assertTrue(any("unclassified/new public headers" in e for e in errors))

    def test_install_contract_accepts_explicit_manifest_setup(self):
        root = self.make_root()
        self.write_valid_install_contract(root)

        self.assertEqual([], surface.check_install_contract(root))

    def test_install_contract_rejects_directory_install(self):
        root = self.make_root()
        self.write_valid_install_contract(root)
        with (root / "CMakeLists.txt").open("a", encoding="utf-8") as stream:
            stream.write("install(DIRECTORY include/ DESTINATION include)\n")

        errors = surface.check_install_contract(root)
        self.assertTrue(any("must not install include/" in error for error in errors))

    def test_install_contract_requires_consumer_surface_check(self):
        root = self.make_root()
        self.write_valid_install_contract(root)
        (root / "scripts/ci/run-consumer-matrix.py").write_text(
            "# missing installed-surface verification\n",
            encoding="utf-8",
        )

        errors = surface.check_install_contract(root)
        self.assertTrue(
            any("consumer matrix runner missing" in error for error in errors),
            errors,
        )

    def test_install_contract_rejects_private_install_loop(self):
        root = self.make_root()
        self.write_valid_install_contract(root)
        (root / "cmake/QtMaterial3ApiChecks.cmake").write_text(
            "set(QTMATERIAL3_PUBLIC_HEADERS public.h)\n"
            "set(QTMATERIAL3_PRIVATE_HEADERS helper_p.h)\n"
            "function(qtmaterial3_install_public_headers)\n"
            "  install(FILES public.h DESTINATION include)\n"
            "endfunction()\n"
            "foreach(header IN LISTS QTMATERIAL3_PRIVATE_HEADERS)\n"
            "  install(FILES ${header} DESTINATION include)\n"
            "endforeach()\n",
            encoding="utf-8",
        )

        errors = surface.check_install_contract(root)
        self.assertTrue(any("must never be installed" in error for error in errors))

    def test_public_header_cannot_include_private(self):
        root = self.make_root()
        public = root / "include/qtmaterial/widgets/public.h"
        private = root / "include/qtmaterial/core/private/helper_p.h"
        public.write_text(
            '#include <qtmaterial/core/private/helper_p.h>\n',
            encoding="utf-8",
        )
        private.write_text("#pragma once\n", encoding="utf-8")

        errors = surface.check_public_includes(
            root,
            ["qtmaterial/widgets/public.h"],
            ["qtmaterial/core/private/helper_p.h"],
        )
        self.assertTrue(errors)

    def test_public_header_cannot_include_relative_private(self):
        root = self.make_root()
        public = root / "include/qtmaterial/widgets/public.h"
        public.write_text(
            '#include "../core/private/helper_p.h"\n',
            encoding="utf-8",
        )

        errors = surface.check_public_includes(
            root,
            ["qtmaterial/widgets/public.h"],
            [],
        )
        self.assertTrue(
            any("private-looking header" in error for error in errors),
            errors,
        )

    def test_public_header_cannot_include_source_tree(self):
        root = self.make_root()
        public = root / "include/qtmaterial/widgets/public.h"
        public.write_text(
            '#include "../../../src/core/helper.h"\n',
            encoding="utf-8",
        )

        errors = surface.check_public_includes(
            root,
            ["qtmaterial/widgets/public.h"],
            [],
        )
        self.assertTrue(
            any("source implementation" in error for error in errors),
            errors,
        )

    def test_installed_private_header_is_rejected(self):
        root = self.make_root()
        (root / "include/qtmaterial/widgets/public.h").write_text(
            "#pragma once\n", encoding="utf-8"
        )
        (root / "include/qtmaterial/core/private/helper_p.h").write_text(
            "#pragma once\n", encoding="utf-8"
        )
        manifest_path = root / "cmake/QtMaterial3HeaderSurfaceManifest.cmake"
        public, private = manifest.scan(root)
        manifest_path.write_text(
            manifest.render(public, private),
            encoding="utf-8",
        )

        prefix = root / "prefix"
        for rel in public + private:
            dst = prefix / "include" / rel
            dst.parent.mkdir(parents=True, exist_ok=True)
            dst.write_text("#pragma once\n", encoding="utf-8")

        # installed.validate loads the helper from root/tools.
        errors = surface.validate_installed(root, prefix)
        self.assertTrue(any("private headers leaked" in e for e in errors))

    def test_installed_check_rejects_invalid_source_manifest(self):
        root = self.make_root()
        (root / "include/qtmaterial/widgets/public.h").write_text(
            "#pragma once\n", encoding="utf-8"
        )
        manifest_path = root / "cmake/QtMaterial3HeaderSurfaceManifest.cmake"
        manifest_path.write_text(manifest.render([], []), encoding="utf-8")

        errors = surface.validate_installed(root, root / "prefix")
        self.assertTrue(any("source manifest invalid" in error for error in errors))

    def test_exact_installed_public_surface_passes(self):
        root = self.make_root()
        (root / "include/qtmaterial/widgets/public.h").write_text(
            "#pragma once\n", encoding="utf-8"
        )
        public, private = manifest.scan(root)
        manifest_path = root / "cmake/QtMaterial3HeaderSurfaceManifest.cmake"
        manifest_path.write_text(
            manifest.render(public, private),
            encoding="utf-8",
        )

        prefix = root / "prefix"
        for rel in public:
            dst = prefix / "include" / rel
            dst.parent.mkdir(parents=True, exist_ok=True)
            dst.write_text("#pragma once\n", encoding="utf-8")

        self.assertEqual([], surface.validate_installed(root, prefix))


if __name__ == "__main__":
    unittest.main()
