#!/bin/sh
set -eu

project_dir=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
framework_dir=$(CDPATH= cd -- "$project_dir/../Eco.Framework" && pwd)
output_dir="$project_dir/BuildFiles/Mac/arm64/NativeRelease"
include_flags="-I$project_dir/SharedFiles -I$project_dir/HeaderFiles -I$framework_dir/Eco.Core1/SharedFiles -I$framework_dir/Eco.InterfaceBus1/SharedFiles -I$framework_dir/Eco.MemoryManager1/SharedFiles -I$framework_dir/Eco.FileSystemManagement1/SharedFiles -I$framework_dir/Eco.System1/SharedFiles"
# Avoid compiler-inserted libc stack-check calls in the submitted test object.
common_flags="-arch arm64 -std=c89 -O2 -Wall -Wextra -Werror -fno-stack-protector -DECO_APPLE -DECO_AARCH64 -DECO_LIB -DUGUID_UTILITY"
test_name=EcoLab1Test
if [ "${LAB76_NEGATIVE_CONTROL:-0}" = 1 ]; then
    common_flags="$common_flags -DLAB76_INJECT_FAILURE"
    test_name=EcoLab1TestNegative
fi

mkdir -p "$output_dir"
clang $common_flags $include_flags -c "$project_dir/SourceFiles/CEcoVector.c" -o "$output_dir/CEcoVector.o"
clang $common_flags $include_flags -c "$project_dir/SourceFiles/CEcoLab1.c" -o "$output_dir/CEcoLab1.o"
clang $common_flags $include_flags -c "$project_dir/SourceFiles/CEcoLab1Factory.c" -o "$output_dir/CEcoLab1Factory.o"
ar rcs "$output_dir/lib72A5C4F19CE74A6B8F1EEE6C9907DB59.a" \
    "$output_dir/CEcoVector.o" "$output_dir/CEcoLab1.o" "$output_dir/CEcoLab1Factory.o"
clang $common_flags $include_flags -c "$project_dir/UnitTestFiles/SourceFiles/EcoLab1.c" \
    -o "$output_dir/$test_name.o"
clang -arch arm64 -o "$output_dir/$test_name" "$output_dir/$test_name.o" \
    "$output_dir/lib72A5C4F19CE74A6B8F1EEE6C9907DB59.a" \
    "$framework_dir/Eco.System1/BuildFiles/Mac/arm64/StaticRelease/lib00000000000000000000000053595333.a" \
    "$framework_dir/Eco.InterfaceBus1/BuildFiles/Mac/arm64/StaticRelease/lib00000000000000000000000042757331.a" \
    "$framework_dir/Eco.MemoryManager1/BuildFiles/Mac/arm64/StaticRelease/lib0000000000000000000000004D656D31.a" \
    "$framework_dir/Eco.FileSystemManagement1/BuildFiles/Mac/arm64/StaticRelease/lib00000000000000000000000046534D31.a"
"$output_dir/$test_name"

if [ "${LAB76_NEGATIVE_CONTROL:-0}" != 1 ]; then
    clang $common_flags $include_flags -c \
        "$project_dir/UnitTestFiles/SourceFiles/EcoLab1Benchmark.c" \
        -o "$output_dir/EcoLab1BenchmarkComponent.o"
    clang $common_flags $include_flags -DLAB76_BASELINE -c \
        "$project_dir/UnitTestFiles/SourceFiles/EcoLab1Benchmark.c" \
        -o "$output_dir/EcoLab1BenchmarkBaseline.o"
    for mode in Component Baseline; do
        benchmark_name="EcoLab1Benchmark$mode"
        clang -arch arm64 -o "$output_dir/$benchmark_name" \
            "$output_dir/$benchmark_name.o" \
            "$output_dir/lib72A5C4F19CE74A6B8F1EEE6C9907DB59.a" \
            "$framework_dir/Eco.System1/BuildFiles/Mac/arm64/StaticRelease/lib00000000000000000000000053595333.a" \
            "$framework_dir/Eco.InterfaceBus1/BuildFiles/Mac/arm64/StaticRelease/lib00000000000000000000000042757331.a" \
            "$framework_dir/Eco.MemoryManager1/BuildFiles/Mac/arm64/StaticRelease/lib0000000000000000000000004D656D31.a" \
            "$framework_dir/Eco.FileSystemManagement1/BuildFiles/Mac/arm64/StaticRelease/lib00000000000000000000000046534D31.a"
    done
    printf '%s\n' 'LAB76_NATIVE_BUILD_AND_TEST_OK'
fi
