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
char NUM_OF_ARG;
char count =0;


void getCommand();
void parseCommandToExecute();
void createChildProcessToExecuteCommand();
void simpleShell();
void changeDirectoryExecution();
void commandsHandlers();
void clearExecutionArray();



int main(){
    simpleShell();
    return 0;

}

void simpleShell(){

    while(1){

        getCommand();
        parseCommandToExecute();
        if(!strcmp(execution_command[0] , "exit")) exit(EXIT_SUCCESS);
        if(!strcmp(execution_command[0] , "")) continue;
        createChildProcessToExecuteCommand();

    }

}


void getCommand(){
    char path[1024];
    getcwd(path , sizeof(path));
    printf("%s$ " , path);
    fgets(command , MAX_COMMAND_LENGTH , stdin);

}

void parseCommandToExecute(){

    NUM_OF_ARG =0;
    char *parsed;
    const char delimiter[2] = " ";

    parsed = strtok(command ,  delimiter);

    while (parsed != NULL){

        int len = strlen(parsed);
        while (len > 0 && isspace(parsed[len-1])) {
            parsed[--len] = '\0';
        }

        execution_command[NUM_OF_ARG] = parsed;
        NUM_OF_ARG++;
        parsed = strtok(NULL  ,  delimiter);
    }


}

void createChildProcessToExecuteCommand() {
    pid_t child = fork();

    if (child == 0) {
        commandsHandlers();
    }

    else if (child == -1){
        printf("Can not create this Child \n");
    }

    else{
        if(execution_command[NUM_OF_ARG] == NULL)waitpid(child , NULL , 0);
    }

    clearExecutionArray();

}

void changeDirectoryExecution() {
        if (NUM_OF_ARG <2) {
            fprintf(stderr, "cd: missing argument\n");
            return;
        } else if(!strcmp(execution_command[1], "~")){
            chdir("/home");
        } else if (chdir( execution_command[1]) != 0){
            perror("cd");
        }
}

void commandsHandlers(){

    if(!strcmp(execution_command[0] , "exit")) exit(EXIT_SUCCESS);

    else if (!strcmp(execution_command[0], "cd")) {
        changeDirectoryExecution();
    }
    else if (!strcmp(execution_command[0], "export")) {
        if (NUM_OF_ARG < 2) {
            fprintf(stderr, "export: missing argument\n");
        }
        else {
            char* variable = strtok(execution_command[1], "=");
            char* value = strtok(NULL, "=");

            if (value == NULL) {
                fprintf(stderr, "export: invalid argument\n");
            }
            else {
                setenv(variable, value, 1);
            }
        }
    }
    else {
        execvp(execution_command[0], execution_command);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
}

void clearExecutionArray(){
    for (int cmd = 0 ; cmd <NUM_OF_ARG ; cmd++ ){
        execution_command[cmd] = NULL ;
    }
}