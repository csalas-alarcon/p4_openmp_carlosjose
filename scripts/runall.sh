#!/bin/bash

# Guardamos la ruta raíz del proyecto para referencias seguras
PROJECT_ROOT="$PWD"
COMPUTER_ID=$(hostname)
OUTPUT_FILE="$PROJECT_ROOT/resultados_rendimiento.json"

# Definimos las rutas absolutas de las dos imágenes
IMG1="$PROJECT_ROOT/src/source/Task1/compressions.png"
IMG2="$PROJECT_ROOT/src/source/Task1/face_swap_enhanced.png"
IMAGES=("$IMG1" "$IMG2")

# NUEVO: Secuencia de hilos a evaluar
THREADS_LIST=(1 2 4 8 16)

# Iniciar la estructura JSON
echo "{" > "$OUTPUT_FILE"
echo "  \"computer_id\": \"$COMPUTER_ID\"," >> "$OUTPUT_FILE"
echo "  \"results\": [" >> "$OUTPUT_FILE"

VERSIONS=("source" "suboptimized" "optimized_async" "optimized_tasks" "hiperoptimized")
FIRST=1

for VERSION in "${VERSIONS[@]}"; do
    echo "==> Evaluando versión: $VERSION..."
    TARGET_DIR="$PROJECT_ROOT/src/$VERSION/Task1"
    
    if [ ! -d "$TARGET_DIR" ]; then
        echo "Saltando $VERSION (no existe)."
        continue
    fi
    
    cd "$TARGET_DIR" || exit
    rm -rf build && mkdir build && cd build || exit
    
    # Compilar en silencio una sola vez por versión
    cmake .. > /dev/null 2>&1
    make > /dev/null 2>&1
    
    if [ -x "detect" ]; then
        # NUEVO: Bucle para iterar sobre la cantidad de hilos
        for THREADS in "${THREADS_LIST[@]}"; do
            # Ejecutar para cada imagen
            for IMG_PATH in "${IMAGES[@]}"; do
                IMG_NAME=$(basename "$IMG_PATH")
                echo "  -> Hilos: $THREADS | Procesando imagen: $IMG_NAME"
                
                # NUEVO: Ejecutar inyectando la variable OMP_NUM_THREADS
                OUTPUT=$(OMP_NUM_THREADS=$THREADS /usr/bin/time -p ./detect "$IMG_PATH" 2>&1)
                
                # Extraer tiempos internos (ms)
                SRM_3=$(echo "$OUTPUT" | grep "3SRM elapsed time:" | grep -oE '[0-9]+' | tail -n 1)
                SRM_5=$(echo "$OUTPUT" | grep "5SRM elapsed time:" | grep -oE '[0-9]+' | tail -n 1)
                ELA=$(echo "$OUTPUT" | grep "ELA elapsed time:" | grep -oE '[0-9]+' | tail -n 1)
                DCT_INV=$(echo "$OUTPUT" | grep "1DCT elapsed time:" | grep -oE '[0-9]+' | tail -n 1)
                DCT_DIR=$(echo "$OUTPUT" | grep "0DCT elapsed time:" | grep -oE '[0-9]+' | tail -n 1)

                # Extraer tiempos del sistema (segundos)
                REAL_TIME=$(echo "$OUTPUT" | grep "^real" | awk '{print $2}')
                USER_TIME=$(echo "$OUTPUT" | grep "^user" | awk '{print $2}')
                SYS_TIME=$(echo "$OUTPUT" | grep "^sys" | awk '{print $2}')

                # Formatear el JSON
                if [ $FIRST -eq 0 ]; then echo "    ," >> "$OUTPUT_FILE"; fi
                
                # NUEVO: Añadido el campo "threads" a la estructura
                cat <<EOF >> "$OUTPUT_FILE"
    {
      "version": "$VERSION",
      "threads": $THREADS,
      "image": "$IMG_NAME",
      "srm_3x3_ms": ${SRM_3:-0},
      "srm_5x5_ms": ${SRM_5:-0},
      "ela_ms": ${ELA:-0},
      "dct_inverse_ms": ${DCT_INV:-0},
      "dct_direct_ms": ${DCT_DIR:-0},
      "real_s": ${REAL_TIME:-0},
      "user_s": ${USER_TIME:-0},
      "sys_s": ${SYS_TIME:-0}
    }
EOF
                FIRST=0
            done
        done
    else
        echo "Error compilando $VERSION"
    fi
done

# Cerrar el JSON y volver a la raíz
echo "  ]" >> "$OUTPUT_FILE"
echo "}" >> "$OUTPUT_FILE"
cd "$PROJECT_ROOT" || exit

echo "==> ¡Pruebas completadas! Resultados guardados en resultados_rendimiento.json"