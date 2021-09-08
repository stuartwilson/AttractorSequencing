#!/bin/bash
for i in $(seq $3)
    do
        ./build/model $1 $2/log$i $i &
    done
