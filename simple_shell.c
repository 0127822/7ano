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


void getCommand();
void parseCommandToExecute();
void createChildProcessToExecuteCommand();
void simpleShell();
void changeDirectoryExecution();



int main(){
    simpleShell();
    return 0;

}

void simpleShell(){

    while(1){

        getCommand();
        parseCommandToExecute();
        if(!strcmp(execution_command[0] , "exit")) break;
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
//ls space error
        execution_command[NUM_OF_ARG] = parsed;
        NUM_OF_ARG++;
        parsed = strtok(NULL  ,  delimiter);
    }


}

void createChildProcessToExecuteCommand() {
    pid_t child = fork();

    if (child == 0) {
        if (!strcmp(execution_command[0], "cd")) {
            changeDirectoryExecution();
            printf("return");
        } else {
            printf("here");
            execvp(execution_command[0], execution_command);
            perror("execvp");
            exit(EXIT_FAILURE);

        }
    }

    else if (child == -1){
        printf("Can not create this Child \n");
    }

    else{

        if(execution_command[NUM_OF_ARG] == NULL)waitpid(child , NULL , 0);

        for (int cmd = 0 ; cmd <NUM_OF_ARG ; cmd++ ){
            execution_command[cmd] = NULL ;
        }
    }

}

void changeDirectoryExecution() {
//if cd go home
        if (NUM_OF_ARG <2) {
            fprintf(stderr, "cd: missing argument\n");
            return;
        } else if(!strcmp(execution_command[1], "~")){
            chdir("/home");
        } else if (chdir( execution_command[1]) != 0){
            perror("cd");
        }
}