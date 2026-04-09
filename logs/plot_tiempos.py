# logs/plot_tiempos.py 

import json
import matplotlib.pyplot as plt
import numpy as np
from collections import defaultdict

# 1. Cargar y promediar los datos del JSON
with open('resultados_rendimiento.json', 'r') as f:
    data = json.load(f)

# Diccionario para agrupar los tiempos por versión
versiones_raw = defaultdict(lambda: {'real': [], 'user': [], 'sys': []})

for res in data['results']:
    v = res['version']
    versiones_raw[v]['real'].append(res['real_s'])
    versiones_raw[v]['user'].append(res['user_s'])
    versiones_raw[v]['sys'].append(res['sys_s'])

# Calcular la media de las dos imágenes
versiones = list(versiones_raw.keys())
real_means = [np.mean(versiones_raw[v]['real']) for v in versiones]
user_means = [np.mean(versiones_raw[v]['user']) for v in versiones]
sys_means  = [np.mean(versiones_raw[v]['sys']) for v in versiones]

# 2. Definir los colores base para cada optimización (SE MANTENDRÁN EN LA OTRA GRÁFICA)
colores = {
    "source": "#1f77b4",          # Azul
    "suboptimized": "#ff7f0e",    # Naranja
    "optimized_async": "#2ca02c", # Verde
    "optimized_tasks": "#d62728", # Rojo
    "hiperoptimized": "#9467bd"   # Morado
}

# 3. Configurar la gráfica
x = np.arange(len(versiones))
width = 0.25 # Ancho de cada barra dentro del grupo

fig, ax = plt.subplots(figsize=(10, 6))

# Dibujar las barras. Usamos el mismo color base de la versión, pero cambiamos el alpha (transparencia)
for i, v in enumerate(versiones):
    c = colores[v]
    # Dibujamos las tres barras para este grupo (esta optimización)
    ax.bar(x[i] - width, real_means[i], width, color=c, alpha=1.0, edgecolor='black', 
           label='Real Time' if i == 0 else "")
    ax.bar(x[i],         user_means[i], width, color=c, alpha=0.5, edgecolor='black', 
           label='User Time' if i == 0 else "")
    ax.bar(x[i] + width, sys_means[i],  width, color=c, alpha=0.2, edgecolor='black', 
           label='Sys Time'  if i == 0 else "")

# Formato
ax.set_ylabel('Tiempo (segundos)')
ax.set_title('Tiempos de Ejecución por Nivel de Optimización')
ax.set_xticks(x)
ax.set_xticklabels(versiones, rotation=15)
ax.legend()
ax.grid(axis='y', linestyle='--', alpha=0.7)

# Escala logarítmica opcional (descomenta la siguiente línea si la barra de hiperoptimized aplasta a las demás)
# ax.set_yscale('log')

plt.tight_layout()
plt.savefig('grafica_tiempos.png', dpi=300)
print("Gráfica de tiempos guardada como 'grafica_tiempos.png'")