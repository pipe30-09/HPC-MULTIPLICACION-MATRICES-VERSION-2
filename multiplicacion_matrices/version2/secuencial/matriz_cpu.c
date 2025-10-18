#include "../../matriz_utils/matriz_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>

// Función de multiplicación secuencial OPTIMIZADA para CPU (Loop Unrolling)
void multiply_matrices_cpu(int **A, int **B, int **C, int N) {
    int i, j, k;
    long long sum;
    // Se calcula el límite para el bucle desenrollado (múltiplo de 4 más cercano)
    int limit_N = N - (N % 4); 

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            sum = 0;
            
            // Bucle principal (k) DESENROLLADO por 4
            for (k = 0; k < limit_N; k += 4) {
                sum += (long long)A[i][k] * B[k][j];
                sum += (long long)A[i][k+1] * B[k+1][j];
                sum += (long long)A[i][k+2] * B[k+2][j];
                sum += (long long)A[i][k+3] * B[k+3][j];
            }
            
            // Bucle de cola (tail loop) para manejar el sobrante si N no es múltiplo de 4
            for (; k < N; k++) {
                sum += (long long)A[i][k] * B[k][j];
            }
            C[i][j] = (int)sum;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) return 1;
    int N = atoi(argv[1]);
    if (N <= 0) return 1;
    srand((unsigned)time(NULL));

    int **A = allocate_matrix(N);
    int **B = allocate_matrix(N);
    int **C = allocate_matrix(N);
    if (!A || !B || !C) return 1;

    fill_random_matrix(A, N);
    fill_random_matrix(B, N);
    
    iniciar_temporizador(RUSAGE_SELF);
    multiply_matrices_cpu(A, B, C, N); // Llamada a la función OPTIMIZADA
    fin_medicion();

    FILE *f = fopen("resultado_sec_cpu.txt", "w");
    if (!f) {
        // ... (manejo de error y liberación de memoria)
        free_matrix(A, N); free_matrix(B, N); free_matrix(C, N); return 1;
    }

    save_matrix(f, A, N, "A"); save_matrix(f, B, N, "B"); 
    save_matrix(f, C, N, "C = A * B");
    fclose(f);

    free_matrix(A, N); free_matrix(B, N); free_matrix(C, N);
    return 0;
}
