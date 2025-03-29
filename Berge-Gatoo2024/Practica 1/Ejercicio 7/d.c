#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 

#define PIPE_READ 0
#define PIPE_WRITE 1

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
    // Los espacios al inicio se ignoran
    while(*input != '\0' && *input==' ')
        input++;

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
        int tmpin = dup(STDIN_FILENO);
        int tmpout = dup(STDOUT_FILENO);

        char* str = inputString(stdin, 30);

        int last_pid = -1;

        int cant_commands = 1;
        for(int i=0;str[i];i++)
            if(str[i]=='|')
                cant_commands++;

        int command_index = 0;

        int in = dup(tmpin);
        int out = dup(tmpout);

        for(char* cmd = strtok(str, "|");cmd != NULL;cmd = strtok(NULL, "|"), command_index++)
        {
            // Redirecciono la entrada estandar usando la entrada del pipe anterior
            dup2(in, STDIN_FILENO);
            close(in);

            // Si este es el ultimo comando del pipeline, la salida se corresponde con la salida estandar
            if(command_index == cant_commands-1)
                out = dup(tmpout);
            else
            {
                // Creo un pipe
                int p[2];
                pipe(p);
                in = p[PIPE_READ];
                out = p[PIPE_WRITE];
            }

            // Redirecciono la salida estandar
            dup2(out, STDOUT_FILENO);
            close(out);

            last_pid = fork();
            if(last_pid == 0)
                execute(cmd);
        }

        // Restauro la salida y la entrada estandar para usarlas con el proximo comando
        dup2(tmpin, STDIN_FILENO);
        dup2(tmpout, STDOUT_FILENO);
        close(tmpin);
        close(tmpout);

        waitpid(last_pid, NULL, 0);
    }
    return 0;
}