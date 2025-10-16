import pandas as pd
import matplotlib.pyplot as plt
# Leer resultados
df = pd.read_csv("resultados.csv")
# Agrupar por N y Tipo, calcular el promedio de tiempo
df_promedio = df.groupby(["N", "Tipo"], as_index=False)["Tiempo_ms"].mean()
# Crear gráfico
plt.figure(figsize=(8,6))
for tipo in df_promedio["Tipo"].unique():
 subset = df_promedio[df_promedio["Tipo"] == tipo]
 plt.plot(subset["N"], subset["Tiempo_ms"], marker="o", label=tipo)
plt.xlabel("Dimensión de la matriz (N x N)")
plt.ylabel("Tiempo de ejecución (ms)")
plt.title("Comparación de tiempos de ejecución")
plt.legend()
plt.grid(True)
plt.savefig("grafico_tiempos.png")
plt.show()