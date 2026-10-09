#!/usr/bin/env python3
"""Run an Xvfb client only after Openbox advertises a live EWMH window."""
from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
import tempfile
import time

PROPERTY = "_NET_SUPPORTING_WM_CHECK"


def window_id(output: str) -> int | None:
    match = re.search(r"window id # (0x[0-9a-fA-F]+)", output)
    if not match:
        return None
    return int(match.group(1), 16) or None


def wait_for_display(timeout: float) -> None:
    """Wait until the X server accepts the same connection Openbox will use.

    xvfb-run can publish DISPLAY before its X server is ready to accept
    connections. Starting Openbox immediately then exits with
    "Failed to open the display", before any Qt/ASan test has run.
    """
    display = os.environ.get("DISPLAY")
    if not display:
        raise RuntimeError("DISPLAY is unset; start this command with xvfb-run")

    deadline = time.monotonic() + timeout
    last_error = ""
    while time.monotonic() < deadline:
        remaining = deadline - time.monotonic()
        try:
            probe = subprocess.run(
                ["xprop", "-root"],
                stdout=subprocess.DEVNULL,
                stderr=subprocess.PIPE,
                text=True,
                timeout=min(1.0, remaining),
                check=False,
            )
            if probe.returncode == 0:
                return
            last_error = probe.stderr.strip()
        except subprocess.TimeoutExpired:
            last_error = "X display probe timed out"
        time.sleep(min(0.1, max(0.0, deadline - time.monotonic())))

    raise RuntimeError(
        f"X display {display!r} did not become ready within {timeout:g} seconds"
        + (f": {last_error}" if last_error else "")
    )


def wait_for_manager(manager: subprocess.Popen, timeout: float) -> None:
    deadline = time.monotonic() + timeout

    def query(*arguments: str) -> str:
        remaining = deadline - time.monotonic()
        if remaining <= 0:
            return ""
        try:
            result = subprocess.run(
                ["xprop", *arguments, PROPERTY], text=True,
                stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
                timeout=min(1.0, remaining), check=False,
            )
            return result.stdout if result.returncode == 0 else ""
        except subprocess.TimeoutExpired:
            return ""

    while time.monotonic() < deadline:
        if manager.poll() is not None:
            raise RuntimeError(f"Openbox exited during startup (exit code {manager.returncode})")
        owner = window_id(query("-root"))
        # EWMH requires a self-reference on the supporting window as well as
        # the root property. A stale root property alone is not readiness.
        if owner and window_id(query("-id", hex(owner))) == owner and manager.poll() is None:
            return
        time.sleep(min(0.05, max(0.0, deadline - time.monotonic())))
    raise RuntimeError(f"Openbox did not become ready within {timeout:g} seconds")


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--startup-timeout", type=float, default=10.0)
    parser.add_argument("command", nargs=argparse.REMAINDER)
    args = parser.parse_args(argv)
    command = args.command[1:] if args.command[:1] == ["--"] else args.command
    if not command or not 0 < args.startup_timeout <= 60:
        parser.error("provide a command and a startup timeout between 0 and 60 seconds")

    manager = None
    with tempfile.TemporaryFile(mode="w+t", encoding="utf-8") as log:
        try:
            wait_for_display(args.startup_timeout)
            manager = subprocess.Popen(["openbox", "--sm-disable"], stdout=log, stderr=log)
            wait_for_manager(manager, args.startup_timeout)
            print("Openbox ready; starting " + command[0], flush=True)
            result = subprocess.call(command)
            return result if result >= 0 else 128 - result
        except (OSError, RuntimeError) as error:
            print(str(error), file=sys.stderr)
            log.seek(0)
            print(log.read(), file=sys.stderr, end="")
            return 1
        except KeyboardInterrupt:
            return 130
        finally:
            if manager is not None and manager.poll() is None:
                manager.terminate()
                try:
                    manager.wait(timeout=3)
                except subprocess.TimeoutExpired:
                    manager.kill()
                    manager.wait()


if __name__ == "__main__":
    raise SystemExit(main())
