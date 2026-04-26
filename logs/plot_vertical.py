import json
import matplotlib.pyplot as plt

# 1. Configuración
IMAGEN_OBJETIVO = "face_swap_enhanced.png"
HILOS_OBJETIVO = 16  # Comparamos versiones bajo la misma carga
METRICAS = [
    ("real_s", "Tiempo Real (s)"),
    ("user_s", "Tiempo de Usuario (s)"),
    ("sys_s", "Tiempo de Sistema (s)")
]
COLORES = {
    "source": "#1f77b4",
    "suboptimized": "#ff7f0e",
    "optimized_async": "#2ca02c",
    "optimized_tasks": "#d62728",
    "hiperoptimized": "#9467bd"
}
ORDEN_VERSIONES = ["source", "suboptimized", "optimized_async", "optimized_tasks", "hiperoptimized"]

# 2. Carga de datos
with open('resultados_rendimiento.json', 'r') as f:
    data = json.load(f)

# Extraer valores siguiendo el orden de las versiones
resultados = {m[0]: [] for m in METRICAS}
versiones_finales = []

for v_name in ORDEN_VERSIONES:
    match = next((res for res in data['results'] 
                  if res['version'] == v_name 
                  and res['threads'] == HILOS_OBJETIVO 
                  and res['image'] == IMAGEN_OBJETIVO), None)
    
    if match:
        versiones_finales.append(v_name)
        for m_key, _ in METRICAS:
            resultados[m_key].append(match[m_key])

# 3. Crear 3 subplots verticales compartiendo el eje X
fig, axes = plt.subplots(3, 1, figsize=(10, 12), sharex=True)
fig.suptitle(f'Evolución de Tiempos por Versión ({IMAGEN_OBJETIVO} - {HILOS_OBJETIVO} hilos)', 
             fontsize=14, fontweight='bold')

for i, (m_key, m_label) in enumerate(METRICAS):
    ax = axes[i]
    tiempos = resultados[m_key]
    
    # Línea base de conexión
    ax.plot(versiones_finales, tiempos, color='#ccc', linestyle='--', alpha=0.6, zorder=1)
    
    # Dibujar cada punto con su color
    for x_val, y_val in zip(versiones_finales, tiempos):
        ax.scatter(x_val, y_val, color=COLORES[x_val], s=120, edgecolors='black', zorder=2)
        ax.text(x_val, y_val, f' {y_val:.3f}s', va='bottom', ha='left', fontsize=9, fontweight='bold')

    ax.set_ylabel(m_label)
    ax.grid(axis='y', linestyle=':', alpha=0.5)

# Ajustes finales de formato
axes[2].set_xlabel('Implementación (Orden de optimización)')
plt.xticks(rotation=15)
plt.tight_layout(rect=[0, 0.03, 1, 0.95])
plt.savefig('grafica_tiempos_vertical.png', dpi=300)
print(f"Gráfica guardada como 'grafica_tiempos_vertical.png'")