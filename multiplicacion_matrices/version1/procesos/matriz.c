#include "../../matriz_utils/matriz_utils.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <sys/resource.h>
// ----------------- Estructura para el contexto general -----------------
typedef struct {
    int N;               // Tamaño de la matriz
    int NUM_PROCESOS;    // Número de procesos
    int NUM_HILOS;       // Número de hilos por proceso
    int **A;             // Matriz A
    int **B;             // Matriz B
    int **C;             // Matriz C (en memoria compartida)
    int shmid;           // ID de la memoria compartida
    int *shm_ptr;        // Puntero a la memoria compartida
    pid_t *procesos;     // Vector de PIDs de los procesos hijos
} MatrixContext;

// ----------------- Estructura para hilos -----------------
typedef struct {
    int inicio;
    int fin;
    MatrixContext *ctx; // Referencia al contexto para acceder a matrices y N
} Rango;

// Función para realizar la multiplicación de matrices en un rango específico
void* multiply_matrices(void* arg) {
    Rango* rango = (Rango*)arg;
    int N = rango->ctx->N;
    int **A = rango->ctx->A;
    int **B = rango->ctx->B;
    int **C = rango->ctx->C;

    for (int i = rango->inicio; i < rango->fin; i++) {
        for (int j = 0; j < N; j++) {
            long long sum = 0;
            for (int k = 0; k < N; k++) {
                sum += (long long)A[i][k] * B[k][j];
            }
            C[i][j] = (int)sum;
        }
    }
    return NULL;
}

// Función para inicializar matrices y memoria compartida
void initialize_matrices_and_memory(MatrixContext *ctx) {
    srand((unsigned)time(NULL));
    ctx->A = allocate_matrix(ctx->N);
    ctx->B = allocate_matrix(ctx->N);
    if (!ctx->A || !ctx->B) exit(1);

    fill_random_matrix(ctx->A, ctx->N);
    fill_random_matrix(ctx->B, ctx->N);

    ctx->shmid = shmget(IPC_PRIVATE, ctx->N * ctx->N * sizeof(int), IPC_CREAT | 0666);
    if (ctx->shmid < 0) {
        perror("shmget");
        exit(1);
    }
    ctx->shm_ptr = (int*)shmat(ctx->shmid, NULL, 0);
    if (ctx->shm_ptr == (void*)-1) {
        perror("shmat");
        exit(1);
    }

    ctx->C = (int**)malloc(ctx->N * sizeof(int*));
    for (int i = 0; i < ctx->N; i++) {
        ctx->C[i] = ctx->shm_ptr + i * ctx->N;
    }
}

// Función para configurar y ejecutar hilos en un proceso
void execute_threads(int inicio_proceso, int fin_proceso, MatrixContext *ctx) {
    // Asignar dinámicamente los vectores para hilos y rangos
    pthread_t *hilos = (pthread_t*)malloc(ctx->NUM_HILOS * sizeof(pthread_t));
    Rango *rangos = (Rango*)malloc(ctx->NUM_HILOS * sizeof(Rango));
    if (!hilos || !rangos) {
        perror("malloc");
        exit(1);
    }

    int filas_por_hilo = (fin_proceso - inicio_proceso) / ctx->NUM_HILOS;
    int resto_hilos = (fin_proceso - inicio_proceso) % ctx->NUM_HILOS;

    for (int i = 0; i < ctx->NUM_HILOS; i++) {
        rangos[i].inicio = inicio_proceso + i * filas_por_hilo + (i < resto_hilos ? i : resto_hilos);
        rangos[i].fin = rangos[i].inicio + filas_por_hilo + (i < resto_hilos ? 1 : 0);
        rangos[i].ctx = ctx;
        pthread_create(&hilos[i], NULL, multiply_matrices, &rangos[i]);
    }
    for (int i = 0; i < ctx->NUM_HILOS; i++) pthread_join(hilos[i], NULL);

    // Liberar vectores de hilos y rangos
    free(hilos);
    free(rangos);
}

// Función para liberar recursos
void cleanup_resources(MatrixContext *ctx) {
    free_matrix(ctx->A, ctx->N);  // Liberar matriz A
    free_matrix(ctx->B, ctx->N);  // Liberar matriz B
    free(ctx->C);                // Liberar el arreglo de punteros de C
    shmdt(ctx->shm_ptr);         // Desconectar la memoria compartida
    shmctl(ctx->shmid, IPC_RMID, NULL);  // Eliminar el segmento de memoria compartida
    free(ctx->procesos);         // Liberar el vector de PIDs
}

// ----------------- Programa principal -----------------
int main(int argc, char *argv[]) {
    // Validar argumentos de entrada
    if (argc != 4) {
        printf("Uso: %s <tamaño N> <num_procesos> <num_hilos_por_proceso>\n", argv[0]);
        return 1;
    }

    // Crear y inicializar el contexto
    MatrixContext ctx;
    ctx.N = atoi(argv[1]);
    ctx.NUM_PROCESOS = atoi(argv[2]);
    ctx.NUM_HILOS = atoi(argv[3]);
    if (ctx.N <= 0 || ctx.NUM_PROCESOS <= 0 || ctx.NUM_HILOS <= 0) return 1;

    // Asignar vector para PIDs de procesos
    ctx.procesos = (pid_t*)malloc(ctx.NUM_PROCESOS * sizeof(pid_t));
    if (!ctx.procesos) {
        perror("malloc");
        return 1;
    }

    // Inicializar matrices y memoria compartida
    initialize_matrices_and_memory(&ctx);

    iniciar_temporizador(RUSAGE_CHILDREN);

    int filas_por_proceso = ctx.N / ctx.NUM_PROCESOS;
    int resto = ctx.N % ctx.NUM_PROCESOS;

    // Crear y gestionar procesos
    for (int p = 0; p < ctx.NUM_PROCESOS; p++) {
        ctx.procesos[p] = fork();
        if (ctx.procesos[p] == 0) {
            // Configurar y ejecutar hilos en el proceso hijo
            int inicio_proceso = p * filas_por_proceso + (p < resto ? p : resto);
            int fin_proceso = inicio_proceso + filas_por_proceso + (p < resto ? 1 : 0);
            execute_threads(inicio_proceso, fin_proceso, &ctx);
            shmdt(ctx.shm_ptr);
            exit(0);
        }
    }

    // Esperar a que todos los procesos hijos terminen
    for (int p = 0; p < ctx.NUM_PROCESOS; p++) {
        waitpid(ctx.procesos[p], NULL, 0);
    }

    fin_medicion();

    // Guardar resultados en archivo
    FILE *f = fopen("resultado_proc_hilos.txt", "w");
    if (!f) {
        perror("fopen");
        return 1;
    }
    save_matrix(f, ctx.A, ctx.N, "A");
    save_matrix(f, ctx.B, ctx.N, "B");
    save_matrix(f, ctx.C, ctx.N, "C = A * B");
    fclose(f);

    // Liberar recursos
    cleanup_resources(&ctx);

    return 0;
}




