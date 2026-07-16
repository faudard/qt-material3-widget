#!/usr/bin/env python3
"""Static architecture contract checker for qt-material3-widgets.

The checker intentionally combines:
  * source-level dependency checks;
  * forbidden API/symbol checks;
  * direct CMake target dependency checks;
  * a debt baseline so existing violations can be removed incrementally.

Exit status is non-zero when:
  * a new violation is not present in the baseline;
  * a stale baseline entry remains after its violation was fixed;
  * --strict is used and any violation exists.
"""

from __future__ import print_function

import argparse
import fnmatch
import hashlib
import json
import re
import shlex
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Sequence, Set, Tuple

SOURCE_SUFFIXES = {".h", ".hh", ".hpp", ".hxx", ".c", ".cc", ".cpp", ".cxx", ".ipp", ".inl"}
SKIP_DIR_NAMES = {
    ".git", ".svn", ".hg", ".idea", ".vs", ".vscode", "__pycache__",
    "third_party", "_deps", "generated", "install", "dist"
}
SKIP_DIR_PREFIXES = ("build", "cmake-build-")


@dataclass(frozen=True)
class Violation:
    rule: str
    path: str
    line: int
    kind: str
    evidence: str
    message: str

    @property
    def key(self) -> str:
        normalized = re.sub(r"\\s+", " ", self.evidence.strip())
        digest = hashlib.sha1((self.kind + "|" + normalized).encode("utf-8")).hexdigest()[:12]
        return "{}|{}|{}|{}".format(self.rule, self.path, self.kind, digest)


class Reporter:
    def __init__(self) -> None:
        self._items: Dict[str, Violation] = {}

    def add(self, violation: Violation) -> None:
        # Collapse duplicate matches of the same architectural defect in one file.
        self._items.setdefault(violation.key, violation)

    @property
    def items(self) -> List[Violation]:
        return sorted(self._items.values(), key=lambda v: (v.rule, v.path, v.line, v.kind))


def load_json(path: Path) -> dict:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError:
        raise SystemExit("architecture config not found: {}".format(path))
    except json.JSONDecodeError as exc:
        raise SystemExit("invalid architecture config {}: {}".format(path, exc))


def is_skipped(path: Path, root: Path) -> bool:
    try:
        parts = path.relative_to(root).parts
    except ValueError:
        return True
    for part in parts[:-1]:
        lowered = part.lower()
        if lowered in SKIP_DIR_NAMES or lowered.startswith(SKIP_DIR_PREFIXES):
            return True
    return False


def iter_source_files(root: Path) -> Iterable[Path]:
    for base in (root / "include", root / "src"):
        if not base.exists():
            continue
        for path in base.rglob("*"):
            if not path.is_file() or is_skipped(path, root):
                continue
            if path.suffix.lower() in SOURCE_SUFFIXES:
                yield path


def iter_cmake_files(root: Path) -> Iterable[Path]:
    for path in root.rglob("*"):
        if not path.is_file() or is_skipped(path, root):
            continue
        if path.name == "CMakeLists.txt" or path.suffix.lower() == ".cmake":
            yield path


def relpath(path: Path, root: Path) -> str:
    return path.relative_to(root).as_posix()


def matches_any(path: str, patterns: Sequence[str]) -> bool:
    return any(fnmatch.fnmatchcase(path, pattern) for pattern in patterns)


def strip_cpp_comments(text: str) -> str:
    out: List[str] = []
    i = 0
    state = "code"
    quote = ""
    while i < len(text):
        ch = text[i]
        nxt = text[i + 1] if i + 1 < len(text) else ""
        if state == "code":
            if ch == "/" and nxt == "/":
                out.extend("  ")
                i += 2
                state = "line_comment"
            elif ch == "/" and nxt == "*":
                out.extend("  ")
                i += 2
                state = "block_comment"
            elif ch in ('"', "'"):
                quote = ch
                out.append(ch)
                i += 1
                state = "string"
            else:
                out.append(ch)
                i += 1
        elif state == "line_comment":
            if ch == "\n":
                out.append("\n")
                state = "code"
            else:
                out.append(" ")
            i += 1
        elif state == "block_comment":
            if ch == "*" and nxt == "/":
                out.extend("  ")
                i += 2
                state = "code"
            else:
                out.append("\n" if ch == "\n" else " ")
                i += 1
        else:  # string or character literal
            out.append(ch)
            if ch == "\\" and i + 1 < len(text):
                out.append(text[i + 1])
                i += 2
            elif ch == quote:
                i += 1
                state = "code"
            else:
                i += 1
    return "".join(out)


def strip_cpp_strings(text: str) -> str:
    out: List[str] = []
    i = 0
    state = "code"
    quote = ""
    while i < len(text):
        ch = text[i]
        if state == "code":
            if ch in ('"', "'"):
                quote = ch
                out.append(" ")
                i += 1
                state = "string"
            else:
                out.append(ch)
                i += 1
        else:
            if ch == "\\" and i + 1 < len(text):
                out.extend("  ")
                i += 2
            elif ch == quote:
                out.append(" ")
                i += 1
                state = "code"
            else:
                out.append("\n" if ch == "\n" else " ")
                i += 1
    return "".join(out)


def line_of(text: str, offset: int) -> int:
    return text.count("\n", 0, offset) + 1


def evidence_for(text: str, match: re.Match) -> str:
    start = text.rfind("\n", 0, match.start()) + 1
    end = text.find("\n", match.end())
    if end < 0:
        end = len(text)
    return text[start:end].strip()[:240]


def add_regex_matches(
    reporter: Reporter,
    root: Path,
    path: Path,
    text: str,
    rule: str,
    kind: str,
    pattern: str,
    message: str,
    flags: int = re.MULTILINE,
) -> None:
    for match in re.finditer(pattern, text, flags):
        reporter.add(Violation(
            rule=rule,
            path=relpath(path, root),
            line=line_of(text, match.start()),
            kind=kind,
            evidence=evidence_for(text, match),
            message=message,
        ))


def check_qwidget_free_layer(
    reporter: Reporter,
    root: Path,
    path: Path,
    original: str,
    code: str,
    patterns: Sequence[str],
    rule: str,
    layer_name: str,
) -> None:
    rel = relpath(path, root)
    if not matches_any(rel, patterns):
        return
    add_regex_matches(
        reporter, root, path, original, rule, "qtwidgets-include",
        r"^\s*#\s*include\s*[<\"](?:QtWidgets/)?(?:QWidget|qwidget\.h)[>\"]",
        "{} must not include QWidget".format(layer_name),
    )
    add_regex_matches(
        reporter, root, path, original, rule, "qtwidgets-module-include",
        r"^\s*#\s*include\s*[<\"]QtWidgets/",
        "{} must not include the QtWidgets module".format(layer_name),
    )
    add_regex_matches(
        reporter, root, path, strip_cpp_strings(code), rule, "qwidget-symbol",
        r"\bQWidget\b",
        "{} must not reference QWidget".format(layer_name),
    )


def check_sources(root: Path, config: dict, reporter: Reporter) -> None:
    layers = config.get("layers", {})
    foundation = layers.get("foundation", [])
    theme_model = layers.get("theme_model", [])
    specs = layers.get("specs", [])
    core = layers.get("core", [])
    widgets = layers.get("widgets", [])
    renderers = layers.get("renderers", [])
    theme_io = layers.get("theme_io", [])
    resolution_allow = config.get("theme_resolution_allow", [])
    textual_key_scope = config.get("textual_key_scope", [])

    for path in iter_source_files(root):
        rel = relpath(path, root)
        try:
            original = path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            original = path.read_text(encoding="utf-8", errors="replace")
        code = strip_cpp_comments(original)
        symbols = strip_cpp_strings(code)

        check_qwidget_free_layer(
            reporter, root, path, original, code, foundation,
            "ARCH-FOUNDATION-NO-QWIDGET", "foundation",
        )
        check_qwidget_free_layer(
            reporter, root, path, original, code, theme_model,
            "ARCH-THEME-MODEL-NO-QWIDGET", "theme_model",
        )

        if matches_any(rel, specs):
            add_regex_matches(
                reporter, root, path, original,
                "ARCH-SPECS-NO-CORE-WIDGETS", "forbidden-project-include",
                r"^\s*#\s*include\s*[<\"]qtmaterial/(?:core|widgets)/",
                "specs must depend only on foundation and theme_model, not core/widgets",
            )
            add_regex_matches(
                reporter, root, path, symbols,
                "ARCH-SPECS-NO-THEME-MANAGER", "theme-manager-symbol",
                r"\bThemeManager\b|\bQtMaterialThemeManager\b",
                "specs must resolve from an explicit Theme value and never read ThemeManager",
            )
            add_regex_matches(
                reporter, root, path, original,
                "ARCH-SPECS-NO-THEME-MANAGER", "theme-manager-include",
                r"qtmaterialthememanager\.h",
                "specs must not include ThemeManager",
            )

        if matches_any(rel, renderers):
            add_regex_matches(
                reporter, root, path, symbols,
                "ARCH-RENDERERS-NO-THEME-MANAGER", "theme-manager-symbol",
                r"\bThemeManager\b|\bQtMaterialThemeManager\b",
                "renderers must consume resolved render data and never read ThemeManager",
            )
            add_regex_matches(
                reporter, root, path, original,
                "ARCH-RENDERERS-NO-THEME-MANAGER", "theme-manager-include",
                r"qtmaterialthememanager\.h",
                "renderers must not include ThemeManager",
            )

        if matches_any(rel, widgets) and not matches_any(rel, resolution_allow):
            add_regex_matches(
                reporter, root, path, original,
                "ARCH-WIDGETS-RESOLVED-SPEC-ONLY", "theme-header-outside-resolution",
                r"^\s*#\s*include\s*[<\"]qtmaterial/theme/",
                "widgets may read theme data only inside the dedicated spec-resolution boundary",
            )
            add_regex_matches(
                reporter, root, path, symbols,
                "ARCH-WIDGETS-RESOLVED-SPEC-ONLY", "theme-access-outside-resolution",
                r"\bThemeManager\b|\bThemeManager\s*::\s*instance\b|"
                r"(?:\.|->)\s*(?:theme|colorScheme|typographyScale|shapeScale|elevationScale|"
                r"motionTokens|densityTokens|iconSizeTokens|componentTokens)\s*\(",
                "widget code after resolution must consume only its resolved spec",
            )

        if matches_any(rel, textual_key_scope) and not matches_any(rel, theme_io):
            add_regex_matches(
                reporter, root, path, symbols,
                "ARCH-THEME-IO-OWNS-TEXT-KEYS", "serialization-api-outside-theme-io",
                r"\b(?:QJsonObject|QJsonDocument|QJsonValue|QJsonArray|"
                r"QXmlStreamReader|QXmlStreamWriter|QDomDocument|QSettings)\b",
                "ThemeIO is the only layer allowed to manipulate serialized textual keys",
            )
            add_regex_matches(
                reporter, root, path, symbols,
                "ARCH-THEME-IO-OWNS-TEXT-KEYS", "string-conversion-outside-theme-io",
                r"\b(?:componentId|colorRole|shapeRole|elevationRole|motionToken|themeMode)"
                r"(?:ToString|FromString)\b",
                "typed theme identifiers must be converted to/from text only by ThemeIO",
            )
            add_regex_matches(
                reporter, root, path, code,
                "ARCH-THEME-IO-OWNS-TEXT-KEYS", "component-key-list-outside-theme-io",
                r"apply[A-Za-z0-9_]*ComponentTokens\s*\([^;]*?QStringList\s*\{\s*"
                r"(?:QStringLiteral|QLatin1String)\s*\(",
                "spec resolution must use typed component/token identifiers, not textual lookup keys",
                flags=re.MULTILINE | re.DOTALL,
            )
            add_regex_matches(
                reporter, root, path, symbols,
                "ARCH-THEME-IO-OWNS-TEXT-KEYS", "string-key-overload-outside-theme-io",
                r"\b(?:contains|overrideFor|setOverride|removeOverride)\s*\(\s*const\s+QString\s*&",
                "string-key theme APIs belong in ThemeIO; model/spec APIs must be typed",
            )

        if matches_any(rel, core):
            add_regex_matches(
                reporter, root, path, symbols,
                "ARCH-CORE-NO-DOCS-GALLERY", "documentation-gallery-field",
                r"\b(?:docsPath|galleryRoute|documentationUrl|galleryUrl|showcaseRoute)\b",
                "core must not contain documentation or gallery metadata",
            )
            add_regex_matches(
                reporter, root, path, code,
                "ARCH-CORE-NO-DOCS-GALLERY", "documentation-gallery-literal",
                r"(?:QStringLiteral|QLatin1String)\s*\(\s*[\"](?:docs/|examples/gallery/|gallery/)",
                "documentation/gallery paths belong in tooling, examples, or a catalog module",
            )
            add_regex_matches(
                reporter, root, path, original,
                "ARCH-CORE-NO-DOCS-GALLERY", "documentation-gallery-include",
                r"^\s*#\s*include\s*[<\"](?:docs/|examples/gallery/|gallery/)",
                "core must not include documentation or gallery resources",
            )


def remove_cmake_comments(text: str) -> str:
    lines: List[str] = []
    for line in text.splitlines(True):
        in_quote = False
        escaped = False
        out: List[str] = []
        for ch in line:
            if escaped:
                out.append(ch)
                escaped = False
                continue
            if ch == "\\":
                out.append(ch)
                escaped = True
                continue
            if ch == '"':
                in_quote = not in_quote
                out.append(ch)
                continue
            if ch == "#" and not in_quote:
                out.append("\n" if line.endswith("\n") else "")
                break
            out.append(ch)
        lines.append("".join(out))
    return "".join(lines)


def extract_cmake_calls(text: str, names: Set[str]) -> List[Tuple[str, str, int]]:
    calls: List[Tuple[str, str, int]] = []
    clean = remove_cmake_comments(text)
    name_re = re.compile(r"\b(" + "|".join(re.escape(n) for n in sorted(names, key=len, reverse=True)) + r")\s*\(", re.IGNORECASE)
    pos = 0
    while True:
        match = name_re.search(clean, pos)
        if not match:
            break
        depth = 1
        i = match.end()
        in_quote = False
        escaped = False
        while i < len(clean) and depth:
            ch = clean[i]
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == '"':
                in_quote = not in_quote
            elif not in_quote:
                if ch == "(":
                    depth += 1
                elif ch == ")":
                    depth -= 1
            i += 1
        body = clean[match.end():i - 1] if depth == 0 else clean[match.end():]
        calls.append((match.group(1).lower(), body, line_of(clean, match.start())))
        pos = max(i, match.end())
    return calls


def cmake_tokens(body: str) -> List[str]:
    try:
        return shlex.split(body.replace(";", " "), posix=True)
    except ValueError:
        return re.findall(r"[^\s;]+", body)


def normalize_dependency(token: str) -> str:
    token = token.strip().strip('"')
    token = token.replace("$<LINK_ONLY:", "").rstrip(">")
    return token


def dependency_matches(actual: str, forbidden: str) -> bool:
    a = actual.lower()
    f = forbidden.lower()
    if a == f:
        return True
    # Retain matching through common generator-expression wrappers.
    return f in a and ("$<" in a or "${" in a)


def check_cmake(root: Path, config: dict, reporter: Reporter) -> None:
    definitions: Dict[str, Tuple[str, int]] = {}
    links: Dict[str, List[Tuple[str, str, int]]] = {}
    call_names = {
        "add_library", "qtm3_add_library", "qtm3_add_module",
        "target_link_libraries"
    }

    for path in iter_cmake_files(root):
        rel = relpath(path, root)
        try:
            text = path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            text = path.read_text(encoding="utf-8", errors="replace")
        for name, body, call_line in extract_cmake_calls(text, call_names):
            tokens = cmake_tokens(body)
            if not tokens:
                continue
            target = normalize_dependency(tokens[0])
            if name in {"add_library", "qtm3_add_library", "qtm3_add_module"}:
                if target and "${" not in target and "$<" not in target:
                    definitions.setdefault(target, (rel, call_line))
            elif name == "target_link_libraries":
                for token in tokens[1:]:
                    dep = normalize_dependency(token)
                    if not dep or dep.upper() in {"PUBLIC", "PRIVATE", "INTERFACE", "DEBUG", "OPTIMIZED", "GENERAL"}:
                        continue
                    links.setdefault(target, []).append((dep, rel, call_line))

    for target in config.get("required_targets", []):
        if target not in definitions:
            reporter.add(Violation(
                rule="ARCH-REQUIRED-TARGETS",
                path="CMakeLists.txt",
                line=1,
                kind="missing-target",
                evidence=target,
                message="required architecture target is not defined: {}".format(target),
            ))

    forbidden_links: Dict[str, List[str]] = config.get("target_forbidden_links", {})
    for target, forbidden_deps in forbidden_links.items():
        for dep, source_path, line in links.get(target, []):
            for forbidden in forbidden_deps:
                if dependency_matches(dep, forbidden):
                    reporter.add(Violation(
                        rule="ARCH-CMAKE-TARGET-GRAPH",
                        path=source_path,
                        line=line,
                        kind="{}-forbids-{}".format(target, forbidden),
                        evidence="{} -> {}".format(target, dep),
                        message="target {} must not link {}".format(target, forbidden),
                    ))


def load_baseline(path: Path) -> Set[str]:
    if not path.exists():
        return set()
    values: Set[str] = set()
    for raw in path.read_text(encoding="utf-8").splitlines():
        line = raw.strip()
        if line and not line.startswith("#"):
            values.add(line)
    return values


def write_baseline(path: Path, violations: Sequence[Violation]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    lines = [
        "# Existing architecture debt.",
        "# Remove entries in the same change that fixes the corresponding violation.",
        "# Generated by: python tools/check_architecture_contracts.py --update-baseline",
        "",
    ]
    lines.extend(v.key for v in violations)
    path.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")


def print_violation(prefix: str, violation: Violation) -> None:
    print("{} {}:{}: {} [{}]".format(
        prefix, violation.path, violation.line, violation.message, violation.rule
    ))
    if violation.evidence:
        print("    {}".format(violation.evidence.replace("\n", " ")[:240]))
    print("    baseline-key: {}".format(violation.key))


def main() -> int:
    parser = argparse.ArgumentParser(description="Check qt-material3 architecture contracts")
    parser.add_argument("--root", default=".", help="repository root")
    parser.add_argument("--config", default="tools/architecture_contracts.json")
    parser.add_argument("--baseline", default="tools/architecture_contracts_baseline.txt")
    parser.add_argument("--strict", action="store_true", help="fail on every violation, ignoring baseline")
    parser.add_argument("--update-baseline", action="store_true", help="replace baseline with current violations")
    args = parser.parse_args()

    root = Path(args.root).resolve()
    config_path = (root / args.config).resolve() if not Path(args.config).is_absolute() else Path(args.config)
    baseline_path = (root / args.baseline).resolve() if not Path(args.baseline).is_absolute() else Path(args.baseline)

    config = load_json(config_path)
    reporter = Reporter()
    check_sources(root, config, reporter)
    check_cmake(root, config, reporter)
    violations = reporter.items

    if args.update_baseline:
        write_baseline(baseline_path, violations)
        print("Updated {} with {} violation(s).".format(baseline_path, len(violations)))
        return 0

    current = {v.key: v for v in violations}
    baseline = load_baseline(baseline_path)

    if args.strict:
        for violation in violations:
            print_violation("ERROR", violation)
        if violations:
            print("Architecture contracts failed in strict mode: {} violation(s).".format(len(violations)))
            return 1
        print("Architecture contracts passed in strict mode.")
        return 0

    new_keys = sorted(set(current) - baseline)
    stale_keys = sorted(baseline - set(current))
    debt_keys = sorted(set(current) & baseline)

    for key in new_keys:
        print_violation("ERROR", current[key])
    for key in stale_keys:
        print("ERROR stale architecture baseline entry: {}".format(key))
        print("    Remove it by running: python tools/check_architecture_contracts.py --update-baseline")

    if new_keys or stale_keys:
        print(
            "Architecture contracts failed: {} new violation(s), {} stale baseline entry/entries, "
            "{} accepted debt item(s).".format(len(new_keys), len(stale_keys), len(debt_keys))
        )
        return 1

    print("Architecture contracts passed; {} existing debt item(s) remain baselined.".format(len(debt_keys)))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
