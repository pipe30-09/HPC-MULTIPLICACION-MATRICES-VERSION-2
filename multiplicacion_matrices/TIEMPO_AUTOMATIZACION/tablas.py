import pandas as pd
import matplotlib.pyplot as plt
import os

# === 1️⃣ Crear carpeta de salida ===
output_dir = "tablas"
os.makedirs(output_dir, exist_ok=True)

# === 2️⃣ Leer CSV ===
try:
    df = pd.read_csv("resultados.csv")
except FileNotFoundError:
    print("❌ No se encontró 'resultados.csv'. Asegúrate de que exista en esta carpeta.")
    exit()

df.columns = [c.strip() for c in df.columns]

# === 3️⃣ Convertir 'M:SS.xx' a segundos ===
def convertir_tiempo(t):
    partes = str(t).split(":")
    if len(partes) == 2:
        return float(partes[0]) * 60 + float(partes[1])
    else:
        return float(partes[0])

df["Tiempo_s"] = df["Tiempo_ms"].apply(convertir_tiempo)

# === 4️⃣ Calcular promedio por N, Tipo y Hilos ===
promedios = (
    df.groupby(["N", "Tipo", "Hilos"], as_index=False)["Tiempo_s"]
    .mean()
    .sort_values(by=["Tipo", "Hilos", "N"])
)

# === 5️⃣ Preparar datos para la tabla ===
tabla_datos = promedios.copy()
tabla_datos["Tiempo promedio (s)"] = tabla_datos["Tiempo_s"].round(4)
tabla_datos = tabla_datos[["N", "Tipo", "Hilos", "Tiempo promedio (s)"]]

# === 6️⃣ Crear figura con tabla ===
fig, ax = plt.subplots(figsize=(10, len(tabla_datos)*0.5 + 2))
ax.axis('off')  # quitar ejes

# Definir colores por tipo
colores_tipo = {
    "Memoria-Optimizada": "#ADD8E6",  # lightblue
    "CPU-Optimizada": "#90EE90",      # lightgreen
    "OpenMP": "#F08080"                # lightcoral
}

# Colorear filas según el tipo
fila_colores = [colores_tipo.get(t, "white") for t in tabla_datos["Tipo"]]

# Crear tabla en matplotlib
tabla = ax.table(
    cellText=tabla_datos.values,
    colLabels=tabla_datos.columns,
    cellColours=[[color]*len(tabla_datos.columns) for color in fila_colores],
    loc='center',
    cellLoc='center'
)
tabla.auto_set_font_size(False)
tabla.set_fontsize(12)
tabla.scale(1, 1.5)  # ajustar tamaño de celdas

# Guardar la tabla como PNG
output_file = os.path.join(output_dir, "tabla_tiempos_promedio.png")
plt.savefig(output_file, bbox_inches='tight')
plt.close()

print(f"✅ Tabla de tiempos promedio guardada en: {output_file}")
