#!/bin/sh

set -e
set -x

CC=gcc45

$CC -W -Wall -std=iso9899:1999 -pedantic -Wno-long-long -g3 -ggdb -o du02 fit-pa1-du02-kalendar.c
