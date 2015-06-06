#!/bin/sh

set -e
set -x

TEST_EXE=./du05
TEST_DIR=./CZE
TESTINPUT_DIR=$TEST_DIR

MUDFLAP_OPTIONS="-print-leaks -viol-abort -check-initialization -wipe-heap -wipe-stack -backtrace=16"
export MUDFLAP_OPTIONS

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
