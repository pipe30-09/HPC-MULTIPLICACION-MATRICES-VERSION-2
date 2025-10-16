#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include "matriz_utils.h"  // Ajustado a tu ruta local si es necesario

// -----------------------------
// Generar un entero aleatorio de 32 bits
// -----------------------------
int random_int32(void) {
    return (rand() << 16) ^ rand();
}

// -----------------------------
// Reservar memoria para matriz NxN
// -----------------------------
int **allocate_matrix(int N) {
    int **M = malloc(N * sizeof(int *));
    if (!M) return NULL;

    for (int i = 0; i < N; i++) {
        M[i] = malloc(N * sizeof(int));
        if (!M[i]) {
            for (int j = 0; j < i; j++) free(M[j]);
            free(M);
            return NULL;
        }
    }
    return M;
}

// -----------------------------
// Liberar memoria de matriz NxN
// -----------------------------
void free_matrix(int **M, int N) {
    if (!M) return;
    for (int i = 0; i < N; i++) free(M[i]);
    free(M);
}

// -----------------------------
// Llenar matriz con valores aleatorios
// -----------------------------
void fill_random_matrix(int **M, int N) {
    if (!M) return;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            M[i][j] = random_int32();
}

// -----------------------------
// Guardar matriz en archivo
// -----------------------------
void save_matrix(FILE *f, int **M, int N, const char *name) {
    if (!f || !M) return;
    fprintf(f, "Matriz %s:\n", name);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(f, "%11d ", M[i][j]);
        }
        fprintf(f, "\n");
    }
    fprintf(f, "\n");
}

// -----------------------------
// Temporizador global
// -----------------------------
static struct timespec start, end;
static struct rusage usage_start, usage_end;
static int usage_target;  // RUSAGE_SELF o RUSAGE_CHILDREN

void iniciar_temporizador(int target) {
    usage_target = target;
    clock_gettime(CLOCK_MONOTONIC, &start);
    getrusage(usage_target, &usage_start);
}

void fin_medicion(void) {
    clock_gettime(CLOCK_MONOTONIC, &end);
    getrusage(usage_target, &usage_end);

    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;

    double user_time = (usage_end.ru_utime.tv_sec  - usage_start.ru_utime.tv_sec) +
                       (usage_end.ru_utime.tv_usec - usage_start.ru_utime.tv_usec) / 1e6;

    double sys_time  = (usage_end.ru_stime.tv_sec  - usage_start.ru_stime.tv_sec) +
                       (usage_end.ru_stime.tv_usec - usage_start.ru_stime.tv_usec) / 1e6;

    double cpu_percent = ((user_time + sys_time) / elapsed) * 100.0;

    printf("\nUser: %.2f s\nSystem: %.2f s\nCPU: %.0f%%\nElapsed: %.2f s\n",
           user_time, sys_time, cpu_percent, elapsed);
}
