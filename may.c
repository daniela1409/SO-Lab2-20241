#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGUMENTS 64

void print_error() {
    char error_message[] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}


void parse_input(char *input, char **arguments) {
    char *token;
    int i = 0;
    token = strtok(input, " \t\r\n");
    while (token != NULL && i < MAX_ARGUMENTS - 1) {
        arguments[i++] = token;
        token = strtok(NULL, " \t\r\n");
    }
    arguments[i] = NULL;
}

void execute_command(char **arguments, char **path) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(arguments[0], arguments);
        print_error();
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        print_error();
    } else {
        wait(NULL);
    }
}

void execute_parallel_commands(char **commands, char **path) {
    int num_commands = 0;
    while (commands[num_commands] != NULL) {
        num_commands++;
    }
    pid_t pids[num_commands];
    int status;

    for (int i = 0; i < num_commands; i++) {
        char *arguments[MAX_ARGUMENTS];
        parse_input(commands[i], arguments);

        pids[i] = fork();
        if (pids[i] == 0) {
            execute_command(arguments, path);
            exit(EXIT_SUCCESS);
        } else if (pids[i] < 0) {
            print_error();
        }
    }

    for (int i = 0; i < num_commands; i++) {
        waitpid(pids[i], &status, 0);
    }
}

int main(int argc, char *argv[]) {
    FILE *input_file = stdin;
    char command[MAX_COMMAND_LENGTH];
    char *path[MAX_ARGUMENTS] = {"/bin", NULL};

    if (argc > 1) {
        input_file = fopen(argv[1], "r");
        if (input_file == NULL) {
            print_error();
            exit(EXIT_FAILURE);
        }
    }

    while (1) {
        if (input_file == stdin) {
            printf("wish> ");
            fflush(stdout);
        }
        if (fgets(command, sizeof(command), input_file) == NULL) {
            if (input_file != stdin) {
                fclose(input_file);
            }
            exit(0);
        }

        // Eliminar el salto de línea del final de la entrada
        command[strcspn(command, "\n")] = '\0';
        if (strcmp(command, "exit") == 0) {
            printf("Saliendo de la shell...\n");
            exit(0); // Salir del bucle
        }

        char *commands[MAX_ARGUMENTS];
        char *token;
        int i = 0;
        token = strtok(command, "&\n");
        while (token != NULL && i < MAX_ARGUMENTS - 1) {
            commands[i++] = token;
            token = strtok(NULL, "&\n");
        }
        commands[i] = NULL;

        execute_parallel_commands(commands, path);
    }

    return 0;
}
