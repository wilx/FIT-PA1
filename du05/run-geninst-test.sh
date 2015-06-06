#!/bin/sh

set -x

GENINST=./geninst-du05
INST_DIR=./instances
SUBJECT=./du05
MAX=100

i=0
while test $i -lt $MAX ; do
    num=`printf "%08d" $i`
    ${GENINST} >${INST_DIR}/${num}_in.txt 2>${INST_DIR}/${num}_err.txt
    rc=$?

    if test $rc -ne 0 ; then
        echo "geninst error ${rc}, inst ${i} (${num})"
        exit 1
    fi

    eval `dmalloc all -v -i 1 -l ${INST_DIR}/${num}_dmalloc.log`
    
    ${SUBJECT} <${INST_DIR}/${num}_in.txt >${INST_DIR}/${num}_out.txt 2>&1
    rc=$?

    if test $rc -ne 0 ; then
        echo "$SUBJECT error ${rc}, inst ${i} (${num})"
        exit 1
    fi    
    
    i=$((${i} + 1))
done
