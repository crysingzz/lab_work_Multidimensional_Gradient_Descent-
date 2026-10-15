#!/bin/sh
set -eu

project_dir=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
output=$(sh "$project_dir/AssemblyFiles/Mac/run-visualization.sh")

printf '%s\n' "$output" | rg -q 'Quadratic L\(w\)=w\^2'
printf '%s\n' "$output" | rg -q 'MSE y=w0\+w1\*x'
if [ "$(printf '%s\n' "$output" | rg -c '^step \|       loss \| relative loss$')" -ne 4 ]; then
    exit 1
fi
if [ "$(printf '%s\n' "$output" | rg -c '^-----\+------------\+------------------------------$')" -ne 4 ]; then
    exit 1
fi
printf '%s\n' "$output" | rg -Fq '   0 |   4.000000 | ##############################'
printf '%s\n' "$output" | rg -Fq '   3 |   1.048576 | ########'
printf '%s\n' "$output" | rg -Fq '     | w[0] = 1.024000'
printf '%s\n' "$output" | rg -Fq '   0 |  21.000000 | ##############################'
printf '%s\n' "$output" | rg -Fq '   6 |   0.643395 | #'
printf '%s\n' "$output" | rg -Fq '     | w[0..1] = 0.787581  1.667689'
printf '%s\n' "$output" | rg -Fq 'Separable quadratic sum((w_i-i)^2) | n=5 | eta=0.08'
printf '%s\n' "$output" | rg -Fq 'Separable quadratic sum((w_i-i)^2) | n=10 | eta=0.05'
printf '%s\n' "$output" | rg -Fq '   0 |  55.000000 | ##############################'
printf '%s\n' "$output" | rg -Fq '   6 |   6.787567 | ####'
printf '%s\n' "$output" | rg -Fq '     | w[0..4] = 0.351298  1.702596  3.053894  4.405192  5.756490'
printf '%s\n' "$output" | rg -Fq '   0 | 385.000000 | ##############################'
printf '%s\n' "$output" | rg -Fq '   6 | 108.735372 | ########'
printf '%s\n' "$output" | rg -Fq '     | w[0..4] = 0.531441  2.062882  3.594323  5.125764  6.657205'
printf '%s\n' "$output" | rg -Fq '     | w[5..9] = 8.188646  9.720087  11.251528  12.782969  14.314410'

printf '%s\n' 'LAB76_VISUALIZATION_CHECK_OK'
