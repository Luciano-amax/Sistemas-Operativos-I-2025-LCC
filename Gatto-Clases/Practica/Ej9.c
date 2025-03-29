#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void INThandler(int);

int main(void){

    signal(SIGINT, INThandler);

    while (1)
        sleep(10);
}

void INThandler(int sig){

    char c;
    signal(sig, SIG_IGN);                       //Debo ingnorarla, para que no se vuelve a capturar
    printf("OUCH, did you hit Ctrl-C?\n"
    "Do you really want to quit? [y/n] ");
    c = getchar();
    if (c == 'y' || c == 'Y')
        exit(0);
    else
        signal(SIGINT, INThandler);             
    getchar();
}

/*
1) The signal handler INThandler() is designed to handle signal SIGINT.

2) When the user press Ctrl-C, INThandler() is called with its only 
argument indicating the signal number. 

3) The first thing for INThandler() to do is to ignore the signal. 
Note that since INThandler() is installed as a Ctrl-C handler, its argument sig should contain SIGINT.

4) Then, INThandler() prints a message which asks if the user wants to continue. 
If the user input is a y or Y, INThandler() exits. 
Otherwise, it reinstalls itself as the SIGINT handler.

Note that all SIGINT occurs between these two signal() calls cannot be caught.
    
In the main program, INThandler() is installed as the SIGINT handler. 
After this, the main program enters an infinite loop. 
For each iteration, the main program just pauses itself. 
Once the user presses Ctrl-C, this key-press is caught by INThandler() and processed. 
Finally, the control returns to main and executes the next statement which is while. 
Consequently, the main program pauses itself again.

Sometimes Unix may not be able to catch your Ctrl-C. In this case, try a few more times. 
*/