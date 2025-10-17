#!/bin/bash
# ============================================
# AUTOMATIZACIÓN: Secuencial + Optimización CPU + OpenMP + Perfilado
# ============================================

# ------------------ Compilación ------------------ #
echo "=== COMPILANDO TODAS LAS VERSIONES ==="

# Versión 1: Secuencial optimizada (Tiling)
gcc matriz_memoria.c ../../matriz_utils/matriz_utils.c -o matriz_sec_opt -O3 -pg

# Versión 2: Optimización CPU (Loop Unrolling)
gcc matriz_cpu.c ../../matriz_utils/matriz_utils.c -o cpu_opt -O3 -pg

# Versión 3: OpenMP
gcc OpenMP.c ../../matriz_utils/matriz_utils.c -o mm_openmp -fopenmp -O3 -pg

echo "[OK] Compilación completa."

# ------------------ Configuración ------------------ #
ITERACIONES=2
N_LISTA=(1600)
HILOS_LISTA=(2 4)
OUTPUT=resultados_metricas.csv

# Limpiar resultados previos
echo "Version,N,Hilos,Tiempo(s),User(s),System(s),CPU(%),Memoria(KB)" > $OUTPUT

# ===================================================
# OPTIMIZACIÓN MEMORIA (Tiling)
# ===================================================
echo -e "\n=== MÉTRICAS OPTIMIZACIÓN MEMORIA (Tiling) ==="
for N in "${N_LISTA[@]}"; do
    for ((i=1; i<=ITERACIONES; i++)); do
        echo "Ejecutando matriz_sec_opt N=$N (iteración $i)"
        /usr/bin/time -v ./matriz_sec_opt $N 2> temp.txt > /dev/null

        USER=$(grep "User time" temp.txt | awk '{print $4}')
        SYS=$(grep "System time" temp.txt | awk '{print $4}')
        CPU=$(grep "Percent of CPU" temp.txt | awk '{print $8}')
        MEM=$(grep "Maximum resident set size" temp.txt | awk '{print $6}')
        ELAPSED=$(grep "Elapsed" temp.txt | awk '{print $8}')

        echo "Memoria-Optimizada,$N,1,$ELAPSED,$USER,$SYS,$CPU,$MEM" >> $OUTPUT
    done
done

# Perfilado con gprof
if [ -f gmon.out ]; then
    gprof ./matriz_sec_opt gmon.out > perfil_memoria_opt.txt
    echo "Perfil Memoria optimizada guardado en perfil_memoria_opt.txt"
    rm -f gmon.out
fi


# ===================================================
# OPTIMIZACIÓN CPU (Loop Unrolling)
# ===================================================
echo -e "\n=== MÉTRICAS OPTIMIZACIÓN CPU (Loop Unrolling) ==="
for N in "${N_LISTA[@]}"; do
    for ((i=1; i<=ITERACIONES; i++)); do
        echo "Ejecutando cpu_opt N=$N (iteración $i)"
        /usr/bin/time -v ./cpu_opt $N 2> temp.txt > /dev/null

        USER=$(grep "User time" temp.txt | awk '{print $4}')
        SYS=$(grep "System time" temp.txt | awk '{print $4}')
        CPU=$(grep "Percent of CPU" temp.txt | awk '{print $8}')
        MEM=$(grep "Maximum resident set size" temp.txt | awk '{print $6}')
        ELAPSED=$(grep "Elapsed" temp.txt | awk '{print $8}')

        echo "CPU-Optimizada,$N,1,$ELAPSED,$USER,$SYS,$CPU,$MEM" >> $OUTPUT
    done
done

# Perfilado con gprof
if [ -f gmon.out ]; then
    gprof ./cpu_opt gmon.out > perfil_cpu_opt.txt
    echo "Perfil CPU optimizada guardado en perfil_cpu_opt.txt"
    rm -f gmon.out
fi


# ===================================================
# OPENMP
# ===================================================
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

# Perfilado con gprof
if [ -f gmon.out ]; then
    gprof ./mm_openmp gmon.out > perfil_openmp.txt
    echo "Perfil OpenMP guardado en perfil_openmp.txt"
    rm -f gmon.out
fi

# ===================================================
echo -e "\n TODAS LAS MÉTRICAS GUARDADAS EN: $OUTPUT"
