#!/bin/sh

set -e
set -x

CFLAGS="-O0 -g3 -fmudflap" #-ggdb -DDEBUG
CC=gcc46
LDFLAGS="-lmudflap" # -ldmalloc"

$CC $CFLAGS -W -Wall -std=iso9899:1990 -pedantic -save-temps -fverbose-asm -o du06 du06.c $LDFLAGS
