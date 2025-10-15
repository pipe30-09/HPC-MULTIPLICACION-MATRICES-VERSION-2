#include "../../matriz_utils/matriz_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>

// Definición de la constante del tamaño de bloque (Tiling)
#define BLOCK_SIZE 32

// Función de multiplicación secuencial OPTIMIZADA con Bloqueo de Caché
void multiply_matrices_opt(int **A, int **B, int **C, int N) {
    // 1. Inicializar C a cero (necesario para el Tiling, que es una acumulación)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
        }
    }

    // 2. Multiplicación con Bloqueo de Caché (Tiling)
    // Se agregan 3 bucles externos (k_bloque, i_bloque, j_bloque)
    for (int k_bloque = 0; k_bloque < N; k_bloque += BLOCK_SIZE) {
        for (int i_bloque = 0; i_bloque < N; i_bloque += BLOCK_SIZE) {
            for (int j_bloque = 0; j_bloque < N; j_bloque += BLOCK_SIZE) {

                // Bucles internos
                
                // Determinar límites reales para evitar salirse de N
                int i_limit = (i_bloque + BLOCK_SIZE < N) ? i_bloque + BLOCK_SIZE : N;
                int j_limit = (j_bloque + BLOCK_SIZE < N) ? j_bloque + BLOCK_SIZE : N;
                int k_limit = (k_bloque + BLOCK_SIZE < N) ? k_bloque + BLOCK_SIZE : N;

                for (int i = i_bloque; i < i_limit; i++) {
                    for (int j = j_bloque; j < j_limit; j++) {
                        long long temp_sum = C[i][j]; // Acumulación
                        for (int k = k_bloque; k < k_limit; k++) {
                            temp_sum += (long long)A[i][k] * B[k][j];
                        }
                        C[i][j] = (int)temp_sum; // Almacenar el valor acumulado
                    }
                }
            }
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
    multiply_matrices_opt(A, B, C, N); // Llamada a la función OPTIMIZADA
    fin_medicion();

    FILE *f = fopen("resultado_sec_opt.txt", "w"); // Nombre de archivo de salida
    if (!f) {
        perror("Error al abrir el archivo");
        free_matrix(A, N);
        free_matrix(B, N);
        free_matrix(C, N);
        return 1;
    }

    save_matrix(f, A, N, "A");
    save_matrix(f, B, N, "B");
    save_matrix(f, C, N, "C = A * B");

    fclose(f);

    free_matrix(A, N);
    free_matrix(B, N);
    free_matrix(C, N);

    return 0;
}
