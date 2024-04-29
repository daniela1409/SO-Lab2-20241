#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <command>\n", argv[0]);
        return 1;
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Fallo al crear el proceso hijo\n");
        return 1;
    } else if (pid == 0) {
        // Proceso hijo
        if (execvp(argv[1], &argv[1]) < 0) {
            fprintf(stderr, "Error al ejecutar el comando\n");
            return 1;
        }
    } else {
        // Proceso padre
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            gettimeofday(&end, NULL);
            double elapsed = (end.tv_sec - start.tv_sec) +
                             (end.tv_usec - start.tv_usec) / 1000000.0;
            printf("Tiempo transcurrido: %.2f segundos\n", elapsed);
        } else {
            fprintf(stderr, "El comando no terminó correctamente\n");
            return 1;
        }
    }

    return 0;
}
