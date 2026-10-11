#!/usr/bin/env python3
"""Compare real ACOM trace output with NumPy and SciPy references.

NumPy follows the same fixed learning-rate schedule using exact gradients of
the test losses. SciPy independently locates each minimum with its numerical
three-point gradient. Neither package is linked into the C component.
"""

from __future__ import annotations

import re
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path

try:
    import numpy as np
    import scipy
    from scipy.differentiate import jacobian
    from scipy.optimize import minimize
except ImportError as error:
    raise SystemExit(
        "Install the comparison dependencies first: "
        "python3 -m pip install -r "
        "Eco.Lab1/AssemblyFiles/Mac/requirements-reference.txt"
    ) from error


ROOT = Path(__file__).resolve().parents[3]
DEMO = ROOT / "Eco.Lab1/AssemblyFiles/Mac/run-visualization.sh"
DISPLAY_TOLERANCE = 5.1e-7  # C display rounds each value to six decimal places.
STEPS = 6


@dataclass
class Row:
    step: int
    loss: float
    weights: list[float] = field(default_factory=list)


@dataclass(frozen=True)
class Scenario:
    name: str
    dimension: int
    learning_rate: float


SCENARIOS = (
    Scenario("quadratic-1", 1, 0.1),
    Scenario("mse-2", 2, 0.03),
    Scenario("quadratic-5", 5, 0.08),
    Scenario("quadratic-10", 10, 0.05),
)

STEP_LINE = re.compile(r"^\s*(\d+)\s*\|\s*([0-9]+\.[0-9]{6})\s*\|")
WEIGHT_LINE = re.compile(r"^\s*\|\s*w\[(\d+)(?:\.\.(\d+))?\]\s*=\s*(.*)$")


def parse_trace(output: str) -> dict[str, list[Row]]:
    traces: dict[str, list[Row]] = {}
    current: str | None = None
    for line in output.splitlines():
        if line.startswith("Quadratic L(w)=w^2"):
            current = "quadratic-1"
            traces[current] = []
            continue
        if line.startswith("MSE y=w0+w1*x"):
            current = "mse-2"
            traces[current] = []
            continue
        if line.startswith("Separable quadratic"):
            match = re.search(r"\bn=(5|10)\b", line)
            if match is None:
                raise ValueError(f"Unexpected quadratic heading: {line}")
            current = f"quadratic-{match.group(1)}"
            traces[current] = []
            continue
        match = STEP_LINE.match(line)
        if match is not None and current is not None:
            traces[current].append(Row(int(match.group(1)), float(match.group(2))))
            continue
        match = WEIGHT_LINE.match(line)
        if match is not None and current is not None:
            if not traces[current]:
                raise ValueError(f"Weight line without a step: {line}")
            first = int(match.group(1))
            last = int(match.group(2) or match.group(1))
            values = [float(value) for value in match.group(3).split()]
            if first != len(traces[current][-1].weights) or len(values) != last - first + 1:
                raise ValueError(f"Incomplete or reordered C weights: {line}")
            traces[current][-1].weights.extend(values)

    if set(traces) != {scenario.name for scenario in SCENARIOS}:
        raise ValueError(f"C output has unexpected scenarios: {sorted(traces)}")
    for scenario in SCENARIOS:
        rows = traces[scenario.name]
        if len(rows) != STEPS + 1:
            raise ValueError(f"{scenario.name}: expected {STEPS + 1} trace rows")
        for step, row in enumerate(rows):
            if row.step != step or len(row.weights) != scenario.dimension:
                raise ValueError(f"{scenario.name}: malformed C row {step}")
            if not np.isfinite(row.loss) or not np.all(np.isfinite(row.weights)):
                raise ValueError(f"{scenario.name}: nonfinite C values")
    return traces


def initial_weights(scenario: Scenario) -> np.ndarray:
    if scenario.dimension == 1:
        return np.array([2.0], dtype=np.float64)
    if scenario.dimension == 2:
        return np.zeros(2, dtype=np.float64)
    return 2.0 * np.arange(scenario.dimension, dtype=np.float64) + 1.0


def target_weights(scenario: Scenario) -> np.ndarray:
    if scenario.dimension == 1:
        return np.zeros(1, dtype=np.float64)
    if scenario.dimension == 2:
        return np.array([1.0, 2.0], dtype=np.float64)
    return np.arange(scenario.dimension, dtype=np.float64)


def loss_and_exact_gradient(weights: np.ndarray, scenario: Scenario) -> tuple[float, np.ndarray]:
    if scenario.dimension == 2:
        x = np.arange(4, dtype=np.float64)
        design = np.column_stack((np.ones(4, dtype=np.float64), x))
        labels = 1.0 + 2.0 * x
        residual = design @ weights - labels
        return float(np.mean(residual * residual)), 2.0 * (design.T @ residual) / 4.0
    residual = weights - target_weights(scenario)
    return float(residual @ residual), 2.0 * residual


def numpy_path(scenario: Scenario) -> list[tuple[np.ndarray, float]]:
    weights = initial_weights(scenario)
    path = []
    for step in range(STEPS + 1):
        loss, gradient = loss_and_exact_gradient(weights, scenario)
        path.append((weights.copy(), loss))
        if step < STEPS:
            weights = weights - scenario.learning_rate * gradient
    return path


def scipy_loss_batch(weights: np.ndarray, scenario: Scenario) -> np.ndarray:
    """Vectorized scalar loss required by scipy.differentiate.jacobian."""
    if scenario.dimension == 2:
        residuals = np.stack(
            [weights[0] + weights[1] * index - (1.0 + 2.0 * index)
             for index in range(4)], axis=0
        )
        return np.mean(residuals * residuals, axis=0)
    target = target_weights(scenario).reshape(
        (scenario.dimension,) + (1,) * (weights.ndim - 1)
    )
    residual = weights - target
    return np.sum(residual * residual, axis=0)


def scipy_path(scenario: Scenario) -> list[tuple[np.ndarray, float]]:
    """Fixed-rate descent whose gradient is computed by SciPy itself."""
    weights = initial_weights(scenario)
    path = []
    for step in range(STEPS + 1):
        loss = float(scipy_loss_batch(weights, scenario))
        path.append((weights.copy(), loss))
        if step < STEPS:
            derivative = jacobian(
                lambda trial: scipy_loss_batch(trial, scenario), weights
            )
            if not np.all(derivative.success):
                raise AssertionError(
                    f"{scenario.name}, step {step}: SciPy Jacobian failed"
                )
            gradient = np.asarray(derivative.df, dtype=np.float64).reshape(-1)
            if gradient.shape != weights.shape or not np.all(np.isfinite(gradient)):
                raise AssertionError(
                    f"{scenario.name}, step {step}: invalid SciPy gradient"
                )
            weights = weights - scenario.learning_rate * gradient
    return path


def scipy_minimum(scenario: Scenario) -> tuple[np.ndarray, float]:
    # SciPy estimates derivatives itself; it does not use our exact-gradient function.
    result = minimize(
        lambda weights: loss_and_exact_gradient(weights, scenario)[0],
        initial_weights(scenario),
        method="BFGS",
        jac="3-point",
        options={"gtol": 1e-8, "maxiter": 1000},
    )
    if not result.success:
        raise AssertionError(f"{scenario.name}: SciPy did not converge: {result.message}")
    optimum_error = float(np.max(np.abs(result.x - target_weights(scenario))))
    if optimum_error > 1e-6 or result.fun > 1e-10:
        raise AssertionError(f"{scenario.name}: SciPy minimum differs from known solution")
    return result.x, float(result.fun)


def compare_scenario(scenario: Scenario, rows: list[Row]) -> tuple[float, float, float, float]:
    numpy_reference = numpy_path(scenario)
    scipy_reference = scipy_path(scenario)
    numpy_weight_error = 0.0
    scipy_weight_error = 0.0
    maximum_loss_error = 0.0
    for row, (numpy_weights, numpy_loss), (scipy_weights, scipy_loss) in zip(
        rows, numpy_reference, scipy_reference, strict=True
    ):
        c_weights = np.asarray(row.weights)
        current_numpy_error = float(np.max(np.abs(c_weights - numpy_weights)))
        current_scipy_error = float(np.max(np.abs(c_weights - scipy_weights)))
        numpy_loss_error = abs(row.loss - numpy_loss)
        scipy_loss_error = abs(row.loss - scipy_loss)
        numpy_weight_error = max(numpy_weight_error, current_numpy_error)
        scipy_weight_error = max(scipy_weight_error, current_scipy_error)
        maximum_loss_error = max(maximum_loss_error, numpy_loss_error, scipy_loss_error)
        if (current_numpy_error > DISPLAY_TOLERANCE or
                current_scipy_error > DISPLAY_TOLERANCE or
                numpy_loss_error > DISPLAY_TOLERANCE or
                scipy_loss_error > DISPLAY_TOLERANCE):
            raise AssertionError(
                f"{scenario.name}, step {row.step}: C/Python mismatch "
                f"(NumPy w {current_numpy_error:.9g}, "
                f"SciPy w {current_scipy_error:.9g}, "
                f"loss {max(numpy_loss_error, scipy_loss_error):.9g})"
            )
    _, scipy_loss = scipy_minimum(scenario)
    return numpy_weight_error, scipy_weight_error, maximum_loss_error, scipy_loss


def main() -> int:
    completed = subprocess.run(
        ["sh", str(DEMO)], cwd=ROOT, text=True, capture_output=True, check=True
    )
    traces = parse_trace(completed.stdout)
    print(f"NumPy {np.__version__}; SciPy {scipy.__version__}")
    print("C values are displayed to six decimals; comparison tolerance = 5.1e-7")
    print("case          n  max |C-NumPy w|  max |C-SciPy w|  max |C-Python L|  SciPy minimum L")
    for scenario in SCENARIOS:
        numpy_error, scipy_error, loss_error, minimum_loss = compare_scenario(
            scenario, traces[scenario.name]
        )
        print(
            f"{scenario.name:<13} {scenario.dimension:>2}  "
            f"{numpy_error:>15.9g}  {scipy_error:>15.9g}  "
            f"{loss_error:>16.9g}  {minimum_loss:>15.9g}"
        )
        c_last = traces[scenario.name][-1]
        python_last = numpy_path(scenario)[-1]
        print(f"  step 6 C:     w={c_last.weights}, L={c_last.loss:.6f}")
        print(
            "  step 6 NumPy: "
            f"w={np.array2string(python_last[0], precision=9)}, "
            f"L={python_last[1]:.9f}"
        )

    # Prove that the comparison rejects a visibly wrong component value.
    damaged = [Row(row.step, row.loss, row.weights.copy()) for row in traces["mse-2"]]
    damaged[3].weights[1] += 0.001
    try:
        compare_scenario(SCENARIOS[1], damaged)
    except AssertionError:
        pass
    else:
        raise AssertionError("Negative control was not rejected")
    print("LAB76_PYTHON_REFERENCE_OK")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (AssertionError, ValueError, subprocess.CalledProcessError) as error:
        print(f"LAB76_PYTHON_REFERENCE_FAILED: {error}", file=sys.stderr)
        sys.exit(1)
