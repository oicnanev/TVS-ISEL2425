#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>

#define MAX_LINE 131072  // In Linux usaually the maximum size of a command is 128KB = 131072 bytes
#define MAX_ARGS 100
#define MAX_PROCS 10  


void process_args_and_execute_cmd(char *cmd) {
    char *args[MAX_ARGS];
    int arg_count = 0;
    
    // Separa os argumentos pelo caractere de espaço
    char *token = strtok(cmd, " ");
    while (token != NULL && arg_count < MAX_ARGS) {
        // Remove espaços em branco do início e do fim
        while (isspace((unsigned char)*token))
            token++;
        
        char *end = token + strlen(token) - 1;
        while (end > token && isspace((unsigned char)*end))
            end--;
        end[1] = '\0';
    
        args[arg_count++] = strdup(token); // Armazena o argumento
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL;  // Último argumento deve ser NULL
  
    // verificar se o comando é cd ou exit
    if (arg_count > 0) {
        if (strcmp(args[0], "cd") == 0) {
            if (arg_count != 2) {
                fprintf(stderr, "Usage: cd <dir>\n");
            } else {
                if (chdir(args[1]) != 0) {
                    perror("chdir");
                }
            }
        } else if (strcmp(args[0], "exit") == 0) {
            for (int i = 0; i < arg_count; i++) {
                free(args[i]);
            }
            exit(EXIT_SUCCESS);
        } else {
            // ver se ultimo argumento é '>'
            if (arg_count >= 3 && strcmp(args[arg_count - 2], ">") == 0) {
                // Redirecionamento de output para ficheiro
                int fd = open(args[arg_count - 1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fd == -1) {
                    perror("open");
                } else {
                    // Substitui o file descriptor do stdout pelo fd
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    // Remove o último argumento
                    free(args[arg_count - 1]);
                    args[arg_count - 1] = NULL;
                    free(args[arg_count - 2]);
                    args[arg_count - 2] = NULL;
                }
            }
            // Executa o comando
            execvp(args[0], args);
            // Se execvp retornar, então ocorreu um erro
            perror("execvp");
        }
    }
    
    // Libertar a memória alocada
    for (int i = 0; i < arg_count; i++) {
        free(args[i]);
    }
    return;
}

void process_pipes(char *cmd[], int counter) {
    pid_t pid;
    if (counter == 1) {
        pid = fork();
        // Não há pipes
        if (pid == 0){
            process_args_and_execute_cmd(cmd[0]);
        } else if (pid < 0){
            perror ("fork");
            exit(EXIT_FAILURE);
        }
        return;
    }

    // Processamento de pipes
    int pipefd[2 * (counter - 1)];
    int i;

    // Cria os pipes necessários
    for (i = 0; i < counter - 1; i++) {
        if (pipe(pipefd + i * 2) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < counter; i++) {
        pid = fork();
        
        if (pid == 0) {
            // Processo filho
            if (i != 0) {
                // Redireciona a entrada padrão para o pipe anterior
                if (dup2(pipefd[(i - 1) * 2], STDIN_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }
            if (i != counter - 1) {
                // Redireciona a saída padrão para o próximo pipe
                if (dup2(pipefd[i * 2 + 1], STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // Fecha todos os pipes no processo filho
            for (int j = 0; j < 2 * (counter - 1); j++) {
                close(pipefd[j]);
            }

            // Processa os argumentos e executa o comando
            process_args_and_execute_cmd(cmd[i]);
            return;
        } else if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    // Fecha todos os pipes no processo pai
    for (i = 0; i < 2 * (counter - 1); i++) {
        close(pipefd[i]);
    }

    // Espera todos os processos filhos terminarem
    for (i = 0; i < counter; i++) {
        int status;
        wait(&status);
    }
}


void process_string(char *line){
    char *commands[MAX_PROCS];
    // Remove o caractere de nova linha, se presente
    line[strcspn(line, "\n")] = 0;
    int command_count = 0;

    // Separa os comandos pelo caractere '|'
    char *token = strtok(line, "|");

    while (token != NULL && command_count < MAX_PROCS) {
        // Remove espaços em branco do início e do fim
        while (isspace((unsigned char)*token))
            token++;

        char *end = token + strlen(token) - 1;
        while (end > token && isspace((unsigned char)*end))
            end--;

        end[1] = '\0';

        commands[command_count++] = strdup(token); // Armazena o comando
        token = strtok(NULL, "|");
    }

    process_pipes(commands, command_count);

    // libertar a memoria alocada pelo strdup
    for (int i = 0; i < command_count; i++) {
        free(commands[i]);
    }
}


int main(int argc, char *argv[]) {
    char line [MAX_LINE];
    while (1) {
        printf("$ ");
        // Lê a linha de entrada, stdin
        if (fgets(line, MAX_LINE, stdin) == NULL)
            break;

        process_string(line);  // parse dos comandos introduzidos pelo user
    }
    return EXIT_SUCCESS;  // = return 0
}
