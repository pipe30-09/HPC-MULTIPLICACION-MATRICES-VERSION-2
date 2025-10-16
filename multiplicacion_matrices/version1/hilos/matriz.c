#include "../../matriz_utils/matriz_utils.h" 
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>
//Para correr parado en hilos 
// gcc matriz.c ../../matriz_utils/matriz_utils.c -o matriz_hilos
typedef struct {
    int inicio;
    int fin;
    int **A;     //int **A → dirección de un arreglo de punteros a filas.
    int **B;
    int **C;
    int N;
} Rango;  //Se creó un tipo de dato llamado Rango


// Función que ejecuta cada hilo
void* multiply_matrices(void* arg) {
    Rango* rango = (Rango*)arg;
    int N = rango->N;
    int **A = rango->A;
    int **B = rango->B;
    int **C = rango->C;

    for (int i = rango->inicio; i < rango->fin; i++) {
        for (int j = 0; j < N; j++) {
            long long sum = 0;
            for (int k = 0; k < N; k++) {
                sum += (long long)A[i][k] * B[k][j];
            }
            C[i][j] = (int)sum;
        }
    }
  
}

// ----------- Programa principal ----------- //

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Uso: %s <tamaño N> <num_hilos>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]); // Convertir argumento a entero
    int NUM_HILOS = atoi(argv[2]); // Número de hilos
    if (N <= 0 || NUM_HILOS <= 0) return 1;


    pthread_t hilos[NUM_HILOS];  //Identificador de la variable hilo
    Rango rangos[NUM_HILOS];     //Vector de tipo de dato Rango
    srand((unsigned)time(NULL));

    int **A = allocate_matrix(N);
    int **B = allocate_matrix(N);
    int **C = allocate_matrix(N);
    if (!A || !B || !C) return 1;

    fill_random_matrix(A, N);
    fill_random_matrix(B, N);

    int filas_por_hilo = N / NUM_HILOS;   //Repartir hilos
    int resto = N % NUM_HILOS;


    // ----------------- Medición ----------------- //
    iniciar_temporizador(RUSAGE_SELF);

    // ----------------- Lanzar hilos ----------------- //
    for (int i = 0; i < NUM_HILOS; i++) {
        // Definir filas que procesará este hilo
        rangos[i].inicio = i * filas_por_hilo + (i < resto ? i : resto);
        rangos[i].fin = rangos[i].inicio + filas_por_hilo + (i < resto ? 1 : 0);
    
        // Pasar punteros a las matrices y tamaño
        rangos[i].A = A;
        rangos[i].B = B;
        rangos[i].C = C;
        rangos[i].N = N;
    
        // Crear el hilo y pasarle su rango
        pthread_create(&hilos[i], NULL, multiply_matrices, &rangos[i]);
    }
    

    
    for (int i = 0; i < NUM_HILOS; i++) {
        // Espera a que cada hilo termine su ejecución antes de continuar
        pthread_join(hilos[i], NULL);
    }
    
     // ----------------- Fin medición ----------------- //
     fin_medicion();

//////////////////////////////////////////////////////////////////////////////////////////////

    FILE *f = fopen("resultado_hilos.txt", "w");
    if (!f) {
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


