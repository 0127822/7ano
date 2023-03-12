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
char SETUP_PATH[1024];


void getCommand();
void parseCommandToExecute();
void createChildProcessToExecuteCommand();
void simpleShell();
void changeDirectoryExecution();
void commandsHandlers();
void clearExecutionCommandArray();
void loggingHandler();
void backgroundExecutionParsingHandler();
void setupEnvironment();
void exitSetup();
void exitSignalHandler(int sig);


int main(){
    exitSetup();
    signal(SIGCHLD , loggingHandler);
    setupEnvironment();
    simpleShell();

    return 0;

}

void simpleShell(){

    while(1){

        getCommand();
        parseCommandToExecute();
        if(!strcmp(execution_command[0] , "exit")) {
            kill(0, SIGTERM);
            break;
        }
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
        //parse for get the exported variables
        if (parsed[0] == '$') {
            char* value = getenv(parsed + 1);
            if (value != NULL) {
                execution_command[NUM_OF_ARG] = value;
            } else {
                execution_command[NUM_OF_ARG] = parsed;
            }
        }else {
            execution_command[NUM_OF_ARG] = parsed;
        }
        NUM_OF_ARG++;
        parsed = strtok(NULL  ,  delimiter);
    }
    backgroundExecutionParsingHandler();
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
        if(execution_command[NUM_OF_ARG] == NULL) waitpid(child , NULL , 0);
    }

    clearExecutionCommandArray();

}

void changeDirectoryExecution() {
        if (NUM_OF_ARG <2) {
            chdir(SETUP_PATH);
        } else if(!strcmp(execution_command[1], "~")){
            chdir("/home");
        } else if (chdir( execution_command[1]) != 0){
            perror("cd");
        }
}

void commandsHandlers(){

    if (!strcmp(execution_command[0], "cd")) {
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
        printf("%s: command not found\n" , execution_command[0]);
        exit(EXIT_FAILURE);
    }
}

void clearExecutionCommandArray(){
    for (int cmd = 0 ; cmd <NUM_OF_ARG ; cmd++ ){
        execution_command[cmd] = NULL ;
    }
}

void loggingHandler(){
    //zombie handling
    FILE *file;
    file = fopen("log.txt","a");
    if(file == NULL)perror("Error create file");
    else fprintf(file , "[LOGS]: child proccess terminated. \n");
    fclose(file);
}

void backgroundExecutionParsingHandler(){
    if(!strcmp(execution_command[NUM_OF_ARG-1], "&")){
        execution_command[NUM_OF_ARG-1] = NULL;
        execution_command[NUM_OF_ARG]= "&";
    } else{
        execution_command[NUM_OF_ARG]= NULL;
    }
}

void setupEnvironment(){
 getcwd(SETUP_PATH , sizeof(SETUP_PATH));
}

void exitSetup(){
    struct sigaction exit_signal;
    exit_signal.sa_handler = exitSignalHandler;
    sigemptyset(&exit_signal.sa_mask);
    exit_signal.sa_flags = 0;
    if (sigaction(SIGTERM, &exit_signal, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
}

void exitSignalHandler(int sig)
{
    if (sig == SIGTERM) {
        exit(EXIT_SUCCESS);
    }
}
