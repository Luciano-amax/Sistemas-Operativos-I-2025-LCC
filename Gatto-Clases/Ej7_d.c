#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#define COMMAND_LENGTH 1024

int remove_enter(char* string) {
int len = strlen(string);
string[len - 1] = '\0';
return len - 1;
}

void print_line() {
printf("\033[0;32muser@user:$ ");
printf("\033[0m");
}

int tokens(char *buf, char **command) {
char *token;
int tokenCounter = 0, ret = 0;
token = strtok(buf, " ");

while (token != NULL) {
if (!strcmp(token, ">")) {
token = strtok(NULL, " ");
int filefd = open(token, O_CREAT | O_WRONLY | O_TRUNC, 0644);
if (filefd == -1) {
perror("Error al abrir el archivo especificado\n");
exit(EXIT_FAILURE);
}
ret = filefd;
}
else {
command[tokenCounter] = token;
tokenCounter++;
}
token = strtok(NULL, " ");
}
return ret;
}

int commands_count(char* buf, char** commands) {
char *token;
int tokenCounter = 0;
token = strtok(buf, "|");

while (token != NULL) {
commands[tokenCounter] = token;
tokenCounter++;
token = strtok(NULL, "|");
}
return tokenCounter;
}

void quit_shell(char *buf, int commandsCounter) {
if (commandsCounter == 1 && strcmp(buf, "exit") == 0)
exit(0);
}

int set_pos(int commandsCounter, int i) {
enum Comandos pos = COMANDO_UNICO;
if (commandsCounter > 1 && i == 0)
pos = PRIMER_COMANDO;
else if (i == commandsCounter - 1 && i != 0)
pos = ULTIMO_COMANDO;
else if (i > 0 && i < commandsCounter - 1)
pos = COMANDO_N;
return pos;
}

void exec_command(char* buf, char** command, int pos, int pipefd[], int prevPipeOutput) {
int stdoutModificado = 0, stdoutCopy, stdinModificado = 0, stdinCopy;
int filefd = tokens(buf, command);

if (filefd != 0) {
stdoutModificado++;
stdoutCopy = dup(1); // 1 es el fd de la salida estandar
dup2(filefd, 1);
close(filefd);
}

if (pos == PRIMER_COMANDO) {
close(pipefd[0]);
dup2(pipefd[1], 1);
close(pipefd[1]);
}

else if (pos == COMANDO_N) {
close(pipefd[0]);
dup2(pipefd[1], 1);
dup2(prevPipeOutput, 0);
close(pipefd[1]);
close(prevPipeOutput);
}

else if (pos == ULTIMO_COMANDO) {
close(pipefd[1]);
dup2(prevPipeOutput, 0);
close(pipefd[0]);
close(prevPipeOutput);
}

int execvpReturn = execvp(command[0], command);
if (execvpReturn == -1) {
perror("Error al ejecutar el comando\n");
exit(EXIT_FAILURE);
}

if(stdoutModificado) { // restaurar salida estandar
stdoutModificado = 0;
dup2(stdoutCopy, 1);
close(stdoutCopy);
}
}


int main() {
    char *buf = malloc(sizeof(char) * COMMAND_LENGTH);
    int pipefd[2];
    enum Comandos pos;

    while (1) {
        char *command[COMMAND_LENGTH];
        char *commands[COMMAND_LENGTH];

        print_line();
        fgets(buf, COMMAND_LENGTH, stdin);
        remove_enter(buf);

        int commandsCounter = commands_count(buf, commands);
        quit_shell(buf, commandsCounter);

        int prevPipeOutput = 0; // stdin
        
        for (int i = 0; i < commandsCounter; i++) {
            buf = commands[i];
            pos = set_pos(commandsCounter, i);

            int pipeReturn = pipe(pipefd);

            if (pipeReturn == -1) {
                perror("Error al crear el pipe\n");
                exit(EXIT_FAILURE);
            }

            pid_t pid = fork();
            if (pid == 0)
                exec_command(buf, command, pos, pipefd, prevPipeOutput);
            else {
                prevPipeOutput = pipefd[0];
                if (i != 0)
                    close(pipefd[0]);
                close(pipefd[1]);
            }
        }

        for(int i = 0; i < commandsCounter; i++) {
            wait(NULL);
        }
    }
    free(buf);
    return 0;
}