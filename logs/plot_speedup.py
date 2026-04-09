# logs/plot_speedup.py

import json
import matplotlib.pyplot as plt
import numpy as np
from collections import defaultdict

# 1. Cargar y promediar los datos
with open('resultados_rendimiento.json', 'r') as f:
    data = json.load(f)

versiones_raw = defaultdict(list)
for res in data['results']:
    versiones_raw[res['version']].append(res['real_s'])

versiones = list(versiones_raw.keys())
real_means = {v: np.mean(tiempos) for v, tiempos in versiones_raw.items()}

# 2. Calcular el Speed-up (Tiempo Base / Tiempo Paralelo)
tiempo_base = real_means["source"]
speedups = [tiempo_base / real_means[v] for v in versiones]

# 3. Mismos colores que la gráfica anterior
colores = {
    "source": "#1f77b4",
    "suboptimized": "#ff7f0e",
    "optimized_async": "#2ca02c",
    "optimized_tasks": "#d62728",
    "hiperoptimized": "#9467bd"
}
colores_lista = [colores[v] for v in versiones]

# 4. Configurar la gráfica
fig, ax = plt.subplots(figsize=(8, 5))

barras = ax.bar(versiones, speedups, color=colores_lista, edgecolor='black')

# Añadir el número exacto del speed-up encima de cada barra
for barra in barras:
    yval = barra.get_height()
    ax.text(barra.get_x() + barra.get_width()/2, yval + 0.1, f'{yval:.2f}x', 
            ha='center', va='bottom', fontweight='bold')

# Formato
ax.set_ylabel('Speed-up (S)')
ax.set_title('Aceleración Relativa (Speed-up) respecto a la Secuencial')
ax.axhline(y=1, color='gray', linestyle='--', alpha=0.7) # Línea base 1.0x
ax.set_xticks(range(len(versiones)))
ax.set_xticklabels(versiones, rotation=15)
ax.grid(axis='y', linestyle='--', alpha=0.3)

plt.tight_layout()
plt.savefig('grafica_speedup.png', dpi=300)
print("Gráfica de speed-up guardada como 'grafica_speedup.png'")