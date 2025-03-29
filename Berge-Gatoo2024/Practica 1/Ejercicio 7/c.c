#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 

char *inputString(FILE* fp, size_t size){
    // El tamanio se extiende a medida que la entrada crece
    char *str;
    int ch;
    size_t len = 0;
    str = realloc(NULL, sizeof(*str)*size); //size es el tamanio inicial
    if(!str)return str;
    while(EOF!=(ch=fgetc(fp)) && ch != '\n'){
        str[len++]=ch;
        if(len==size){
            str = realloc(str, sizeof(*str)*(size+=16));
            if(!str)return str;
        }
    }
    str[len++]='\0';

    return realloc(str, sizeof(*str)*len);
}

void execute(char* input) {
    // Primero averiguo la cantidad de argumentos que hay
    int arg_number = 0;
    for(int i=0;input[i] != '\0'; i++)
        if(input[i] == ' ')
            arg_number++;

    // Creo el array de argumentos
    char** arglist = (char**)malloc((arg_number+1)*sizeof(char*));
    char* out_file = NULL;
    int len = 0;
    for(char* token = strtok(input, " "); token != NULL; token=strtok(NULL, " "))
    {
        if(!strcmp(">", token))
            out_file=strtok(NULL, " ");
        else
            arglist[len++] = token;
    }
    arglist[len]=NULL;

    // Redirecciono la salida
    if(out_file)
    {
        int fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC);
        if(fd < 0)
        {
            perror("Error al abrir el archivo");
            exit(1);
        }
        dup2(fd, STDOUT_FILENO);
    }
    execvp(input, arglist);
}

int main() {
    while(1) {
        char* str = inputString(stdin, 30);
        int pid = fork();
        if(pid == 0)
        {
            execute(str);
        }
        else
        {
            free(str);
            wait(NULL);
        }
    }
    return 0;
}