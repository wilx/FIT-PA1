#!/bin/sh

set -e
set -x

CFLAGS=-O1 #-DDEBUG
CC=gcc45
LDFLAGS=-lmudflap # -ldmalloc

$CC $CFLAGS -fmudflap -W -Wall -std=iso9899:1990 -pedantic -save-temps -fverbose-asm -g3 -ggdb -o du05 du05.c $LDFLAGS
