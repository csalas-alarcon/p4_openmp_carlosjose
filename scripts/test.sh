#!/bin/bash

gcc -fopenmp entorno.cc -o entornoyes
gcc entorno.cc -o entornono

./testyes
./testno

rm -rf entornoyes entornono