#!/bin/bash

gcc -fopenmp test.cc -o testyes
gcc test.cc -o testno

./testyes
./testno

rm -rf testyes testno