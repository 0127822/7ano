#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>


#define MAX_COMMAND_LENGTH 256
#define MAX_ARG_LENGTH 20


char command[MAX_COMMAND_LENGTH];
char *execution_command[MAX_ARG_LENGTH];
char x = 0;


void getCommand();
void parseCommandToExecute();
void createChildToExecuteCommand();
void simpleShell();




int main(){

    simpleShell();
    return 0;

}

void simpleShell(){

    while(1){

        getCommand();
        parseCommandToExecute();
        if(!strcmp(execution_command[0] , "exit()")) break;
        if(!strcmp(execution_command[0] , "")) continue;
        createChildToExecuteCommand();

    }

}


void getCommand(){
    printf("Shell>> ");
    fgets(command , MAX_COMMAND_LENGTH , stdin);
}

void parseCommandToExecute(){
    char *parsed;
    const char delimiter[2] = " ";
    char i = 0;


    if (command[strlen(command) - 1] == '\n') {
        command[strlen(command) - 1] = '\0';
    }
    parsed = strtok(command ,  delimiter);

    while (parsed != NULL){

        int len = strlen(parsed);
        while (len > 0 && isspace(parsed[len-1])) {
            parsed[--len] = '\0';
        }

        execution_command[i] = parsed;
        i++;
        parsed = strtok(NULL  ,  delimiter);
    }


}

void createChildToExecuteCommand(){
    pid_t child = fork();

    if(child == 0){

        execvp(execution_command[0] , execution_command);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else if (child == -1){
        printf("Can not create this Child \n");
    }
    else{
        waitpid(child , NULL , 0);
    }

}