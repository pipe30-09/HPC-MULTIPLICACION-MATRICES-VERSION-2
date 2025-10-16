#!/bin/bash
# Para entrar al archivo: cd TIEMPO_AUTOMATIZACION
# Para dar permisos: chmod +x automatizacion.sh
# Para ejecutar: ./automatizacion.sh

# Limpiar archivo de resultados
OUTPUT="resultados.csv"
# Se añade la columna Tipo para las versiones optimizadas (ej. 4_hilos_opt)
echo "N,Tiempo_ms,Tipo" > $OUTPUT

# ------------------ Compilar programas ------------------ #
echo -e "\n=== Compilando versiones secuencial y paralelas (v1 y v2) ==="

# Versión 1 (Base - No optimizada)
cd ../version1/secuencial
gcc matriz.c ../../matriz_utils/matriz_utils.c -o matriz_sec_base # Renombrado a base

cd ../hilos
gcc matriz.c ../../matriz_utils/matriz_utils.c -o matriz_hilos -lpthread

cd ../procesos
gcc matriz.c ../../matriz_utils/matriz_utils.c -o matriz_procesos

# Versión 2 (Optimizada con Tiling)
cd ../../version2/secuencial
gcc matriz.c ../../matriz_utils/matriz_utils.c -o matriz_sec_opt # NUEVO: Secuencial Optimizado

cd ../hilos
gcc matriz.c ../../matriz_utils/matriz_utils.c -o matriz_hilos_opt -lpthread

cd ../procesos
gcc matriz.c ../../matriz_utils/matriz_utils.c -o matriz_procesos_opt

# Volver a la carpeta de automatización
cd ../../TIEMPO_AUTOMATIZACION

# ------------------ Configuración ------------------ #
ITERACIONES=10
N_LISTA=(100 200 400 800 1600 3200)
HILOS_LISTA=(2 4 8)
PROCESOS_LISTA=(2 4 8)


# ------------------ 1. Secuencial BASE (v1 - NO optimizada) ------------------ #
echo -e "\n=== Ejecutando versión Secuencial BASE (v1) ==="
cd ../version1/secuencial
for ((i=1; i<=ITERACIONES; i++)); do
    for N in "${N_LISTA[@]}"; do
        echo -e "\nEjecución $i con N=$N (Secuencial BASE)"
        SALIDA=$(./matriz_sec_base $N)
        echo "$SALIDA"
        ELAPSED=$(echo "$SALIDA" | grep "Elapsed" | awk '{print $2}')
        MS=$(echo "$ELAPSED * 1000" | bc -l)
        echo "$N,$MS,Secuencial" >> ../../TIEMPO_AUTOMATIZACION/$OUTPUT
    done
done


# ------------------ 2. Secuencial OPTIMIZADO (v2 - con Tiling) ------------------ #
echo -e "\n=== Ejecutando versión Secuencial OPTIMIZADA (v2) ==="
cd ../../version2/secuencial
for ((i=1; i<=ITERACIONES; i++)); do
    for N in "${N_LISTA[@]}"; do
        echo -e "\nEjecución $i con N=$N (Secuencial OPTIMIZADA)"
        SALIDA=$(./matriz_sec_opt $N)
        echo "$SALIDA"
        ELAPSED=$(echo "$SALIDA" | grep "Elapsed" | awk '{print $2}')
        MS=$(echo "$ELAPSED * 1000" | bc -l)
        echo "$N,$MS,Secuencial_opt" >> ../../../TIEMPO_AUTOMATIZACION/$OUTPUT # Etiqueta con "_opt"
    done
done


# ------------------ 3. Hilos (v1 - NO optimizada) ------------------ #
cd ../../version1/hilos
for H in "${HILOS_LISTA[@]}"; do
    echo -e "\n=== Ejecutando versión con $H hilos (v1) ==="
    for ((i=1; i<=ITERACIONES; i++)); do
        for N in "${N_LISTA[@]}"; do
            echo -e "\nEjecución $i con N=$N ($H hilos v1)"
            SALIDA=$(./matriz_hilos $N $H)
            echo "$SALIDA"
            ELAPSED=$(echo "$SALIDA" | grep "Elapsed" | awk '{print $2}')
            MS=$(echo "$ELAPSED * 1000" | bc -l)
            echo "$N,$MS,${H}_hilos" >> ../../TIEMPO_AUTOMATIZACION/$OUTPUT
        done
    done
done


# ------------------ 4. Hilos OPTIMIZADOS (v2 - con Tiling) ------------------ #
cd ../../version2/hilos
for H in "${HILOS_LISTA[@]}"; do
    echo -e "\n=== Ejecutando versión OPTIMIZADA con $H hilos (v2) ==="
    for ((i=1; i<=ITERACIONES; i++)); do
        for N in "${N_LISTA[@]}"; do
            echo -e "\nEjecución $i con N=$N ($H hilos OPTIMIZADOS v2)"
            SALIDA=$(./matriz_hilos_opt $N $H) # Ejecutable optimizado
            echo "$SALIDA"
            ELAPSED=$(echo "$SALIDA" | grep "Elapsed" | awk '{print $2}')
            MS=$(echo "$ELAPSED * 1000" | bc -l)
            echo "$N,$MS,${H}_hilos_opt" >> ../../../TIEMPO_AUTOMATIZACION/$OUTPUT
        done
    done
done


# ------------------ 5. Procesos (v1 - NO optimizada) ------------------ #
cd ../../version1/procesos
H=1     # un solo hilo por proceso
for P in "${PROCESOS_LISTA[@]}"; do
    echo -e "\n=== Ejecutando versión con $P procesos (v1) ==="
    for ((i=1; i<=ITERACIONES; i++)); do
        for N in "${N_LISTA[@]}"; do
            echo -e "\nEjecución $i con N=$N, $P procesos, $H hilo (v1)"
            SALIDA=$(./matriz_procesos $N $P $H)
            echo "$SALIDA"
            ELAPSED=$(echo "$SALIDA" | grep "Elapsed" | awk '{print $2}')
            MS=$(echo "$ELAPSED * 1000" | bc -l)
            echo "$N,$MS,${P}_procesos_${H}_hilo" >> ../../TIEMPO_AUTOMATIZACION/$OUTPUT
        done
    done
done


# ------------------ 6. Procesos OPTIMIZADOS (v2 - con Tiling) ------------------ #
cd ../../version2/procesos
H=1     # un solo hilo por proceso
for P in "${PROCESOS_LISTA[@]}"; do
    echo -e "\n=== Ejecutando versión OPTIMIZADA con $P procesos (v2) ==="
    for ((i=1; i<=ITERACIONES; i++)); do
        for N in "${N_LISTA[@]}"; do
            echo -e "\nEjecución $i con N=$N, $P procesos, $H hilo (v2 OPTIMIZADA)"
            SALIDA=$(./matriz_procesos_opt $N $P $H) # Ejecutable optimizado
            echo "$SALIDA"
            ELAPSED=$(echo "$SALIDA" | grep "Elapsed" | awk '{print $2}')
            MS=$(echo "$ELAPSED * 1000" | bc -l)
            echo "$N,$MS,${P}_procesos_${H}_hilo_opt" >> ../../../TIEMPO_AUTOMATIZACION/$OUTPUT
        done
    done
done


# === Generar gráficos y tablas (Ahora incluirán las versiones _opt) ===
echo -e "\nGenerando gráfico..."
cd ../../TIEMPO_AUTOMATIZACION
python3 grafico.py

echo -e "\nGenerando comparativa (Speedup)..."
python3 speedup.py

echo -e "\nGenerando tablas..."
python3 tablas.py
