#!/bin/bash

gcc -fopenmp entorno.cc -o entornoyes
gcc entorno.cc -o entornono

./entornoyes
./entornono

rm -rf entornoyes entornono