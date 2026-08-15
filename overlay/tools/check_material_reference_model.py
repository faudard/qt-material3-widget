#!/usr/bin/env python3
"""Validate the versioned Material reference model and product-registry linkage."""

from __future__ import annotations

import argparse
import itertools
import json
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
REF_ROOT = Path("references/material3")
CATALOG = REF_ROOT / "reference-catalog.json"
REGISTRY = Path("docs/components/component-registry.json")

CLASSIFICATIONS = {
    "MUST_MATCH",
    "PLATFORM_ADAPTATION",
    "QTMATERIAL_EXTENSION",
}
DOMAINS = {
    "geometry", "tokens", "colors", "typography",
    "states", "motion", "accessibility", "behavior",
}


def load_json(path: Path):
    return json.loads(path.read_text(encoding="utf-8"))


def index_registry(registry: list[dict]) -> dict[str, dict]:
    return {
        item["id"]: item
        for item in registry
        if isinstance(item, dict) and isinstance(item.get("id"), str)
    }


def validate_profile(profile: dict, expected_id: str | None = None) -> list[str]:
    errors: list[str] = []
    if profile.get("schemaVersion") != 1:
        errors.append("profile schemaVersion must be 1")
    if expected_id and profile.get("id") != expected_id:
        errors.append(f"profile id mismatch: expected {expected_id}")

    material_profile = profile.get("materialProfile")
    if material_profile not in {"classic", "expressive"}:
        errors.append("profile materialProfile must be classic or expressive")

    stability = profile.get("stability")
    if stability not in {"stable", "experimental"}:
        errors.append("profile stability must be stable or experimental")
    classifications = (
        profile.get("comparisonPolicy", {}).get("classifications", [])
    )
    if classifications != [
        "MUST_MATCH", "PLATFORM_ADAPTATION", "QTMATERIAL_EXTENSION"
    ]:
        errors.append("profile classification order/contract drift")

    domains = set(profile.get("comparisonPolicy", {}).get("domains", []))
    if domains != DOMAINS:
        errors.append("profile comparison domains are incomplete")

    sources = profile.get("sources", [])
    source_ids = [source.get("id") for source in sources if isinstance(source, dict)]
    if len(source_ids) != len(set(source_ids)):
        errors.append("profile source IDs must be unique")
    if not any(source.get("kind") == "material-guideline" for source in sources):
        errors.append("profile requires at least one Material guideline source")
    if not any(source.get("kind") == "androidx-implementation" for source in sources):
        errors.append("profile requires an AndroidX executable reference source")

    for source in sources:
        uri = source.get("uri", "")
        if not isinstance(uri, str) or not uri.startswith("https://"):
            errors.append(f"profile source must use pinned HTTPS URI: {source.get('id')}")
        if source.get("kind") == "androidx-implementation":
            if not source.get("artifact") or not source.get("version"):
                errors.append(
                    f"AndroidX source lacks artifact/version: {source.get('id')}"
                )

    return errors


def expected_case_keys(suite: dict) -> set[tuple[str, str, str]]:
    dimensions = suite["caseDimensions"]
    return set(
        itertools.product(
            dimensions["components"],
            dimensions["states"],
            dimensions["themes"],
        )
    )


def validate_suite(
    suite: dict,
    profiles: dict[str, dict],
    registry_index: dict[str, dict],
) -> list[str]:
    errors: list[str] = []

    if suite.get("schemaVersion") != 1:
        errors.append("suite schemaVersion must be 1")

    profile_id = suite.get("profileId")
    profile = profiles.get(profile_id)
    if profile is None:
        return [f"suite references unknown profile: {profile_id}"]

    components = suite.get("caseDimensions", {}).get("components", [])
    states = suite.get("caseDimensions", {}).get("states", [])
    themes = suite.get("caseDimensions", {}).get("themes", [])
    if not components or not states or not themes:
        errors.append("suite dimensions must be non-empty")

    for component in components:
        if component not in registry_index:
            errors.append(f"suite component missing from product registry: {component}")

    anchor = suite.get("anchorComponentId")
    anchor_entry = registry_index.get(anchor)
    if anchor_entry is None:
        errors.append(f"suite anchor missing from registry: {anchor}")
    else:
        if not anchor_entry.get("referenceCandidate", False):
            errors.append(f"suite anchor is not referenceCandidate: {anchor}")
        if anchor_entry.get("family") != suite.get("family"):
            errors.append(
                f"suite family {suite.get('family')} disagrees with anchor family "
                f"{anchor_entry.get('family')}"
            )

    source_ids = {
        source.get("id")
        for source in profile.get("sources", [])
        if isinstance(source, dict)
    }
    for source_ref in suite.get("sourceRefs", []):
        if source_ref not in source_ids:
            errors.append(f"suite sourceRef missing from profile: {source_ref}")

    cases = suite.get("cases", [])
    ids = [case.get("id") for case in cases if isinstance(case, dict)]
    if len(ids) != len(set(ids)):
        errors.append("suite case IDs must be unique")

    actual_keys: set[tuple[str, str, str]] = set()
    for case in cases:
        key = (case.get("componentId"), case.get("state"), case.get("theme"))
        actual_keys.add(key)

        if case.get("classification") not in CLASSIFICATIONS:
            errors.append(f"case classification invalid: {case.get('id')}")
        if case.get("componentId") not in components:
            errors.append(f"case outside component dimension: {case.get('id')}")
        if case.get("state") not in states:
            errors.append(f"case outside state dimension: {case.get('id')}")
        if case.get("theme") not in themes:
            errors.append(f"case outside theme dimension: {case.get('id')}")

        assertions = case.get("assertions")
        if not isinstance(assertions, list):
            errors.append(f"case assertions must be a list: {case.get('id')}")
            continue

        if suite.get("status") == "evidence-backed" and not assertions:
            errors.append(
                f"evidence-backed case cannot have zero assertions: {case.get('id')}"
            )

        assertion_ids: set[str] = set()
        for assertion in assertions:
            aid = assertion.get("id")
            if aid in assertion_ids:
                errors.append(f"duplicate assertion id in {case.get('id')}: {aid}")
            assertion_ids.add(aid)

            if assertion.get("domain") not in DOMAINS:
                errors.append(f"invalid assertion domain in {case.get('id')}: {aid}")
            if assertion.get("classification") not in CLASSIFICATIONS:
                errors.append(
                    f"invalid assertion classification in {case.get('id')}: {aid}"
                )
            refs = assertion.get("sourceRefs", [])
            if not refs:
                errors.append(f"assertion lacks sourceRefs in {case.get('id')}: {aid}")
            for ref in refs:
                if ref not in source_ids:
                    errors.append(
                        f"assertion sourceRef missing from profile in "
                        f"{case.get('id')}: {ref}"
                    )

    expected_keys = expected_case_keys(suite)
    if actual_keys != expected_keys:
        missing = sorted(expected_keys - actual_keys)
        extra = sorted(actual_keys - expected_keys)
        if missing:
            errors.append(f"suite Cartesian matrix missing {len(missing)} case(s)")
        if extra:
            errors.append(f"suite Cartesian matrix has {len(extra)} extra case(s)")

    if len(cases) != len(expected_keys):
        errors.append(
            f"suite case count {len(cases)} != dimension product {len(expected_keys)}"
        )

    return errors


def validate_tree(root: Path) -> list[str]:
    errors: list[str] = []
    ref_root = root / REF_ROOT
    catalog_path = root / CATALOG
    registry_path = root / REGISTRY

    if not catalog_path.is_file():
        return [f"missing reference catalog: {CATALOG}"]
    if not registry_path.is_file():
        return [f"missing component registry: {REGISTRY}"]

    catalog = load_json(catalog_path)
    registry = load_json(registry_path)
    registry_index = index_registry(registry)

    profiles: dict[str, dict] = {}
    profile_ids: set[str] = set()
    for item in catalog.get("profiles", []):
        pid = item.get("id")
        if pid in profile_ids:
            errors.append(f"duplicate catalog profile id: {pid}")
            continue
        profile_ids.add(pid)

        path = ref_root / item.get("path", "")
        if not path.is_file():
            errors.append(f"catalog profile path missing: {item.get('path')}")
            continue
        profile = load_json(path)
        profiles[pid] = profile
        errors.extend(
            f"{item.get('path')}: {error}"
            for error in validate_profile(profile, pid)
        )

        if profile.get("materialProfile") != item.get("profile"):
            errors.append(f"catalog/profile kind mismatch for {pid}")
        if profile.get("stability") != item.get("stability"):
            errors.append(f"catalog/profile stability mismatch for {pid}")

    # Classic and Expressive are intentionally independent versioned profiles.
    classic = [
        p for p in profiles.values() if p.get("materialProfile") == "classic"
    ]
    expressive = [
        p for p in profiles.values() if p.get("materialProfile") == "expressive"
    ]
    if not classic:
        errors.append("reference catalog requires a Classic profile")
    if not expressive:
        errors.append("reference catalog requires a separate Expressive profile")
    if classic and expressive:
        if classic[0].get("referenceVersion") == expressive[0].get("referenceVersion"):
            errors.append("Classic and Expressive reference versions must be independent")

    suite_ids: set[str] = set()
    for item in catalog.get("suites", []):
        sid = item.get("id")
        if sid in suite_ids:
            errors.append(f"duplicate catalog suite id: {sid}")
            continue
        suite_ids.add(sid)

        path = ref_root / item.get("path", "")
        if not path.is_file():
            errors.append(f"catalog suite path missing: {item.get('path')}")
            continue
        suite = load_json(path)

        if suite.get("id") != sid:
            errors.append(f"catalog/suite id mismatch: {sid}")
        if suite.get("profileId") != item.get("profileId"):
            errors.append(f"catalog/suite profile mismatch: {sid}")
        if suite.get("status") != item.get("status"):
            errors.append(f"catalog/suite status mismatch: {sid}")

        errors.extend(
            f"{item.get('path')}: {error}"
            for error in validate_suite(suite, profiles, registry_index)
        )

    return errors


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=ROOT)
    args = parser.parse_args(argv)

    errors = validate_tree(args.root.resolve())
    if errors:
        print("Material reference model FAILED:")
        for error in errors:
            print(" -", error)
        return 1

    print(
        "Material reference model OK: "
        "Classic/Expressive versioned independently; "
        "button POC = 50 seeded cases"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
