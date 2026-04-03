#!/bin/bash

echo "[TASK 0]- Compilando Archivos"

make -s compile_suma_vectores
make -s compile_async
make -s compile_initialize_vectors

echo "[TASK 0]- Ejecutando Suma Vectores"
make -s run_suma_vectores

echo "[TASK 0]- Ejecutando Async"
make -s run_async

echo "[TASK 0]- Ejecutando Inicialización Vectores"
make -s run_initialize_vectors

echo "[TASK 0]- Eliminando Binarios"
make -s clean