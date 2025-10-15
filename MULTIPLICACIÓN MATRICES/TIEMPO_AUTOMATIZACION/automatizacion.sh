#!/bin/bash
# Para entrar al archivo: cd TIEMPO_AUTOMATIZACION
# Para dar permisos: chmod +x automatizacion.sh
# Para ejecutar: ./automatizacion.sh

# Limpiar archivo de resultados
OUTPUT="resultados.csv"
echo "N,Tiempo_ms,Tipo" > $OUTPUT

# ------------------ Compilar programas ------------------ #
cd ../version1/secuencial
gcc matriz.c ../../matriz_utils/matriz_utils.c -o matriz_sec

cd ../hilos
gcc matriz.c ../../matriz_utils/matriz_utils.c -o matriz_hilos -lpthread

cd ../procesos
gcc matriz.c ../../matriz_utils/matriz_utils.c -o matriz_procesos 

# ------------------ Configuración ------------------ #
ITERACIONES=10
N_LISTA=(100 200 400 800 1600 3200)
HILOS_LISTA=(2 4 8)
PROCESOS_LISTA=(2 4 8)

# ------------------ Secuencial ------------------ #
echo -e "\n=== Ejecutando versión Secuencial ==="
cd ../secuencial
for ((i=1; i<=ITERACIONES; i++)); do
    for N in "${N_LISTA[@]}"; do
        echo -e "\nEjecución $i con N=$N (Secuencial)"
        SALIDA=$(./matriz_sec $N)
        echo "$SALIDA"
        ELAPSED=$(echo "$SALIDA" | grep "Elapsed" | awk '{print $2}')
        MS=$(echo "$ELAPSED * 1000" | bc -l)
        echo "$N,$MS,Secuencial" >> ../../TIEMPO_AUTOMATIZACION/$OUTPUT
    done
done

# ------------------ Hilos ------------------ #
cd ../hilos
for H in "${HILOS_LISTA[@]}"; do
    echo -e "\n=== Ejecutando versión con $H hilos ==="
    for ((i=1; i<=ITERACIONES; i++)); do
        for N in "${N_LISTA[@]}"; do
            echo -e "\nEjecución $i con N=$N ($H hilos)"
            SALIDA=$(./matriz_hilos $N $H)
            echo "$SALIDA"
            ELAPSED=$(echo "$SALIDA" | grep "Elapsed" | awk '{print $2}')
            MS=$(echo "$ELAPSED * 1000" | bc -l)
            echo "$N,$MS,${H}_hilos" >> ../../TIEMPO_AUTOMATIZACION/$OUTPUT
        done
    done
done

# ------------------ Procesos ------------------ #
cd ../procesos
H=1   # un solo hilo por proceso
for P in "${PROCESOS_LISTA[@]}"; do
    echo -e "\n=== Ejecutando versión con $P procesos y $H hilo cada uno ==="
    for ((i=1; i<=ITERACIONES; i++)); do
        for N in "${N_LISTA[@]}"; do
            echo -e "\nEjecución $i con N=$N, $P procesos, $H hilo"
            SALIDA=$(./matriz_procesos $N $P $H)
            echo "$SALIDA"
            ELAPSED=$(echo "$SALIDA" | grep "Elapsed" | awk '{print $2}')
            MS=$(echo "$ELAPSED * 1000" | bc -l)
            echo "$N,$MS,${P}_procesos_${H}_hilo" >> ../../TIEMPO_AUTOMATIZACION/$OUTPUT
        done
    done
done

# === Generar gráfico ===
echo -e "\nGenerando gráfico..."
cd ../../TIEMPO_AUTOMATIZACION
python3 grafico.py

echo -e "\nGenerando comparativa.."
python3 speedup.py
# === Generar tablas ===
echo -e "\nGenerando tablas..."
python3 tablas.py

