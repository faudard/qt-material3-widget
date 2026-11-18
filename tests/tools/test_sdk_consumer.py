"""Unit tests for the production SDK archive and relocation helpers."""
import hashlib
import importlib.util
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[2]
SPEC = importlib.util.spec_from_file_location(
    "sdk_consumer", ROOT / "scripts/ci/run-consumer-matrix.py")
runner = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(runner)


class SdkConsumerTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)

    def make_prefix(self, name):
        prefix = self.root / name
        (prefix / "include/qtmaterial").mkdir(parents=True)
        config = prefix / "lib/cmake/QtMaterial3Widgets/QtMaterial3WidgetsConfig.cmake"
        config.parent.mkdir(parents=True)
        config.touch()
        return prefix

    def test_locates_nested_prefix(self):
        prefix = self.make_prefix("package wrapper/sdk")
        self.assertEqual(runner.extracted_prefix(self.root), prefix)

    def test_rejects_missing_config(self):
        with self.assertRaises(RuntimeError):
            runner.extracted_prefix(self.root)

    def test_rejects_multiple_configs(self):
        self.make_prefix("one")
        self.make_prefix("two")
        with self.assertRaises(RuntimeError):
            runner.package_config(self.root)

    def test_sha256_rejects_corruption(self):
        archive = self.root / "sdk.zip"
        archive.write_bytes(b"original")
        sidecar = archive.with_name(archive.name + ".sha256")
        sidecar.write_text(hashlib.sha256(b"original").hexdigest() + "  sdk.zip\n")
        runner.verify_archive_checksum(archive)
        archive.write_bytes(b"corrupt")
        with self.assertRaisesRegex(RuntimeError, "SHA256 mismatch"):
            runner.verify_archive_checksum(archive)

    def test_missing_checksum_fails(self):
        archive = self.root / "sdk.zip"
        archive.touch()
        with self.assertRaises(OSError):
            runner.verify_archive_checksum(archive)

    def test_archive_consumer_receives_relocated_prefix(self):
        archive = self.root / "sdk.zip"
        archive.write_bytes(b"fixture")
        archive.with_name("sdk.zip.sha256").write_text(
            hashlib.sha256(b"fixture").hexdigest())
        original = self.root / "build/ archive-placeholder"
        def extract(command, *, cwd):
            nonlocal original
            original = cwd / "package"
            (original / "include/qtmaterial").mkdir(parents=True)
            config = original / "lib/cmake/QtMaterial3Widgets/QtMaterial3WidgetsConfig.cmake"
            config.parent.mkdir(parents=True)
            config.touch()
        with patch.object(runner, "run", side_effect=extract), \
                patch.object(runner, "check_installed_consumer") as consumer:
            runner.execute_archive(archive, build_root=self.root / "build",
                                   generator="Ninja", config="Release")
        relocated = self.root / "build/archive-sdk.zip/relocated SDK"
        self.assertFalse(original.exists())
        self.assertTrue(relocated.is_dir())
        consumer.assert_called_once_with(relocated, relocated.parent,
                                         generator="Ninja", config="Release")


if __name__ == "__main__":
    unittest.main()
