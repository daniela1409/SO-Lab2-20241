#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGUMENTS 64

int separaItems (char * expresion,char *** items);
void exec_cd(char * directorio);
void exec_path(char *** items, int num);
void print_error();
void parse_input(char *input, char **arguments);
void execute_command(char **arguments, char **path);
void execute_parallel_commands(char **commands, char **path);


char ** items;
int num;
char expresion[MAX_COMMAND_LENGTH];
const char *path[] = {
"./",
"/usr/bin/",
"/bin/",
NULL
};


int main(int argc, char *argv[]) {
    FILE *input_file = stdin;
    char command[MAX_COMMAND_LENGTH];

    if (argc > 1) {
        input_file = fopen(argv[1], "r");
        if (input_file == NULL) {
            print_error();
            exit(EXIT_FAILURE);
        }
    }


    while (1) {
        // Imprimir el prompt
        printf("wish> ");
        
        // Obtener la entrada del usuario
        if (fgets(expresion, MAX_COMMAND_LENGTH, stdin) == NULL) {
            perror("Error al leer la entrada");
            exit(EXIT_FAILURE);
        }

        strcpy(command, expresion);

        num = separaItems (expresion, &items);
        command[strcspn(command, "\n")] = '\0';
        if (strcmp(command, "exit") == 0) {
            printf("Saliendo de la shell...\n");
            exit(0); // Salir del bucle
        }
            
        if(num > 0){
            if (strcmp(items[0], "exit") == 0) {
                exit(0); 
            }
            else if(strcmp(items[0], "cd") == 0 /*&& background != 1**/){
                printf("cd...\n");
                if(num < 3){
                    exec_cd(items[1]);
                }
            }
            else if(strcmp(items[0], "path") == 0){
                printf("path...\n");
                exec_path(items, num);
            }
            else{
                printf("otro...\n");
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
        }    
        if(path[0] != NULL){
            
        }
        else{
            printf("Error\n");
        }
        
    }

    return 0;
}

int separaItems (char * expresion,   // Palabras a separar
                 char *** items)   // 1 si hay un & al final
{
  int i, j, num, ult;
  char ** pp;
                    // En principio:
  *items = NULL;    //   cero parametros

  for (i=0; expresion[i]!='\0'; i++)  // Cambiar saltos de
    if (expresion[i]=='\n' ||         // linea y tabuladores
        expresion[i]=='\t')           // por espacios
      expresion[i] = ' ';

  while (*expresion==' ')   // Quitar espacios del principio
    expresion ++;

  if (*expresion=='\0')     // Si cadena vacia ...
    return 0;               // ... cero parametros

  for (i=1, num=1, ult=0; expresion[i]!='\0'; i++)
    if (expresion[i]!=' ' &&
        expresion[i-1]==' ')            // Contar palabras
    {                                   // (parametros)
      num ++;
      ult = i;     // Recordar posicion de la ultima palabra
    }

  i --;
  while (i>=0 && expresion[i]==' ')   // Quitar espacios
    expresion[i--] = '\0';            // del final

  pp = malloc ((num+1)*sizeof(char*));  // Pedir array
  if (pp==NULL) return -1;              // de punteros

  pp[0] = expresion;  // El primer parametro es facil

  for (i=1, j=1; expresion[i]!='\0'; i++)  // Localizar los
    if (expresion[i]!=' ' &&             // demas parametros,
        expresion[i-1]==' ')           // apuntar a ellos con
    {                                // los punteros del
      expresion[i-1] = '\0';       // array, y poner \0 en
      pp[j++] = expresion + i;   // lugar de espacios
    }
                  // Anyadir un puntero a NULL al final del
  pp[j] = NULL;   // array de punteros (asi es mas facil
                  // trabajar con el)
  *items = pp;

  return num;     // Devolver array y numero de palabras
}

void exec_cd(char * directorio)
{
    char cwd[1024];
    int ret = chdir (directorio);
    //Valida si hubo error al entrar a carpeta
    if (ret != 0) {
        print_error();
    }
}

void exec_path(char *** items, int num){
    int tamaño = sizeof(path) / sizeof(path[0]);
    int i;
    for (i = 1; i < num; i++)
    {
        path[i - 1] = items[i];
    }
    if(num == 1 || i >= num){
        for(int j = i - 1; j < tamaño; j++){
            if(path[j] != NULL){
                path[j] = NULL;
            }
        }
    }
}

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