from pathlib import Path
import os
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
RUNNER = ROOT / "scripts/ci/run-with-openbox.py"


@unittest.skipUnless(os.name == "posix", "The X11 CI runner is POSIX-only")
class WindowManagerRunnerTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.directory = Path(self.temp.name)
        self.env = {**os.environ, "PATH": str(self.directory) + os.pathsep + os.environ["PATH"],
                    "WM_TEST_DIR": str(self.directory), "DISPLAY": ":99"}
        self.executable("openbox", """
import os, signal, sys, time
from pathlib import Path
directory = Path(os.environ['WM_TEST_DIR'])
if os.environ.get('WM_TEST_CRASH'):
    print('mock Openbox startup failed', flush=True)
    raise SystemExit(9)
def stop(*args):
    (directory / 'stopped').write_text('stopped')
    raise SystemExit(0)
signal.signal(signal.SIGTERM, stop)
(directory / 'started').write_text('started')
while True:
    time.sleep(0.01)
""")
        self.executable("xprop", """
import os, sys
from pathlib import Path
directory = Path(os.environ['WM_TEST_DIR'])
# Preflight probes are independent from the EWMH manager-readiness probes.
if sys.argv[1:] == ['-root']:
    path = directory / 'display_probes'
    attempts = int(path.read_text()) + 1 if path.exists() else 1
    path.write_text(str(attempts))
    if os.environ.get('WM_TEST_XSERVER_NEVER') or attempts <= int(os.environ.get('WM_TEST_XSERVER_DELAY', '0')):
        print('xprop: unable to open display', file=sys.stderr)
        raise SystemExit(1)
counter = directory / 'probes'
if '-root' in sys.argv:
    probes = int(counter.read_text()) + 1 if counter.exists() else 1
    counter.write_text(str(probes))
    owner = '0x42' if probes >= 2 and (directory / 'started').exists() else '0x0'
elif os.environ.get('WM_TEST_STALE'):
    owner = '0x43'
else:
    owner = '0x42'
if os.environ.get('WM_TEST_NEVER_READY'):
    owner = '0x0'
print('_NET_SUPPORTING_WM_CHECK(WINDOW): window id # ' + owner)
""")

    def executable(self, name, source):
        path = self.directory / name
        path.write_text('#!' + sys.executable + '\n' + source)
        path.chmod(0o755)

    def run_client(self, exit_code=0, startup_timeout=2):
        client = """
import os
from pathlib import Path
directory = Path(os.environ['WM_TEST_DIR'])
assert int((directory / 'probes').read_text()) >= 2
assert (directory / 'started').exists()
(directory / 'client').write_text('ran')
raise SystemExit(%d)
""" % exit_code
        return subprocess.run(
            [sys.executable, str(RUNNER), '--startup-timeout', str(startup_timeout), '--', sys.executable, '-c', client],
            env=self.env, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=5,
        )

    def test_client_waits_for_manager_and_manager_is_stopped(self):
        result = self.run_client()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertTrue((self.directory / 'client').exists())
        self.assertTrue((self.directory / 'stopped').exists())

    def test_client_exit_code_is_preserved(self):
        result = self.run_client(exit_code=7)
        self.assertEqual(result.returncode, 7, result.stderr)
        self.assertTrue((self.directory / 'stopped').exists())

    def test_crashed_manager_prevents_client_and_reports_startup_log(self):
        self.env['WM_TEST_CRASH'] = '1'
        result = self.run_client()
        self.assertEqual(result.returncode, 1)
        self.assertIn('exit code 9', result.stderr)
        self.assertIn('mock Openbox startup failed', result.stderr)
        self.assertFalse((self.directory / 'client').exists())

    def test_timeout_prevents_client_and_stops_manager(self):
        self.env['WM_TEST_NEVER_READY'] = '1'
        result = self.run_client(startup_timeout=0.5)
        self.assertEqual(result.returncode, 1)
        self.assertIn('did not become ready', result.stderr)
        self.assertFalse((self.directory / 'client').exists())
        self.assertTrue((self.directory / 'stopped').exists())

    def test_stale_root_property_does_not_start_client(self):
        self.env['WM_TEST_STALE'] = '1'
        result = self.run_client(startup_timeout=0.5)
        self.assertEqual(result.returncode, 1)
        self.assertFalse((self.directory / 'client').exists())
        self.assertTrue((self.directory / 'stopped').exists())

    def test_waits_for_x_server_before_starting_openbox(self):
        self.env['WM_TEST_XSERVER_DELAY'] = '2'
        result = self.run_client()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertGreaterEqual(int((self.directory / 'display_probes').read_text()), 3)
        self.assertTrue((self.directory / 'client').exists())
        self.assertTrue((self.directory / 'stopped').exists())

    def test_unavailable_x_server_prevents_manager_and_client(self):
        self.env['WM_TEST_XSERVER_NEVER'] = '1'
        result = self.run_client(startup_timeout=0.5)
        self.assertEqual(result.returncode, 1)
        self.assertIn('X display', result.stderr)
        self.assertIn('did not become ready', result.stderr)
        self.assertFalse((self.directory / 'started').exists())
        self.assertFalse((self.directory / 'client').exists())

    def test_missing_display_is_reported_before_openbox(self):
        del self.env['DISPLAY']
        result = self.run_client()
        self.assertEqual(result.returncode, 1)
        self.assertIn('DISPLAY is unset', result.stderr)
        self.assertFalse((self.directory / 'started').exists())
