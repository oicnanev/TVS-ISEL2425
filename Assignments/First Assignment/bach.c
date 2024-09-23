#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

void execute_command(char *args[]);
void parse_and_execute(char *line);
void handle_redirection(char *cmd, int *fd_out);
void handle_pipes(char *cmd);

int main() {
    // Main loop to read user input
    char line[MAX_LINE];
    
    while (1) {
        printf("$ ");
        if (fgets(line, MAX_LINE, stdin) == NULL){
            break;
        };
        parse_and_execute(line);
    }
    return 0;
}

void parse_and_execute(char *line) {
    // Splits the input by pipes and handles redirection
    char *cmd = strtok(line, "|"); // Split line by pipes
    int fd_out = STDOUT_FILENO;  // Default output is stdout
    
    while (cmd != NULL) {
        handle_redirection(cmd, &fd_out);
        cmd = strtok(NULL, "|");  // Get next command
    }
}

void handle_redirection(char *cmd, int *fd_out) {
    // If it founds a '>', it redirects the output to the specific file
    char *redir = strchr(cmd, '>');  // Find redirection
    
    if (redir != NULL) {
        *redir = '\0';
        redir++;
        while (*redir == ' ') {
            redir++;
        }
        *fd_out =  open(redir, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    }

    handle_pipes(cmd);
}

void handle_pipes(char *cmd) {
    // splits the command into arguments and executes it
    // TODO: falta ls, 
    char *args[MAX_ARGS];
    char *arg = strtok(cmd, " ");
    int i = 0;
    
    while (arg != NULL) {
        args[i++] = arg;
        arg = strtok(NULL, " ");
    }
    
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL || chdir(args[1]) != 0) {
            perror("cd");
        }
        else if (strcmp(args[0], "exit") == 0) {
            exit(0);
        }
        else {
            execute_command(args);
        }
    }
}

void execute_command(char *args[]) {
    // Uses fork and execvp to execute the command in a child process
    pid_t pid = fork();
    
    if (pid == 0) {
        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else if (pid < 0) {
        perror("fork");
    }
    else {
        wait(NULL);
    }
}

