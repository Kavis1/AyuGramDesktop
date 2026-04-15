#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

apply_patch() {
    local submodule="$1"
    local patch="$2"
    local submodule_path="$REPO_DIR/$submodule"

    if [ ! -d "$submodule_path/.git" ] && [ ! -f "$submodule_path/.git" ]; then
        echo "Skipping $submodule (not initialized)"
        return
    fi

    echo "Applying patch to $submodule..."
    cd "$submodule_path"
    if git apply --check "$SCRIPT_DIR/$patch" 2>/dev/null; then
        git apply --whitespace=nowarn "$SCRIPT_DIR/$patch"
        echo "  Done."
    else
        echo "  Patch already applied or conflicts detected, skipping."
    fi
}

apply_patch "lib_ui" "lib_ui_ayugram.patch"
