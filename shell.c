#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 100

int main() {
    char command[MAX_COMMAND_LENGTH];

    while (1) {
        // Imprimir el prompt
        printf("wish> ");
        
        // Obtener la entrada del usuario
        if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL) {
            perror("Error al leer la entrada");
            exit(EXIT_FAILURE);
        }
        
        // Eliminar el salto de línea del final de la entrada
        command[strcspn(command, "\n")] = '\0';
        if (strcmp(command, "exit") == 0) {
            printf("Saliendo de la shell...\n");
            exit(0); // Salir del bucle
        }

        // Aquí puedes procesar el comando ingresado por el usuario
        // En esta implementación, simplemente lo imprimimos
        printf("Comando ingresado: %s\n", command);
    }

    return 0;
}
