#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

#include <stdio.h>

// Generar un entero aleatorio de 32 bits
int random_int32(void);

// Reservar memoria para una matriz NxN
int **allocate_matrix(int N);

// Liberar memoria de una matriz NxN
void free_matrix(int **M, int N);

// Llenar una matriz con valores aleatorios
void fill_random_matrix(int **M, int N);

// Guardar una matriz en archivo
void save_matrix(FILE *f, int **M, int N, const char *name);

// Temporizador con selección de recurso
// target = RUSAGE_SELF (proceso actual) o RUSAGE_CHILDREN (procesos hijos)
void iniciar_temporizador(int target);
void fin_medicion(void);

#endif // MATRIX_UTILS_H
