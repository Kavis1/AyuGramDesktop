#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

apply_patch() {
    local submodule="$1"
    local patch="$2"
    local submodule_path="$REPO_DIR/$submodule"

    if [ ! -d "$submodule_path/.git" ] && [ ! -f "$submodule_path/.git" ]; then
        echo "ERROR: $submodule submodule is not initialized at $submodule_path" >&2
        exit 1
    fi

    echo "Applying patch $patch to $submodule..."
    cd "$submodule_path"

    if git apply --reverse --check "$SCRIPT_DIR/$patch" 2>/dev/null; then
        echo "  Already applied, skipping."
        return
    fi

    if ! git apply --check "$SCRIPT_DIR/$patch" 2>/dev/null; then
        echo "ERROR: patch $patch does not apply cleanly to $submodule" >&2
        echo "       and is not already applied. Aborting." >&2
        git apply --check "$SCRIPT_DIR/$patch" >&2 || true
        exit 1
    fi

    git apply --whitespace=nowarn "$SCRIPT_DIR/$patch"
    echo "  Done."
}

apply_patch "lib_ui" "lib_ui_ayugram.patch"
