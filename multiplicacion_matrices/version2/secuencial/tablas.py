import pandas as pd
import matplotlib.pyplot as plt
import os

# === Crear carpeta de salida ===
output_dir = "tablas"
os.makedirs(output_dir, exist_ok=True)

# === Leer CSV ===
try:
    df = pd.read_csv("resultados.csv")
except FileNotFoundError:
    print(" No se encontró 'resultados.csv'. Asegúrate de que exista en esta carpeta.")
    exit()

df.columns = [c.strip() for c in df.columns]

# === Convertir 'M:SS.xx' a segundos ===
def convertir_tiempo(t):
    partes = str(t).split(":")
    if len(partes) == 2:
        return float(partes[0]) * 60 + float(partes[1])
    else:
        return float(partes[0])

df["Tiempo_s"] = df["Tiempo_ms"].apply(convertir_tiempo)

# === Calcular promedio por N, Tipo y Hilos ===
promedios = (
    df.groupby(["N", "Tipo", "Hilos"], as_index=False)["Tiempo_s"]
    .mean()
    .sort_values(by=["Tipo", "Hilos", "N"])
)

# === Definir colores por tipo ===
colores_tipo = {
    "Memoria-Optimizada": "#ADD8E6",  # lightblue
    "CPU-Optimizada": "#90EE90",      # lightgreen
    "OpenMP": "#F08080"                # lightcoral
}

# === Función para guardar tabla ===
def guardar_tabla(df_tabla, nombre_archivo):
    fig, ax = plt.subplots(figsize=(10, len(df_tabla)*0.5 + 2))
    ax.axis('off')
    fila_colores = [colores_tipo.get(t, "white") for t in df_tabla["Tipo"]]
    tabla = ax.table(
        cellText=df_tabla.values,
        colLabels=df_tabla.columns,
        cellColours=[[color]*len(df_tabla.columns) for color in fila_colores],
        loc='center',
        cellLoc='center'
    )
    tabla.auto_set_font_size(False)
    tabla.set_fontsize(12)
    tabla.scale(1, 1.5)
    output_file = os.path.join(output_dir, nombre_archivo)
    plt.savefig(output_file, bbox_inches='tight')
    plt.close()
    print(f"Tabla guardada: {output_file}")

# === Crear tabla general por Tipo (independiente de Hilos) ===
tipos = promedios["Tipo"].unique()
for tipo in tipos:
    df_tipo = promedios[promedios["Tipo"] == tipo].copy()
    df_tipo["Tiempo promedio (s)"] = df_tipo["Tiempo_s"].round(4)
    df_tipo = df_tipo[["N", "Tipo", "Hilos", "Tiempo promedio (s)"]]
    guardar_tabla(df_tipo, f"tabla_{tipo.replace(' ', '_')}.png")

# === Crear tabla por Tipo y Hilos ===
for tipo in tipos:
    hilos_unicos = promedios["Hilos"].unique()
    for h in hilos_unicos:
        df_th = promedios[(promedios["Tipo"] == tipo) & (promedios["Hilos"] == h)].copy()
        if df_th.empty:
            continue
        df_th["Tiempo promedio (s)"] = df_th["Tiempo_s"].round(4)
        df_th = df_th[["N", "Tipo", "Hilos", "Tiempo promedio (s)"]]
        guardar_tabla(df_th, f"tabla_{tipo.replace(' ', '_')}_{h}_hilos.png")
