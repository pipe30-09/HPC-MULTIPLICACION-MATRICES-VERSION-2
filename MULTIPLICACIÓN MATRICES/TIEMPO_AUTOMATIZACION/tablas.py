import pandas as pd
import matplotlib.pyplot as plt

# Leer resultados
df = pd.read_csv("resultados.csv")

# --- 1. Gráfico Secuencial + Hilos ---
metodos_hilos = ["Secuencial", "2_hilos", "4_hilos", "8_hilos"]
num_metodos = len(metodos_hilos)
cols = 2
rows = (num_metodos + 1) // 2
fig, axes = plt.subplots(rows, cols, figsize=(16, 4 * rows))
axes = axes.flatten()

for idx, metodo in enumerate(metodos_hilos):
    ax = axes[idx]
    subset = df[df["Tipo"] == metodo]
    tabla = subset.copy()
    tabla["Ejecucion"] = tabla.groupby("N").cumcount() + 1
    tabla_pivot = tabla.pivot(index="N", columns="Ejecucion", values="Tiempo_ms")
    tabla_pivot["Promedio"] = tabla_pivot.mean(axis=1)

    ax.axis("off")
    ax.table(
        cellText=tabla_pivot.round(2).values,
        colLabels=[f"Ejec {c}" for c in tabla_pivot.columns[:-1]] + ["Promedio"],
        rowLabels=tabla_pivot.index,
        loc="center",
        cellLoc="center"
    )
    ax.set_title(f"Tiempos de ejecución - {metodo}", fontsize=12, fontweight="bold")

for j in range(idx + 1, len(axes)):
    axes[j].axis("off")

plt.tight_layout()
plt.savefig("tablas_comparativas_hilos.png")
plt.show()


# --- 2. Gráfico solo Procesos ---
metodos_procesos = ["2_procesos_1_hilo", "4_procesos_1_hilo", "8_procesos_1_hilo"]
num_metodos = len(metodos_procesos)
fig, axes = plt.subplots(1, num_metodos, figsize=(16, 4))
axes = axes.flatten()

for idx, metodo in enumerate(metodos_procesos):
    ax = axes[idx]
    subset = df[df["Tipo"] == metodo]
    tabla = subset.copy()
    tabla["Ejecucion"] = tabla.groupby("N").cumcount() + 1
    tabla_pivot = tabla.pivot(index="N", columns="Ejecucion", values="Tiempo_ms")
    tabla_pivot["Promedio"] = tabla_pivot.mean(axis=1)

    ax.axis("off")
    ax.table(
        cellText=tabla_pivot.round(2).values,
        colLabels=[f"Ejec {c}" for c in tabla_pivot.columns[:-1]] + ["Promedio"],
        rowLabels=tabla_pivot.index,
        loc="center",
        cellLoc="center"
    )
    ax.set_title(f"Tiempos de ejecución - {metodo}", fontsize=12, fontweight="bold")

plt.tight_layout()
plt.savefig("tablas_comparativas_procesos.png")
plt.show()
