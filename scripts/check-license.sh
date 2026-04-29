#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 faudard
# SPDX-License-Identifier: LGPL-3.0-only

set -euo pipefail

repo_root="$(git rev-parse --show-toplevel 2>/dev/null || pwd)"
cd "$repo_root"

spdx_license_tag='SPDX-License-Identifier'':'
spdx_copyright_tag='SPDX-FileCopyrightText'':'

missing_license=()
missing_copyright=()

is_checked_file() {
  local file="$1"

  case "$file" in
    .git/*|build/*|cmake-build-*/*|_deps/*|third_party/*|external/*|vendor/*)
      return 1
      ;;

    LICENSE|LICENSE.*|COPYING|COPYING.*|LICENSES/*)
      return 1
      ;;

    *.h|*.hpp|*.hh|*.hxx|*.c|*.cc|*.cpp|*.cxx)
      return 0
      ;;

    CMakeLists.txt|*.cmake)
      return 0
      ;;

    *.py|*.sh|*.ps1)
      return 0
      ;;

    *.yml|*.yaml|*.json|*.toml|*.ini|*.cfg)
      return 0
      ;;

    *.md|*.rst|*.txt)
      return 0
      ;;

    *.ui|*.qrc|*.qss|*.svg)
      return 0
      ;;

    Doxyfile)
      return 0
      ;;

    *)
      return 1
      ;;
  esac
}

while IFS= read -r -d '' file; do
  if ! is_checked_file "$file"; then
    continue
  fi

  if ! grep -q "$spdx_license_tag" "$file"; then
    missing_license+=("$file")
  fi

  if ! grep -q "$spdx_copyright_tag" "$file"; then
    missing_copyright+=("$file")
  fi
done < <(git ls-files -z)

failed=0

if ((${#missing_license[@]} > 0)); then
  failed=1
  echo "Files missing SPDX license tag:"
  printf '  %s\n' "${missing_license[@]}"
  echo
fi

if ((${#missing_copyright[@]} > 0)); then
  failed=1
  echo "Files missing SPDX copyright tag:"
  printf '  %s\n' "${missing_copyright[@]}"
  echo
fi

if ((failed != 0)); then
  echo "License metadata check failed."
  exit 1
fi

echo "License metadata check passed."