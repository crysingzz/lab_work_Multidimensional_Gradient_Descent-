#!/usr/bin/env python3
"""Measure the two already-built console workloads without editing their code."""

import json
import pathlib
import platform
import re
import statistics
import subprocess
import time


project = pathlib.Path(__file__).resolve().parents[2]
source = project / "UnitTestFiles/SourceFiles/EcoLab1Benchmark.c"
output = project / "BuildFiles/Mac/arm64/NativeRelease"
source_text = source.read_text(encoding="utf-8-sig")


def constant(name):
    match = re.search(r"^#define " + name + r" (\d+)$", source_text, re.MULTILINE)
    if match is None:
        raise RuntimeError(f"missing benchmark constant {name}")
    return int(match.group(1))


dimension = constant("BENCH_DIMENSION")
iterations = constant("BENCH_ITERATIONS")
runs = constant("BENCH_RUNS")
calls_per_run = 2 * dimension * iterations + 1
paths = {
    "component": output / "EcoLab1BenchmarkComponent",
    "baseline": output / "EcoLab1BenchmarkBaseline",
}


def execute(name):
    start = time.perf_counter_ns()
    completed = subprocess.run([str(paths[name])], check=False, capture_output=True)
    elapsed = time.perf_counter_ns() - start
    if completed.returncode != 0:
        raise RuntimeError(f"{name} exited {completed.returncode}: {completed.stderr!r}")
    return elapsed / 1_000_000.0


for name in paths:
    execute(name)

samples = {name: [] for name in paths}
for index in range(5):
    order = ("component", "baseline") if index % 2 == 0 else ("baseline", "component")
    for name in order:
        samples[name].append(execute(name))

print(json.dumps({
    "platform": platform.platform(),
    "processor": platform.machine(),
    "compiler": subprocess.check_output(["clang", "--version"], text=True).splitlines()[0],
    "configuration": "arm64, C89, -O2, ECO_LIB, -fno-stack-protector",
    "dimension": dimension,
    "iterations_per_run": iterations,
    "runs": runs,
    "callback_calls_per_run": calls_per_run,
    "callback_calls_per_executable": calls_per_run * runs,
    "samples_ms": samples,
    "median_ms": {name: statistics.median(values) for name, values in samples.items()},
}, ensure_ascii=False, indent=2))
