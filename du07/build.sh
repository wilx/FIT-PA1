#!/bin/sh

set -e
set -x

CC=gcc46

$CC -W -Wall -std=iso9899:1990 -pedantic -save-temps -fverbose-asm -g3 -ggdb -o du07 du07.c
