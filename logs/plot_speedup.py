import json
import matplotlib.pyplot as plt

# 1. Configuración inicial
IMAGEN_OBJETIVO = "face_swap_enhanced.png"
COLORES = {
    "source": "#1f77b4",
    "suboptimized": "#ff7f0e",
    "optimized_async": "#2ca02c",
    "optimized_tasks": "#d62728",
    "hiperoptimized": "#9467bd"
}

# 2. Cargar datos
with open('resultados_rendimiento.json', 'r') as f:
    data = json.load(f)

# Estructura para almacenar tiempos reales: version -> {hilos: tiempo}
tiempos_reales = {v: {} for v in COLORES.keys()}

for res in data['results']:
    if res.get('image') == IMAGEN_OBJETIVO:
        version = res['version']
        hilos = res['threads']
        if version in COLORES:
            tiempos_reales[version][hilos] = res['real_s']

# 3. Identificar el tiempo base (versión 'source' con 1 hilo)
tiempo_base = tiempos_reales["source"].get(1)

if not tiempo_base:
    print("Error: No se encontró el tiempo base (versión 'source' con 1 hilo).")
    exit(1)

# 4. Crear la gráfica
fig, ax = plt.subplots(figsize=(10, 6))

# Extraer y ordenar los hilos disponibles para usarlos como eje X
hilos_ordenados = sorted({h for v in tiempos_reales.values() for h in v.keys()})
# Convertir a texto para que sean equidistantes en la gráfica categórica
hilos_str = [str(h) for h in hilos_ordenados]

for version, color in COLORES.items():
    datos_version = tiempos_reales[version]
    if not datos_version:
        continue
        
    speedups = []
    hilos_plot = []
    
    for h in hilos_ordenados:
        if h in datos_version:
            tiempo_actual = datos_version[h]
            # Speedup = Tiempo base / Tiempo actual
            speedups.append(tiempo_base / tiempo_actual if tiempo_actual > 0 else 0)
            hilos_plot.append(str(h))
    
    # Dibujar la línea y los puntos
    ax.plot(hilos_plot, speedups, marker='o', linewidth=2, color=color, label=version)
    
    # NUEVO: Añadir el valor numérico en cada punto
    for x, y in zip(hilos_plot, speedups):
        ax.text(x, y + 0.05, f'{y:.2f}x', ha='center', va='bottom', 
                fontsize=9, color=color, fontweight='bold')

# 5. Formato
ax.set_title(f'Speed-up por Hilos respecto a Secuencial Base ({IMAGEN_OBJETIVO})')
ax.set_xlabel('Número de Hilos (OpenMP)')
ax.set_ylabel('Speed-up (Aceleración Relativa)')
ax.axhline(y=1, color='gray', linestyle='--', alpha=0.7) # Línea base 1x
ax.grid(axis='y', linestyle='--', alpha=0.4)
ax.legend()

plt.tight_layout()
plt.savefig('grafica_speedup.png', dpi=300)
print("Gráfica guardada como 'grafica_speedup_2.png'")