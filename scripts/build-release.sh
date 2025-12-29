#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"
project_root="$(cd "${script_dir}/.." && pwd)"

cd "$project_root"

cmake --preset release
cmake --build --preset release
