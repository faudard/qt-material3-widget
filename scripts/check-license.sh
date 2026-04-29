#!/usr/bin/env bash
set -euo pipefail

bad=0

echo "Checking forbidden non-strict LGPL identifiers..."

if grep -RIn \
  --exclude-dir=.git \
  --exclude-dir=build \
  --exclude-dir=cmake-build-debug \
  --exclude-dir=cmake-build-release \
  --exclude-dir=_deps \
  --exclude=LICENSE \
  --exclude='LGPL-3.0-only.txt' \
  'LGPL-3\.0-or-later\|LGPLv3+\|LGPL-3\.0+' .; then
  echo "Found non-strict LGPL references. Use LGPL-3.0-only instead." >&2
  bad=1
fi

echo "Checking SPDX headers..."

missing_spdx="$(find . \
  -path './.git' -prune -o \
  -path './build' -prune -o \
  -path './cmake-build-*' -prune -o \
  -path './_deps' -prune -o \
  -path './third_party' -prune -o \
  -path './external' -prune -o \
  -path './vendor' -prune -o \
  -type f \( \
    -name '*.h' -o -name '*.hpp' -o -name '*.hh' -o \
    -name '*.c' -o -name '*.cc' -o -name '*.cpp' -o -name '*.cxx' -o \
    -name '*.cmake' -o -name 'CMakeLists.txt' -o \
    -name '*.py' -o -name '*.sh' -o \
    -name '*.yml' -o -name '*.yaml' -o \
    -name '*.md' -o -name '*.ui' -o -name '*.qrc' \
  \) -print | while read -r f; do
    if ! grep -q 'SPDX-License-Identifier:' "$f"; then
      echo "$f"
    fi
  done)"

if [ -n "$missing_spdx" ]; then
  echo "Files missing SPDX headers:"
  echo "$missing_spdx"
  bad=1
fi

if command -v reuse >/dev/null 2>&1; then
  echo "Running reuse lint..."
  reuse lint || bad=1
else
  echo "reuse CLI not installed; skipping reuse lint."
  echo "Install with: python -m pip install reuse"
fi

exit "$bad"
