#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"
project_root="$(cd "${script_dir}/.." && pwd)"

cd "$project_root"

echo "Removing build-debug and build-release directories..."
rm -rf build-debug build-release

echo "Clean complete."
