#!/bin/bash

CORES=4
for i in `seq 1 $CORES`
do
    cat /dev/zero > /dev/null &
done

