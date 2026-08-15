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
    "qtm3_header_manifest_test",
    ROOT / "tools/update_header_surface_manifest.py",
)
surface = load(
    "qtm3_header_surface_test",
    ROOT / "tools/check_public_private_headers.py",
)
installed = load(
    "qtm3_installed_header_surface_test",
    ROOT / "tools/check_installed_header_surface.py",
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
            "tools",
        ]:
            (root / d).mkdir(parents=True, exist_ok=True)

        # Make helper importable from the temporary root.
        (root / "tools/update_header_surface_manifest.py").write_text(
            (ROOT / "tools/update_header_surface_manifest.py").read_text(
                encoding="utf-8"
            ),
            encoding="utf-8",
        )
        return root

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
        errors = installed.validate(root, prefix)
        self.assertTrue(any("private headers leaked" in e for e in errors))

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

        self.assertEqual([], installed.validate(root, prefix))


if __name__ == "__main__":
    unittest.main()
