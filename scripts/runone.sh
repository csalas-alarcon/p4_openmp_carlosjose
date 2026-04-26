#!/bin/bash
#runone.sh

# Comprobar que se le pasan los argumentos correctos
if [ "$#" -ne 3 ]; then
    echo "Uso: ./runone.sh <version> <ruta_imagen> <hilos>"
    echo "Ejemplo: ./runone.sh optimo_async mi_imagen.png 4"
    exit 1
fi

VERSION=$1
IMAGE_PATH=$(realpath "$2")
THREADS=$3
TARGET_DIR="src/$VERSION/Task1"

# Comprobar que la carpeta existe
if [ ! -d "$TARGET_DIR" ]; then
    echo "Error: El directorio $TARGET_DIR no existe."
    exit 1
fi

echo "==> Compilando y ejecutando Task 1 para la versión: $VERSION con $THREADS hilos"

cd "$TARGET_DIR" || exit

# 1. Limpieza total
rm -rf build
mkdir build
cd build || exit

# 2. Configurar y compilar
cmake ..
make

# 3. Ejecutar inyectando la variable de OpenMP
if [ -x "detect" ]; then
    echo "==> Ejecutando con la imagen proporcionada..."
    OMP_NUM_THREADS=$THREADS /usr/bin/time -p ./detect "$IMAGE_PATH"
else
    echo "Error: La compilación falló, no se generó el binario 'detect'."
    exit 1
fi