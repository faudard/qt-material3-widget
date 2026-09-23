from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = Path("docs/compatibility/qt-support.json")


class QtSupportError(RuntimeError):
    pass


def load_manifest(root: Path = ROOT) -> dict[str, Any]:
    path = root / MANIFEST
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise QtSupportError(f"missing Qt support manifest: {path}") from exc
    except json.JSONDecodeError as exc:
        raise QtSupportError(f"invalid Qt support manifest: {exc}") from exc
    if not isinstance(data, dict):
        raise QtSupportError("Qt support manifest must contain a JSON object")
    return data


def _version_tuple(value: str) -> tuple[int, ...]:
    try:
        return tuple(int(part) for part in value.split("."))
    except ValueError as exc:
        raise QtSupportError(f"invalid version: {value}") from exc


def certification_profiles(data: dict[str, Any]) -> dict[str, dict[str, Any]]:
    qt = data.get("qt", {})
    result: dict[str, dict[str, Any]] = {}

    qt5 = qt.get("5", {})
    primary = qt5.get("primaryCertification")
    if isinstance(primary, dict):
        profile = primary.get("profile")
        if isinstance(profile, str) and profile:
            result[profile] = primary

    qt6 = qt.get("6", {})
    certifications = qt6.get("certification", [])
    if isinstance(certifications, list):
        for item in certifications:
            if not isinstance(item, dict):
                continue
            profile = item.get("profile")
            if isinstance(profile, str) and profile:
                result[profile] = item

    return result


def certification_profile(
    data: dict[str, Any],
    profile: str,
) -> dict[str, Any]:
    profiles = certification_profiles(data)
    if profile not in profiles:
        raise QtSupportError(f"unknown Qt certification profile: {profile}")
    return profiles[profile]


def validate_manifest(data: dict[str, Any]) -> list[str]:
    errors: list[str] = []
    if data.get("schemaVersion") != 1:
        errors.append("qt-support schemaVersion must be 1")
    if data.get("languageStandard") != "C++17":
        errors.append("languageStandard must be C++17")

    qt = data.get("qt")
    if not isinstance(qt, dict):
        return errors + ["qt must be an object"]

    qt5 = qt.get("5")
    qt6 = qt.get("6")
    if not isinstance(qt5, dict):
        errors.append("qt.5 must be an object")
        qt5 = {}
    if not isinstance(qt6, dict):
        errors.append("qt.6 must be an object")
        qt6 = {}

    qt5_min = qt5.get("minimum")
    advertised = qt5.get("advertisedFloor")
    if not isinstance(qt5_min, str) or not qt5_min:
        errors.append("Qt5 minimum must be declared")
    if advertised != qt5_min:
        errors.append("Qt5 advertisedFloor must equal the Qt5 minimum")

    primary = qt5.get("primaryCertification")
    if not isinstance(primary, dict):
        errors.append("Qt5 primaryCertification must be an object")
        primary = {}
    for field in (
        "profile",
        "os",
        "qtVersion",
        "qtArchive",
        "hostedRunner",
        "hostedMsvcToolset",
        "hostedCompilerPrefix",
    ):
        if not primary.get(field):
            errors.append(f"Qt5 primaryCertification missing {field}")

    if isinstance(qt5_min, str) and isinstance(primary.get("qtVersion"), str):
        try:
            if _version_tuple(primary["qtVersion"]) < _version_tuple(qt5_min):
                errors.append(
                    "Qt5 primary certification must not be below the minimum"
                )
        except QtSupportError as exc:
            errors.append(str(exc))

    toolsets = primary.get("referenceMsvcToolsets", [])
    if not isinstance(toolsets, list) or not toolsets:
        errors.append("Qt5 referenceMsvcToolsets must be non-empty")
    elif primary.get("hostedMsvcToolset") not in toolsets:
        errors.append(
            "Qt5 hostedMsvcToolset must be included in referenceMsvcToolsets"
        )

    prefixes = primary.get("referenceCompilerPrefixes", [])
    if not isinstance(prefixes, list) or not prefixes:
        errors.append("Qt5 referenceCompilerPrefixes must be non-empty")
    elif primary.get("hostedCompilerPrefix") not in prefixes:
        errors.append(
            "Qt5 hostedCompilerPrefix must be included in "
            "referenceCompilerPrefixes"
        )

    qt6_min = qt6.get("minimum")
    if not isinstance(qt6_min, str) or not qt6_min:
        errors.append("Qt6 minimum must be declared")

    certifications = qt6.get("certification")
    if not isinstance(certifications, list) or not certifications:
        errors.append("Qt6 certification must be a non-empty array")
        certifications = []

    profiles = certification_profiles(data)
    expected_profile_count = 1 + sum(
        1 for item in certifications
        if isinstance(item, dict) and item.get("profile")
    )
    if len(profiles) != expected_profile_count:
        errors.append("Qt certification profile names must be unique")

    for item in certifications:
        if not isinstance(item, dict):
            errors.append("Qt6 certification entries must be objects")
            continue
        if not item.get("profile"):
            errors.append("Qt6 certification entry missing profile")
        if not item.get("os"):
            errors.append("Qt6 certification entry missing os")
        version = item.get("qtVersion")
        if version is not None and isinstance(qt6_min, str):
            if not isinstance(version, str):
                errors.append("Qt6 certification qtVersion must be a string")
            else:
                try:
                    if _version_tuple(version) < _version_tuple(qt6_min):
                        errors.append(
                            f"Qt6 certification {item.get('profile', '<unknown>')} "
                            "is below the declared minimum"
                        )
                except QtSupportError as exc:
                    errors.append(str(exc))

    return errors
