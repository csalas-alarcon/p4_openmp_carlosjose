#!/bin/bash
#runone.sh

# Comprobar que se le pasan los argumentos correctos
if [ "$#" -ne 2 ]; then
    echo "Uso: ./runone.sh <version> <ruta_imagen>"
    echo "Ejemplo: ./runone.sh optimo_async mi_imagen.png"
    exit 1
fi

VERSION=$1
# Convertimos la ruta de la imagen a absoluta para evitar que se rompa al hacer 'cd'
IMAGE_PATH=$(realpath "$2")
TARGET_DIR="src/$VERSION/Task1"

# Comprobar que la carpeta existe
if [ ! -d "$TARGET_DIR" ]; then
    echo "Error: El directorio $TARGET_DIR no existe."
    exit 1
fi

echo "==> Compilando y ejecutando Task 1 para la versión: $VERSION"

cd "$TARGET_DIR" || exit

# 1. Limpieza total para evitar problemas de caché (como el de 'brian')
rm -rf build
mkdir build
cd build || exit

# 2. Configurar y compilar
cmake ..
make

# 3. Ejecutar si la compilación fue exitosa
if [ -x "detect" ]; then
    echo "==> Ejecutando time ./detect con la imagen proporcionada..."
    /usr/bin/time -p ./detect "$IMAGE_PATH"
else
    echo "Error: La compilación falló, no se generó el binario 'detect'."
    exit 1
fi