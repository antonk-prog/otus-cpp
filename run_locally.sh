#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INPUT="${1:-$ROOT/input}"

if [[ ! -e "$INPUT" ]]; then
    echo "Input path does not exist: $INPUT" >&2
    exit 1
fi

if [[ -d "$INPUT" ]]; then
    mapfile -d '' FILES < <(find "$INPUT" -type f -name '*.csv' -print0 | sort -z)
    if [[ ${#FILES[@]} -eq 0 ]]; then
        echo "No CSV files found in: $INPUT" >&2
        exit 1
    fi
else
    FILES=("$INPUT")
fi

cat_input() {
    cat "${FILES[@]}"
}

run_pair() {
    local mapper="$1"
    local reducer="$2"

    "$ROOT/bin/$mapper" < <(cat_input) |
        LC_ALL=C sort |
        "$ROOT/bin/$reducer"
}

make -C "$ROOT" --no-print-directory all >/dev/null

echo "mean"
run_pair mapper_mean reducer_mean

echo "variance"
run_pair mapper_variance reducer_variance
