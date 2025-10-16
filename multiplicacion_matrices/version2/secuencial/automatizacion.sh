#!/bin/bash
# ============================================
# AUTOMATIZACIÓN: Optimización + OpenMP + Perfilado (una sola vez)
# ============================================

# ------------------ Compilación ------------------ #
# Versión 2 (Optimizada con Tiling)
gcc matriz.c ../../matriz_utils/matriz_utils.c -o matriz_sec_opt -O3 -pg  # NUEVO: Secuencial Optimizado con perfilado

# Con OpenMP
gcc OpenMP.c ../../matriz_utils/matriz_utils.c -o mm_openmp -fopenmp -O3 -pg

# ------------------ Configuración ------------------ #
ITERACIONES=3
N_LISTA=(100 200 400)
HILOS_LISTA=(2 4)
PROCESOS_LISTA=(2 4)
OUTPUT=resultados_metricas.csv

# Limpiar resultados previos
echo "Version,N,Hilos,Tiempo(s),User(s),System(s),CPU(%),Memoria(KB)" > $OUTPUT

# ------------------ Secuencial ------------------ #
echo -e "\n=== MÉTRICAS SECUENCIAL OPTIMIZADA ==="
for N in "${N_LISTA[@]}"; do
    for ((i=1; i<=ITERACIONES; i++)); do
        echo "Ejecutando matriz_sec_opt N=$N (iteración $i)"
        /usr/bin/time -v ./matriz_sec_opt $N 2> temp.txt > /dev/null

        # Extraer métricas
        USER=$(grep "User time" temp.txt | awk '{print $4}')
        SYS=$(grep "System time" temp.txt | awk '{print $4}')
        CPU=$(grep "Percent of CPU" temp.txt | awk '{print $8}')
        MEM=$(grep "Maximum resident set size" temp.txt | awk '{print $6}')
        ELAPSED=$(grep "Elapsed" temp.txt | awk '{print $8}')

        echo "Secuencial,$N,1,$ELAPSED,$USER,$SYS,$CPU,$MEM" >> $OUTPUT
    done
done

# Solo al final: generar perfilado secuencial
if [ -f gmon.out ]; then
    gprof ./matriz_sec_opt gmon.out > perfil_sec.txt
    echo "Perfil secuencial guardado en perfil_sec.txt"
    rm -f gmon.out
fi

# ------------------ OpenMP ------------------ #
echo -e "\n=== MÉTRICAS OPENMP ==="
for N in "${N_LISTA[@]}"; do
    for H in "${HILOS_LISTA[@]}"; do
        export OMP_NUM_THREADS=$H
        for ((i=1; i<=ITERACIONES; i++)); do
            echo "Ejecutando mm_openmp N=$N H=$H (iteración $i)"
            /usr/bin/time -v ./mm_openmp $N 2> temp.txt > /dev/null

            USER=$(grep "User time" temp.txt | awk '{print $4}')
            SYS=$(grep "System time" temp.txt | awk '{print $4}')
            CPU=$(grep "Percent of CPU" temp.txt | awk '{print $8}')
            MEM=$(grep "Maximum resident set size" temp.txt | awk '{print $6}')
            ELAPSED=$(grep "Elapsed" temp.txt | awk '{print $8}')

            echo "OpenMP,$N,$H,$ELAPSED,$USER,$SYS,$CPU,$MEM" >> $OUTPUT
        done
    done
done

# Solo al final: generar perfilado OpenMP
if [ -f gmon.out ]; then
    gprof ./mm_openmp gmon.out > perfil_openmp.txt
    echo "Perfil OpenMP guardado en perfil_openmp.txt"
    rm -f gmon.out
fi

echo -e "\n✅ Métricas completas guardadas en: $OUTPUT"
