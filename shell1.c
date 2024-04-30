#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGUMENTS 64
#define MAX_PATHS 30

int separaItems (char * expresion,char *** items);
void exec_cd(char * directorio);
void exec_path( char **items, char **path, int num);
void print_error();
void parse_input(char *input, char **arguments);
void execute_command(char **arguments, char **path);
void execute_parallel_commands(char **commands, char **path);


char ** items;
int num;
char expresion[MAX_COMMAND_LENGTH];
char *path[MAX_PATHS] = {
"/bin",
NULL
};

void check_and_execute_sh(char *expresion, char **path) {
    // Verificar si la expresión contiene ".sh"
    if (strstr(expresion, ".sh") != NULL) {
        // Si se encuentra ".sh" en la expresión, haz algo aquí
        printf("La expresión contiene '.sh': %s\n", expresion);
        printf("Path despues de la masacre: %s", path[0]);
        path[0]="/tests";
        printf("Path despues sd de la masacre: %s", path[0]);
        char filepath[MAX_COMMAND_LENGTH];
        
        snprintf(filepath, sizeof(filepath), ".%s/%s", path[0], expresion);
        printf("Intentando ejecutar el script desde la ruta: %s\n", filepath);
        if (access(filepath, X_OK) == 0)
        {
                // Si el archivo de script es ejecutable, ejecutarlo directamente
                printf("Ejecutando el script directamente\n");
                execvp(filepath, expresion);
        }else{
            printf("No fue valida");
        }
        /*//print_error();*/
        exit(0);
        // Puedes realizar alguna acción específica aquí, como ejecutar un comando especial o mostrar un mensaje
    }
}

int main(int argc, char *argv[]) {
    FILE *input_file = stdin;
    char command[MAX_COMMAND_LENGTH];

    if(argc>2){
        print_error();
        exit(EXIT_FAILURE);
    }

    if (argc > 1) {
        //printf("recibio archivo");
        input_file = fopen(argv[1], "r");
        if (input_file == NULL) {
            print_error();
            exit(EXIT_FAILURE);
        }
    }
    


    while (1) {
        // Imprimir el prompt
        if (input_file == stdin) {
            printf("wish> ");
            fflush(stdout);
        }
        
        // Obtener la entrada del usuario
        if (fgets(expresion, MAX_COMMAND_LENGTH, input_file) == NULL) {
            //perror("Error al leer la entrada");
            exit(0);
        }
        // Verificar si la expresión contiene ".sh" y ejecutar alguna acción si es así
        //check_and_execute_sh(expresion, path);

        strcpy(command, expresion);

        num = separaItems (expresion, &items);

        /*for(int i = 0; i < num; i++){
            printf("items... %s \n", items[i]);
        }*/
        
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "exit") == 0) {
            //printf("Saliendo de la shell...\n");
            exit(0); // Salir del bucle
        }
            
        if(num > 0){
            if (strcmp(items[0], "exit") == 0) {
                if(num > 1){
                    print_error();
                }else{
                    exit(0); 
                }
                
            }
            else if(strcmp(items[0], "cd") == 0 /*&& background != 1**/){
                //printf("cd...\n");
                if(num < 3){
                    exec_cd(items[1]);
                }else{
                    print_error();
                }
            }
            else if(strcmp(items[0], "path") == 0){
                /*for(int i = 0; i < num; i++){
                    printf("items... %s \n", items[i]);
                }*/
                //printf("path...\n");
                exec_path(items, path, num);
            }
            else{
                if(path[0] != NULL){
                        
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
                else{
                    print_error();
        }    
        }    

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

void exec_path(char **items, char **path, int num)
{

    int i;

    for (i = 0; path[i] != NULL; i++)
    {
        //printf("path %d %s \n", i, path[i]);
        path[i] = NULL;
        //printf("path %d %s \n", i, path[i]);
    }
    // printf("items before... %c \n", items[0]);
    for (i = 1; i < num; i++)
    {
        //printf("items...%d %s \n", i, items[i]);
        path[i - 1] = (char *)malloc(strlen(items[i]) + 1);
        if (path[i-1] == NULL) {
            print_error();
            exit(0);
        }
        
        strcpy(path[i - 1], items[i]);
        //printf("path it...%d %s \n", i - 1, path[i - 1]);
    }
    path[i - 1] = NULL;
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

    int num_arguments = 0;
    while (arguments[num_arguments] != NULL) {
        num_arguments++;
    }
    //Buscamos si esta el simbolo > en los argumentos y contamos sus ocurrencias
    int redirect_index = -1;
    int redirect_cont =0;
    for (int i = 0; arguments[i] != NULL; i++) {
        if (strcmp(arguments[i], ">") == 0) {
            redirect_index = i;
            redirect_cont++;
            
        }
    }


   




    pid_t pid = fork();
    if (pid == 0) {
        
        if (redirect_index >= 0) {
        //Verificar si el ultimo argumento es >, entonces no hay archivo de salida
            if (num_arguments >= 2 && strcmp(arguments[num_arguments - 1], ">") == 0 && arguments[num_arguments] == NULL){
                print_error();
                exit(0);
            }
            //Verificar si el primer argumentos es >
            if (strcmp(arguments[0], ">") == 0){        
                print_error();
                exit(0);
            }

            //Verificar si despues del > hay mas de un argumento
            if (strcmp(arguments[num_arguments - 2], ">") != 0){        
                print_error();
                exit(0);
            }
            //Verificamos si hay mas de 1 >
            if(redirect_cont>1){
                print_error();
                exit(0);
            }


            // Verificar si hay redirección de salida
            if (num_arguments >= 3 && strcmp(arguments[num_arguments - 2], ">") == 0) {
                // Redirección de salida encontrada
                char *output_file = arguments[num_arguments - 1]; // Nombre del archivo de salida

                // Verificar si el archivo de salida es solo el símbolo de redirección
                if (strcmp(output_file, ">") == 0) {
                    // No se proporcionó un nombre de archivo válido después de '>'
                    print_error();
                    //exit(EXIT_FAILURE);
                }

                arguments[num_arguments - 2] = NULL; // Eliminar el símbolo de redirección
                arguments[num_arguments - 1] = NULL; // Eliminar el nombre del archivo de salida

                // Abrir el archivo de salida
                FILE *output = fopen(output_file, "w");
                if (output == NULL) {
                    // Manejar errores al abrir el archivo
                    print_error();
                    //exit(EXIT_FAILURE);
                }

                // Redirigir la salida estándar al archivo
                dup2(fileno(output), STDOUT_FILENO);
                fclose(output);
                execvp(arguments[0], arguments);
                print_error();
            }
        }else{
             // Verificar si algún argumento contiene ".sh"
            for (int i = 0; arguments[i] != NULL; i++) {
                if (strstr(arguments[i], ".sh") != NULL) {
                    // Si se encuentra ".sh" en el argumento, haz algo aquí
                    //printf("El argumento %s contiene '.sh'\n", arguments[i]);
                    // Puedes ejecutar alguna acción específica o realizar algún tratamiento especial
                    char filepath[MAX_COMMAND_LENGTH];
                    snprintf(filepath, sizeof(filepath), "./%s%s%s", path[i], (path[i][strlen(path[i]) - 1] != '/') ? "/" : "", arguments[0]);
                    //printf("Intentando ejecutar el script desde la ruta: %s\n", filepath);
                    if (access(filepath, X_OK) == 0)
                    {
                        // Si el archivo de script es ejecutable, ejecutarlo directamente
                        //printf("Ejecutando el script directamente\n");
                        execvp(filepath, arguments);
                    }
            }
        }
            execvp(arguments[0], arguments);
            print_error();
            //exit(EXIT_FAILURE);
        }
        
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