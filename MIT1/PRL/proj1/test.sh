#!/bin/bash

dd if=/dev/random bs=1 count=16 of=numbers 2> /dev/null
mpic++ -o pms pms.cpp
mpirun -np 5 pms < numbers
rm -f pms numbers