#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define MAX_COMMAND_LENGTH 100

int separaItems (char * expresion,char *** items, int * background);
void exec_cd(char * directorio);
void exec_path(char *** items, int num);

char ** items;
int num, background;
char expresion[MAX_COMMAND_LENGTH];
char error_message[30] = "An error has occurred\n";
const char *path[] = {
"./",
"/usr/bin/",
"/bin/",
NULL
};


int main() {

    while (1) {
        // Imprimir el prompt
        printf("wish> ");
        
        // Obtener la entrada del usuario
        if (fgets(expresion, MAX_COMMAND_LENGTH, stdin) == NULL) {
            perror("Error al leer la entrada");
            exit(EXIT_FAILURE);
        }

        num = separaItems (expresion, &items, &background);
            
        if(num > 0){
            if (strcmp(items[0], "exit") == 0) {
                exit(0); 
            }
            else if(strcmp(items[0], "cd") == 0 /*&& background != 1**/){
                if(num < 3){
                    exec_cd(items[1]);
                }
            }
            else if(strcmp(items[0], "path") == 0){
                exec_path(items, num);
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
                 char *** items,     // Resultado
                 int * background)   // 1 si hay un & al final
{
  int i, j, num, ult;
  char ** pp;
                    // En principio:
  *items = NULL;    //   cero parametros
  *background = 0;  //   ejecucion en primer plano

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
        write(STDERR_FILENO, error_message, strlen(error_message));
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