#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <comando>\n", argv[0]);
        return 1;
    }

    struct timeval start, end;
    gettimeofday(&start, NULL); // Obtener tiempo de inicio

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Error al crear el proceso hijo.\n");
        return 1;
    } else if (pid == 0) { // Proceso hijo
        // Ejecutar el comando
        execvp(argv[1], &argv[1]);
        // Si execvp() retorna, hubo un error
        perror("Error al ejecutar el comando");
        exit(1);
    } else { // Proceso padre
        // Esperar a que el proceso hijo termine
        wait(NULL);
        gettimeofday(&end, NULL); // Obtener tiempo de finalización

        // Calcular el tiempo transcurrido
        double elapsed_time = (end.tv_sec - start.tv_sec) + 
                              (end.tv_usec - start.tv_usec) / 1000000.0;

        // Mostrar el tiempo transcurrido
        printf("Tiempo transcurrido: %.6f segundos\n", elapsed_time);
    }

    return 0;
}

