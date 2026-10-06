#!/bin/bash
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$DIR/gui/.venv/bin/activate"
python "$DIR/gui/src/main.py"
