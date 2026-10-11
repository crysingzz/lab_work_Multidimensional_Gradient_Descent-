#!/bin/sh
set -eu

project_dir=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
output_dir="$project_dir/BuildFiles/Mac/arm64/NativeRelease"

if rg -n '\[!output|\[!if' \
    "$project_dir/SourceFiles" "$project_dir/HeaderFiles" \
    "$project_dir/SharedFiles" "$project_dir/UnitTestFiles"; then
    exit 1
fi

if rg -n '\b(malloc|free|realloc|calloc|memcpy|memmove|memset|strlen|printf|fprintf|sprintf|sqrt|fabs)[[:space:]]*\(' \
    "$project_dir/SourceFiles" "$project_dir/HeaderFiles" \
    "$project_dir/SharedFiles" "$project_dir/UnitTestFiles"; then
    exit 1
fi

undefined_symbols=$(nm -u \
    "$output_dir/CEcoVector.o" "$output_dir/CEcoLab1.o" \
    "$output_dir/CEcoLab1Factory.o" "$output_dir/EcoLab1Test.o" \
    "$output_dir/EcoLab1BenchmarkComponent.o" \
    "$output_dir/EcoLab1BenchmarkBaseline.o")
if [ -f "$output_dir/EcoLab1Visualize.o" ]; then
    undefined_symbols="$undefined_symbols
$(nm -u "$output_dir/EcoLab1Visualize.o")"
fi
if [ -f "$output_dir/EcoLab1VisualizeNegative.o" ]; then
    undefined_symbols="$undefined_symbols
$(nm -u "$output_dir/EcoLab1VisualizeNegative.o")"
fi
if printf '%s\n' "$undefined_symbols" | rg '(^|[[:space:]])_+(malloc|free|realloc|calloc|memcpy|memmove|memset|strlen|printf|fprintf|sprintf|sqrt|fabs|stack_chk_fail|stack_chk_guard)$'; then
    exit 1
fi

printf '%s\n' 'LAB76_NO_LIBC_OR_TEMPLATE_MARKERS_OK'
