import pandas as pd
import matplotlib
matplotlib.use('Agg')  # Para sistemas sin entorno gráfico
import matplotlib.pyplot as plt
import os

# === Crear carpeta de salida ===
output_dir = "graficos"
os.makedirs(output_dir, exist_ok=True)

# ===  Leer CSV ===
df = pd.read_csv("resultados.csv")
df.columns = [c.strip() for c in df.columns]

required_cols = {"N", "Tiempo_ms", "Tipo", "Hilos"}
if not required_cols.issubset(df.columns):
    raise ValueError(f"El CSV debe contener las columnas: {required_cols}")

# ===  Convertir 'M:SS.xx' a segundos ===
def convertir_tiempo(t):
    partes = str(t).split(":")
    if len(partes) == 2:
        return float(partes[0]) * 60 + float(partes[1])
    else:
        return float(partes[0])

df["Tiempo_s"] = df["Tiempo_ms"].apply(convertir_tiempo)

# === Promedios por N, Tipo y Hilos ===
promedios = (
    df.groupby(["N", "Tipo", "Hilos"], as_index=False)["Tiempo_s"]
    .mean()
    .sort_values(by=["Tipo", "Hilos", "N"])
)

# ===  Gráfico combinado de Tiempo promedio vs N ===
plt.figure(figsize=(12, 7))
colores = plt.cm.tab10.colors
tipos = promedios["Tipo"].unique()

for i, tipo in enumerate(tipos):
    grupo_tipo = promedios[promedios["Tipo"] == tipo]
    for hilos, datos in grupo_tipo.groupby("Hilos"):
        plt.plot(
            datos["N"],
            datos["Tiempo_s"] * 1000,
            marker="o",
            linestyle='-',
            color=colores[i % len(colores)],
            label=f"{tipo} - {hilos} hilos"
        )

plt.xlabel("Tamaño de matriz (N)")
plt.ylabel("Tiempo promedio (ms)")
plt.title("Tiempo promedio vs N por tipo e hilos (combinado)")
plt.legend(ncol=2, fontsize=9)
plt.grid(True)
plt.tight_layout()
plt.savefig(os.path.join(output_dir, "tiempo_promedio_vs_N_combinado.png"))
plt.close()
print("Gráfico combinado de Tiempo promedio guardado.")

# === Calcular Speedup por tipo ===
speedup_data = []
for tipo, grupo_tipo in promedios.groupby("Tipo"):
    min_hilos = grupo_tipo["Hilos"].min()
    base = grupo_tipo[grupo_tipo["Hilos"] == min_hilos]
    for hilos, datos in grupo_tipo.groupby("Hilos"):
        merged = pd.merge(
            base[["N", "Tiempo_s"]],
            datos[["N", "Tiempo_s"]],
            on="N",
            suffixes=("_base", "_actual")
        )
        merged["Speedup"] = merged["Tiempo_s_base"] / merged["Tiempo_s_actual"]
        merged["Tipo"] = tipo
        merged["Hilos"] = hilos
        speedup_data.append(merged)

speedup_df = pd.concat(speedup_data, ignore_index=True)

# === Gráfico combinado de Speedup ===
plt.figure(figsize=(12, 7))
for i, tipo in enumerate(tipos):
    grupo_tipo = speedup_df[speedup_df["Tipo"] == tipo]
    for hilos, datos in grupo_tipo.groupby("Hilos"):
        plt.plot(
            datos["N"],
            datos["Speedup"],
            marker="o",
            linestyle='-',
            color=colores[i % len(colores)],
            label=f"{tipo} - {hilos} hilos"
        )

max_hilos = int(df["Hilos"].max())
n_vals = sorted(df["N"].unique())
plt.plot(n_vals, [max_hilos] * len(n_vals), 'k--', label=f'Speedup ideal ({max_hilos}x)')
plt.axhline(1, color='gray', linestyle=':', label='Base (1x)')

plt.xlabel("Tamaño de matriz (N)")
plt.ylabel("Speedup (x)")
plt.title("Speedup promedio vs N - Todos los tipos")
plt.legend(ncol=2, fontsize=9)
plt.grid(True)
plt.tight_layout()
plt.savefig(os.path.join(output_dir, "speedup_promedio_combinado.png"))
plt.close()
print("Gráfico combinado de Speedup guardado.")

# === Gráficos por tipo: Tiempo promedio y Speedup ===
for i, tipo in enumerate(tipos):
    grupo_tipo = promedios[promedios["Tipo"] == tipo]
    # Tiempo promedio
    plt.figure(figsize=(10, 6))
    for hilos, datos in grupo_tipo.groupby("Hilos"):
        plt.plot(
            datos["N"],
            datos["Tiempo_s"] * 1000,
            marker="o",
            linestyle='-',
            label=f"{hilos} hilos"
        )
    plt.xlabel("Tamaño de matriz (N)")
    plt.ylabel("Tiempo promedio (ms)")
    plt.title(f"Tiempo promedio vs N - {tipo}")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, f"tiempo_promedio_{tipo.replace(' ', '_')}.png"))
    plt.close()

    # Speedup
    grupo_speedup = speedup_df[speedup_df["Tipo"] == tipo]
    plt.figure(figsize=(10, 6))
    for hilos, datos in grupo_speedup.groupby("Hilos"):
        plt.plot(
            datos["N"],
            datos["Speedup"],
            marker="o",
            linestyle='-',
            label=f"{hilos} hilos"
        )
    max_hilos_tipo = grupo_tipo["Hilos"].max()
    n_vals_tipo = sorted(grupo_tipo["N"].unique())
    plt.plot(n_vals_tipo, [max_hilos_tipo] * len(n_vals_tipo), 'k--', label=f'Speedup ideal ({max_hilos_tipo}x)')
    plt.axhline(1, color='gray', linestyle=':', label='Base (1x)')
    plt.xlabel("Tamaño de matriz (N)")
    plt.ylabel("Speedup (x)")
    plt.title(f"Speedup promedio vs N - {tipo}")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, f"speedup_{tipo.replace(' ', '_')}.png"))
    plt.close()

print(f"Todos los gráficos se guardaron en la carpeta: '{output_dir}/'")
