#!/bin/sh
set -eu

project_dir=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
framework_dir=$(CDPATH= cd -- "$project_dir/../Eco.Framework" && pwd)
output_dir="$project_dir/BuildFiles/Mac/arm64/NativeRelease"
include_flags="-I$project_dir/SharedFiles -I$project_dir/HeaderFiles -I$framework_dir/Eco.Core1/SharedFiles -I$framework_dir/Eco.InterfaceBus1/SharedFiles -I$framework_dir/Eco.MemoryManager1/SharedFiles -I$framework_dir/Eco.FileSystemManagement1/SharedFiles -I$framework_dir/Eco.System1/SharedFiles"
common_flags="-arch arm64 -std=c89 -O2 -Wall -Wextra -Werror -fno-stack-protector -DECO_APPLE -DECO_AARCH64 -DECO_LIB -DUGUID_UTILITY"
demo_name=EcoLab1Visualize
if [ "${LAB76_VISUALIZATION_NEGATIVE_CONTROL:-0}" = 1 ]; then
    common_flags="$common_flags -DLAB76_VISUALIZATION_INJECT_FAILURE"
    demo_name=EcoLab1VisualizeNegative
fi

sh "$project_dir/AssemblyFiles/Mac/build-native.sh"
clang $common_flags $include_flags -c \
    "$project_dir/UnitTestFiles/SourceFiles/EcoLab1Visualize.c" \
    -o "$output_dir/$demo_name.o"
clang -arch arm64 -o "$output_dir/$demo_name" "$output_dir/$demo_name.o" \
    "$output_dir/lib72A5C4F19CE74A6B8F1EEE6C9907DB59.a" \
    "$framework_dir/Eco.System1/BuildFiles/Mac/arm64/StaticRelease/lib00000000000000000000000053595333.a" \
    "$framework_dir/Eco.InterfaceBus1/BuildFiles/Mac/arm64/StaticRelease/lib00000000000000000000000042757331.a" \
    "$framework_dir/Eco.MemoryManager1/BuildFiles/Mac/arm64/StaticRelease/lib0000000000000000000000004D656D31.a" \
    "$framework_dir/Eco.FileSystemManagement1/BuildFiles/Mac/arm64/StaticRelease/lib00000000000000000000000046534D31.a"

demo_dir=$(mktemp -d "$output_dir/visualization.XXXXXX")
trap 'rm -f "$demo_dir/Lab76-visualization.txt"; rmdir "$demo_dir"' EXIT HUP INT TERM
(
    cd "$demo_dir"
    "$output_dir/$demo_name"
)

if [ "${LAB76_VISUALIZATION_NEGATIVE_CONTROL:-0}" != 1 ]; then
    cat "$demo_dir/Lab76-visualization.txt"
fi
