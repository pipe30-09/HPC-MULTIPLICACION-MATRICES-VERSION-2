#include "../../matriz_utils/matriz_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>
#include <omp.h>  // 👈 Librería OpenMP

void multiply_matrices_omp(int **A, int **B, int **C, int N, int num_threads) {
    // Asignar número de hilos
    omp_set_num_threads(num_threads);

    // Paralelizamos el doble bucle externo (i, j)
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            long long sum = 0;
            for (int k = 0; k < N; k++) {
                sum += (long long)A[i][k] * B[k][j];
            }
            C[i][j] = (int)sum;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <N> <num_hilos>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    if (N <= 0 || num_threads <= 0) return 1;

    srand((unsigned)time(NULL));

    int **A = allocate_matrix(N);
    int **B = allocate_matrix(N);
    int **C = allocate_matrix(N);
    if (!A || !B || !C) return 1;

    fill_random_matrix(A, N);
    fill_random_matrix(B, N);

    iniciar_temporizador(RUSAGE_CHILDREN);

    // 🔥 Llamada paralela
    multiply_matrices_omp(A, B, C, N, num_threads);

    fin_medicion();

    FILE *f = fopen("resultado_openmp.txt", "w");
    if (!f) {
        perror("Error al abrir el archivo");
        free_matrix(A, N);
        free_matrix(B, N);
        free_matrix(C, N);
        return 1;
    }

    save_matrix(f, A, N, "A");
    save_matrix(f, B, N, "B");
    save_matrix(f, C, N, "C = A * B (OpenMP)");

    fclose(f);

    free_matrix(A, N);
    free_matrix(B, N);
    free_matrix(C, N);

    return 0;
}
