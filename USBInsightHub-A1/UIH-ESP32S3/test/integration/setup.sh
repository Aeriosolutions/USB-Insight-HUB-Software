#!/usr/bin/env bash
# Set up the development environment for integration tests.
set -euo pipefail

cd "$(dirname "$0")"
[ -d .venv ] || python3 -m venv .venv
.venv/bin/pip install -q -r requirements.txt
