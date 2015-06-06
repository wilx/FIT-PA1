#!/bin/sh

set -e
set -x

TEST_EXE=./du07
TEST_DIR=./CZE
TESTINPUT_DIR=$TEST_DIR

DIFF='diff -upd'

for input in "${TEST_DIR}"/*_in.txt; do
    reference_output="${input%_in.txt}_out.txt"
    output="${input}.test_output"

    echo "Testing with ${input}"

    RET=0
    set +e
    $TEST_EXE <"${input}" >"${output}"
    RET=$?
    set -e
    echo "Exit code: ${RET}"
    $DIFF "${reference_output}" "${output}"
done
