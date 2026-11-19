#!/usr/bin/env python3
"""Prepare and record real 1.22 screen-reader sessions; never auto-pass a check."""
from __future__ import annotations

import argparse
import copy
import json
import re
import subprocess
from pathlib import Path

import certification_center as center

ROOT = center.ROOT
SCENARIOS_PATH = Path(__file__).with_name("native_at_scenarios_1_22.json")
PLATFORMS = ("windows-nvda", "linux-orca", "macos-voiceover")
EXECUTABLES = {
    "1.5": "qtmaterial3_accessibility_certification",
    "1.7": "qtmaterial3_catalogue_certification",
    "1.9": "qtmaterial3_adaptive_desktop_certification",
    "1.10/1.11": "qtmaterial3_gallery",
}
GALLERY_EXERCISES = {
    "button.split": "Disable split button; inspect Last expressive button action.",
    "button.group": "Disable Week segment; inspect Last expressive button action; use Direction in theme toolbar.",
    "navigation.floating-toolbar": "Collapse/Vertical/Disable floating toolbar controls; inspect last navigation action.",
    "progress.loading-indicator": "Loading active and Reduced motion checkboxes.",
    "data.segmented-list": "Disable Work item, Remove last segment and Restore three segments controls.",
    "navigation.menu": "Use the Expressive menu next to Floating Toolbar on Navigation page; Use standard menu styling toggles the variant.",
    "compact.chip": "Expressive chips, Disable filter chip and Restore input chip controls.",
}
METADATA = ("osVersion", "screenReaderVersion", "qtVersion", "locale", "sourceCommit")


def scenarios():
    return json.loads(SCENARIOS_PATH.read_text(encoding="utf-8"))


def current_commit(root=ROOT):
    return subprocess.check_output(
        ["git", "rev-parse", "HEAD"], cwd=root, text=True).strip()


def create_session(milestone, platform, ledger):
    session = center.build_session_template(milestone, platform, ledger)
    session["schemaVersion"] = 2
    session["environment"] = dict.fromkeys(METADATA, "")
    session["observations"] = {
        cid: dict.fromkeys(checks, "") for cid, checks in session["checks"].items()
    }
    return session


def worklist(milestone, platform, session, routes):
    procedures = scenarios()
    lines = [
        f"# {milestone} — {session['screenReader']} ({platform})", "",
        "Use the real reader with the existing interactive demo. A Qt accessibility",
        "tree, CTest pass, screenshot or simulated speech is not native AT evidence.", "",
        "Record OS, reader version, Qt version, locale and the source commit of the tested build.",
        "Record the actual spoken words and observed behavior for every check; mark fail",
        "when expected semantics are absent. If the fixture cannot exercise a case, leave",
        "it pending and describe the missing fixture. Do not substitute an assumed pass.", "",
        f"Preferred executable: `{EXECUTABLES[milestone]}`", "",
        "In the downloaded CI bundle see README.txt and source.json. Record the exact tested source SHA.", "",
    ]
    for cid, checks in session["checks"].items():
        route = routes[cid]
        lines += [f"## {cid}", ""]
        if milestone == "1.10/1.11":
            if cid == "navigation.menu":
                route = routes["navigation.floating-toolbar"]
            lines += [f"Launch: `qtmaterial3_gallery --route {route}`", "",
                      GALLERY_EXERCISES[cid], ""]
        else:
            lines += [f"Open the {cid} section in {EXECUTABLES[milestone]}.",
                      "", f"Gallery fallback: `{route}`", ""]
        for check in checks:
            instruction = procedures.get(cid, {}).get(check)
            if not instruction:
                raise ValueError(f"No native AT procedure for {cid}/{check}")
            lines += [f"### {check}", "", instruction, "",
                      "Result: pending", "", "Actual announcement / behavior:", ""]
    return "\n".join(lines)


def prepare(output, root=ROOT):
    registry = json.loads((root / "docs/components/component-registry.json").read_text(encoding="utf-8"))
    routes = {row["id"]: row["galleryRoute"] for row in registry}
    payloads = []
    for milestone in center.MILESTONES:
        ledger = center.load_ledger(milestone, root)
        for platform in PLATFORMS:
            session = create_session(milestone, platform, ledger)
            stem = milestone.replace("/", "-") + "-" + platform
            payloads.append((stem, session, worklist(milestone, platform, session, routes)))
    output.mkdir(parents=True, exist_ok=True)
    paths = [output / (stem + suffix) for stem, _, _ in payloads
             for suffix in (".json", ".md")]
    if any(path.exists() for path in paths):
        raise FileExistsError("Review files already exist; choose a fresh output folder")
    for stem, session, guide in payloads:
        (output / (stem + ".json")).write_text(json.dumps(session, indent=2) + "\n", encoding="utf-8")
        (output / (stem + ".md")).write_text(guide + "\n", encoding="utf-8")
    return len(payloads)


def validate(session, ledger):
    if not isinstance(session, dict):
        return ["session must be an object"]
    errors = []
    if session.get("schemaVersion") != 2:
        errors.append("detailed native session must use schemaVersion 2")
    legacy = copy.deepcopy(session)
    legacy["schemaVersion"] = 1
    errors.extend(center.validate_session(legacy, ledger, require_final_results=True))
    env = session.get("environment")
    if not isinstance(env, dict):
        errors.append("missing environment")
    else:
        for key in METADATA:
            value = env.get(key)
            if not isinstance(value, str) or not value.strip():
                errors.append(f"environment/{key} must be non-empty")
        if not re.fullmatch(r"[0-9a-f]{40}", str(env.get("sourceCommit", ""))):
            errors.append("environment/sourceCommit must be a full Git SHA")
    checks, observations = session.get("checks"), session.get("observations")
    if not isinstance(checks, dict) or not isinstance(observations, dict):
        return errors + ["observations must match the component/check contract"]
    if set(checks) != set(observations):
        errors.append("observation component set does not match checks")
    for cid, values in checks.items():
        obs = observations.get(cid)
        if not isinstance(values, dict) or not isinstance(obs, dict) or set(obs) != set(values):
            errors.append(f"{cid}: observation keys must match checks")
            continue
        for key, observed in obs.items():
            if not isinstance(observed, str) or not observed.strip():
                errors.append(f"{cid}/{key}: actual spoken output or behavior required")
    return errors


def record(session, root=ROOT, *, replace=False):
    milestone = session.get("milestone")
    if milestone not in center.MILESTONES:
        raise ValueError("unknown milestone")
    ledger = center.load_ledger(milestone, root)
    errors = validate(session, ledger)
    if errors:
        raise ValueError("; ".join(errors))
    # A different commit may contain evidence-only edits, but must not change the
    # production code, public headers, Gallery, build files or test fixtures.
    subprocess.run(["git", "diff", "--quiet", session["environment"]["sourceCommit"],
                    current_commit(root), "--", "src", "include", "examples",
                    "tests", "CMakeLists.txt", "cmake",
                    ":(exclude)tests/visual/goldens",
                    ":(exclude)tests/tools"], cwd=root, check=True)
    relative = Path("docs/components/native-at-1.22") / (
        milestone.replace("/", "-") + "-" + session["platform"] + ".json")
    path = root / relative
    if path.exists() and not replace:
        raise FileExistsError(f"{relative} exists; use --replace to record a new actual run")
    legacy = copy.deepcopy(session)
    legacy["schemaVersion"] = 1
    legacy["evidence"] = f"{relative.as_posix()}; {session['evidence']}"
    updated = center.apply_session(ledger, legacy)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(session, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    center.save_ledger(milestone, updated, root)
    return updated["platforms"][session["platform"]]["status"]


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    export = commands.add_parser("prepare")
    export.add_argument("--output", required=True, type=Path)
    for name in ("validate", "record"):
        cmd = commands.add_parser(name)
        cmd.add_argument("--session", required=True, type=Path)
        if name == "record":
            cmd.add_argument("--confirm-native-session", action="store_true")
            cmd.add_argument("--replace", action="store_true")
    args = parser.parse_args(argv)
    try:
        if args.command == "prepare":
            print(f"Prepared {prepare(args.output)} native sessions; results remain pending.")
            return 0
        session = json.loads(args.session.read_text(encoding="utf-8"))
        if not isinstance(session, dict) or session.get("milestone") not in center.MILESTONES:
            raise ValueError("invalid session milestone")
        if args.command == "validate":
            errors = validate(session, center.load_ledger(session["milestone"]))
            if errors:
                raise ValueError("; ".join(errors))
            print("Session structure complete; this does not prove actual native speech.")
        else:
            if not args.confirm_native_session:
                raise ValueError("actual native session confirmation is mandatory")
            result = record(session, replace=args.replace)
            print(f"Recorded native session: {result}; registry unchanged")
    except (OSError, ValueError, KeyError, TypeError, subprocess.CalledProcessError) as error:
        parser.exit(1, f"Native AT review rejected: {error}\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
