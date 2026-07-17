#!/usr/bin/env python3
"""Staged architecture checker for qt-material3-widget."""

from __future__ import annotations

import argparse
import json
import os
import re
import sys
from pathlib import Path
from typing import Dict, Iterable, List, Sequence, Tuple

CPP_SUFFIXES = {
    ".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".hxx",
    ".inl", ".ipp", ".mm",
}
IGNORED_DIRS = {
    ".git", ".idea", ".qtcreator", ".vscode", "__pycache__",
    "_build", "build", "build-debug", "build-release",
    "cmake-build-debug", "cmake-build-release",
}
INCLUDE_RE = re.compile(
    r'^[ \t]*#[ \t]*include[ \t]*[<"]([^">]+)[">]',
    re.MULTILINE,
)


class CheckError(RuntimeError):
    pass


def find_root(start: Path) -> Path:
    start = start.resolve()
    for candidate in (start,) + tuple(start.parents):
        if (
            (candidate / "CMakeLists.txt").is_file()
            and (candidate / "include" / "qtmaterial").is_dir()
            and (candidate / "src").is_dir()
        ):
            return candidate
    raise CheckError("Could not find repository root from {0}".format(start))


def load_rules(path: Path) -> Dict[str, object]:
    try:
        value = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as error:
        raise CheckError("Rules file not found: {0}".format(path)) from error
    except json.JSONDecodeError as error:
        raise CheckError(
            "Invalid rules JSON at line {0}: {1}".format(
                error.lineno, error.msg
            )
        ) from error
    if not isinstance(value, dict):
        raise CheckError("Rules root must be an object")
    return value


def ignored(path: Path) -> bool:
    return any(part in IGNORED_DIRS for part in path.parts)


def iter_cpp(root: Path, relative_paths: Sequence[str]) -> Iterable[Path]:
    seen = set()
    for relative in relative_paths:
        target = root / relative
        if not target.exists():
            continue
        candidates = (target,) if target.is_file() else target.rglob("*")
        for path in candidates:
            if (
                not path.is_file()
                or path.suffix.lower() not in CPP_SUFFIXES
                or ignored(path.relative_to(root))
                or path in seen
            ):
                continue
            seen.add(path)
            yield path


def line_at(text: str, offset: int) -> int:
    return text.count("\n", 0, offset) + 1


def compile_patterns(
    raw: object, rule_name: str, field_name: str
) -> List[re.Pattern[str]]:
    if raw is None:
        return []
    if not isinstance(raw, list):
        raise CheckError(
            "{0}: {1} must be a list".format(rule_name, field_name)
        )
    result = []
    for value in raw:
        try:
            result.append(re.compile(str(value), re.MULTILINE))
        except re.error as error:
            raise CheckError(
                "{0}: invalid regex {1!r}: {2}".format(
                    rule_name, value, error
                )
            ) from error
    return result


def add_violation(
    out: List[Tuple[str, Path, int, str]],
    rule: str,
    path: Path,
    line: int,
    message: str,
) -> None:
    out.append((rule, path, line, message))


def check_layers(
    root: Path, raw_rules: object
) -> List[Tuple[str, Path, int, str]]:
    if not isinstance(raw_rules, list):
        raise CheckError("layer_rules must be a list")
    out: List[Tuple[str, Path, int, str]] = []

    for raw in raw_rules:
        if not isinstance(raw, dict):
            raise CheckError("Every layer rule must be an object")
        name = str(raw.get("name", "unnamed-layer"))
        paths = raw.get("paths", [])
        prefixes = raw.get("forbidden_include_prefixes", [])
        exact = raw.get("forbidden_includes", [])
        if not isinstance(paths, list):
            raise CheckError("{0}: paths must be a list".format(name))
        if not isinstance(prefixes, list) or not isinstance(exact, list):
            raise CheckError(
                "{0}: forbidden include fields must be lists".format(name)
            )

        for path in iter_cpp(root, [str(value) for value in paths]):
            text = path.read_text(encoding="utf-8")
            for match in INCLUDE_RE.finditer(text):
                include = match.group(1).replace("\\", "/")
                matched = next(
                    (
                        str(value)
                        for value in prefixes
                        if include.startswith(str(value).replace("\\", "/"))
                    ),
                    None,
                )
                if matched is None:
                    matched = next(
                        (
                            str(value)
                            for value in exact
                            if include == str(value)
                            or include.startswith(str(value).rstrip("/") + "/")
                        ),
                        None,
                    )
                if matched is not None:
                    add_violation(
                        out,
                        name,
                        path.relative_to(root),
                        line_at(text, match.start()),
                        "forbidden include '{0}' (boundary '{1}')".format(
                            include, matched
                        ),
                    )
    return out


def check_file_rule(
    root: Path,
    raw: Dict[str, object],
    single_file: bool,
) -> List[Tuple[str, Path, int, str]]:
    name = str(raw.get("name", "unnamed-rule"))
    if single_file:
        file_value = raw.get("file")
        files = [str(file_value)] if file_value else []
    else:
        file_values = raw.get("files", [])
        if not isinstance(file_values, list):
            raise CheckError("{0}: files must be a list".format(name))
        files = [str(value) for value in file_values]

    forbidden = compile_patterns(
        raw.get("forbidden_regex"), name, "forbidden_regex"
    )
    required = compile_patterns(
        raw.get("required_regex"), name, "required_regex"
    )
    out: List[Tuple[str, Path, int, str]] = []

    for relative in files:
        path = root / relative
        if not path.is_file():
            add_violation(
                out, name, Path(relative), 1,
                "required architecture file is missing"
            )
            continue

        text = path.read_text(encoding="utf-8")
        for pattern in forbidden:
            for match in pattern.finditer(text):
                add_violation(
                    out,
                    name,
                    Path(relative),
                    line_at(text, match.start()),
                    "forbidden pattern matched: {0}".format(pattern.pattern),
                )
        for pattern in required:
            if not pattern.search(text):
                add_violation(
                    out,
                    name,
                    Path(relative),
                    1,
                    "required pattern is missing: {0}".format(
                        pattern.pattern
                    ),
                )
    return out


def run_checks(
    root: Path, rules: Dict[str, object]
) -> List[Tuple[str, Path, int, str]]:
    violations = check_layers(root, rules.get("layer_rules", []))

    file_rules = rules.get("file_rules", [])
    text_rules = rules.get("text_rules", [])
    if not isinstance(file_rules, list):
        raise CheckError("file_rules must be a list")
    if not isinstance(text_rules, list):
        raise CheckError("text_rules must be a list")

    for raw in file_rules:
        if not isinstance(raw, dict):
            raise CheckError("Every file rule must be an object")
        violations.extend(check_file_rule(root, raw, False))
    for raw in text_rules:
        if not isinstance(raw, dict):
            raise CheckError("Every text rule must be an object")
        violations.extend(check_file_rule(root, raw, True))

    unique = {}
    for violation in violations:
        key = (
            violation[0],
            violation[1].as_posix(),
            violation[2],
            violation[3],
        )
        unique[key] = violation
    return sorted(
        unique.values(),
        key=lambda item: (
            item[1].as_posix(), item[2], item[0], item[3]
        ),
    )


def emit(
    violation: Tuple[str, Path, int, str], github: bool
) -> None:
    rule, path, line, message = violation
    if github:
        escaped = (
            message.replace("%", "%25")
            .replace("\r", "%0D")
            .replace("\n", "%0A")
        )
        print(
            "::error file={0},line={1},title={2}::{3}".format(
                path.as_posix(), line, rule, escaped
            )
        )
    else:
        print(
            "{0}:{1}: error [{2}]: {3}".format(
                path.as_posix(), line, rule, message
            )
        )


def explain(rules: Dict[str, object]) -> None:
    print("Configured architecture rules:")
    for category in ("layer_rules", "file_rules", "text_rules"):
        print("  {0}:".format(category))
        values = rules.get(category, [])
        if not isinstance(values, list) or not values:
            print("    (none)")
            continue
        for raw in values:
            if isinstance(raw, dict):
                print("    - {0}".format(raw.get("name", "unnamed")))


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Check staged QtMaterial3 architecture boundaries."
    )
    parser.add_argument("--root", type=Path, default=Path.cwd())
    parser.add_argument("--rules", type=Path)
    parser.add_argument("--github", action="store_true")
    parser.add_argument("--explain", action="store_true")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = find_root(args.root)
        rules_path = args.rules
        if rules_path is None:
            rules_path = (
                Path(__file__).resolve().parent
                / "architecture_rules.json"
            )
        elif not rules_path.is_absolute():
            rules_path = root / rules_path

        rules = load_rules(rules_path.resolve())
        if args.explain:
            explain(rules)

        violations = run_checks(root, rules)
        github = args.github or os.environ.get("GITHUB_ACTIONS") == "true"
        if violations:
            for violation in violations:
                emit(violation, github)
            print(
                "Architecture check failed with {0} violation(s).".format(
                    len(violations)
                )
            )
            return 1

        print("Architecture check passed.")
        print("  root: {0}".format(root))
        print("  rules: {0}".format(rules_path.resolve()))
        return 0
    except CheckError as error:
        print("architecture-check error: {0}".format(error), file=sys.stderr)
        return 2
    except OSError as error:
        print(
            "architecture-check I/O error: {0}".format(error),
            file=sys.stderr,
        )
        return 3


if __name__ == "__main__":
    raise SystemExit(main())
