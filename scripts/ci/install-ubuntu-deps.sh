#!/usr/bin/env bash
set -euo pipefail

sudo apt-get update

sudo apt-get install -y \
  build-essential \
  cmake \
  ninja-build \
  pkg-config \
  xvfb \
  libgl1 \
  libegl1 \
  libxkbcommon-x11-0 \
  libxcb-cursor0 \
  libxcb-icccm4 \
  libxcb-image0 \
  libxcb-keysyms1 \
  libxcb-render-util0 \
  libxcb-shape0 \
  libxcb-xfixes0 \
  libxcb-xinerama0 \
  openbox \
  qt6-base-dev \
  qt6-base-dev-tools