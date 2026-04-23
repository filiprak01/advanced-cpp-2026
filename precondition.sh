#!/usr/bin/env bash
set -euo pipefail

if ! command -v apt-get >/dev/null 2>&1; then
    echo "Ten skrypt jest przygotowany dla Ubuntu/Debian/WSL z apt-get."
    exit 1
fi

echo "==> Instalowanie zaleznosci backendu PWChat dla Linux/WSL..."
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    pkg-config \
    nlohmann-json3-dev \
    libssl-dev \
    libgtest-dev \
    doxygen \
    graphviz

echo
echo "==> Gotowe. Mozesz budowac backend:"
echo "    cmake --preset server-only"
echo "    cmake --build --preset server-only"
