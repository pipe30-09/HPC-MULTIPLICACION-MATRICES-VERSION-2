import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.gridspec import GridSpec

# Leer resultados
df = pd.read_csv("resultados.csv")

# Calcular promedio de tiempo por N y Tipo
df_promedio = df.groupby(["N", "Tipo"], as_index=False)["Tiempo_ms"].mean()

# Crear diccionario con tiempos secuenciales
tiempos_sec = df_promedio[df_promedio["Tipo"] == "Secuencial"].set_index("N")["Tiempo_ms"].to_dict()

# Calcular speedup
df_promedio["Speedup"] = df_promedio.apply(
    lambda row: tiempos_sec[row["N"]] / row["Tiempo_ms"] if row["Tipo"] != "Secuencial" else 1.0,
    axis=1
)

# Separar métodos
hilos = ["2_hilos", "4_hilos", "8_hilos"]
procesos = ["2_procesos_1_hilo", "4_procesos_1_hilo", "8_procesos_1_hilo"]

# Crear figura con GridSpec
fig = plt.figure(figsize=(14, 10))
gs = GridSpec(2, 2, figure=fig)

# Speedup hilos (superior izquierda)
ax1 = fig.add_subplot(gs[0, 0])
for tipo in hilos:
    subset = df_promedio[df_promedio["Tipo"] == tipo]
    ax1.plot(subset["N"], subset["Speedup"], marker="o", label=tipo)
ax1.set_title("Speedup Hilos")
ax1.set_xlabel("Dimensión de la matriz (N x N)")
ax1.set_ylabel("Speedup")
ax1.grid(True)
ax1.legend()

# Speedup secuencial solo como referencia (superior derecha)
ax2 = fig.add_subplot(gs[0, 1])
subset_sec = df_promedio[df_promedio["Tipo"] == "Secuencial"]
ax2.plot(subset_sec["N"], subset_sec["Speedup"], marker="o", color="black", label="Secuencial")
ax2.set_title("Secuencial")
ax2.set_xlabel("Dimensión de la matriz (N x N)")
ax2.set_ylabel("Speedup")
ax2.grid(True)
ax2.legend()

# Speedup procesos (debajo de los dos anteriores)
ax3 = fig.add_subplot(gs[1, :])
for tipo in procesos:
    subset = df_promedio[df_promedio["Tipo"] == tipo]
    ax3.plot(subset["N"], subset["Speedup"], marker="o", label=tipo)
ax3.set_title("Speedup Procesos")
ax3.set_xlabel("Dimensión de la matriz (N x N)")
ax3.set_ylabel("Speedup")
ax3.grid(True)
ax3.legend()

plt.tight_layout()
plt.savefig("grafico_speedup_distribuido.png")
plt.show()
