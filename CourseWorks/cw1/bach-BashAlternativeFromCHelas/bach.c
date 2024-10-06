#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// In Linux usaually the maximum size of a command is 128KB = 131072 bytes
#define MAX_LINE 131072
#define MAX_ARGS 100
#define MAX_PROCS 10

void remove_whitespaces(char *str) {
  // Remove espaços em branco do início
  while (isspace((unsigned char)*str))
    str++;

  // Remove espaços em branco do fim
  char *end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end))
    end--;

  end[1] = '\0';
}

void free_memory(char *arr[], int arr_count) {
  for (int i = 0; i < arr_count; i++) {
    free(arr[i]);
  }
}

void process_args_and_execute_cmd(char *cmd) {
  char *args[MAX_ARGS];
  int arg_count = 0;

  // Separa os argumentos pelo caractere de espaço
  char *token = strtok(cmd, " ");
  while (token != NULL && arg_count < MAX_ARGS) {
    remove_whitespaces(token);
    args[arg_count++] = strdup(token); // Armazena o argumento
    token = strtok(NULL, " ");
  }
  args[arg_count] = NULL; // Último argumento deve ser NULL

  // verificar se existe comando e argumentos
  if (arg_count > 0) {
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

  // Libertar a memória alocada se não executar o exec
  free_memory(args, arg_count);
}

int is_cd(char *cmd) { return (strstr(cmd, "cd ") != NULL); }

int is_exit(char *cmd) { return (strcmp(cmd, "exit") == 0); }

void execute_cd(char *cmd) {
  char *arg = strtok(cmd + 3, " "); // cd so tem um argumento
  remove_whitespaces(arg);
  // change the directory
  if (arg == NULL) {
    fprintf(stderr, "cd: missing arguments\n");
    return;
  }
  if (chdir(arg) != 0) {
    perror("chdir"); // printa o erro se a directoria for invalida
  }
}

void process_pipes(char *cmd[], int counter) {
  pid_t pid;

  // Se nao houver pipes
  if (counter == 1) {
    if (is_cd(cmd[0])) {
      execute_cd(cmd[0]);
      return;
    } else if (is_exit(cmd[0])) {
      exit(EXIT_SUCCESS);
    }

    pid = fork();

    if (pid == 0) {
      process_args_and_execute_cmd(cmd[0]);
    } else if (pid < 0) {
      perror("fork");
      exit(EXIT_FAILURE);
    } else {
      int status;
      waitpid(pid, &status, 0);
      // if (WIFEXITED(status))
      // printf("Child exited with status %d\n", WEXITSTATUS(status));
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

      // Se execvp retornar, então ocorreu um erro
      printf("DEBUG: error in execvp executing the cmd[%d] = %s\n", i, cmd[i]);
      exit(EXIT_SUCCESS);

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
    waitpid(-1, &status,
            0); // -1 espera por qualquer processo filho, 0 comportamento padrão
  }
}

void process_string(char *line) {
  char *commands[MAX_PROCS];
  // Remove o caractere de nova linha, se presente
  line[strcspn(line, "\n")] = 0;
  int command_count = 0;

  // Separa os comandos pelo caractere '|'
  char *token = strtok(line, "|");

  while (token != NULL && command_count < MAX_PROCS) {
    remove_whitespaces(token);
    commands[command_count++] = strdup(token); // Armazena o comando
    token = strtok(NULL, "|");
  }

  process_pipes(commands, command_count);

  // libertar a memoria alocada pelo strdup
  free_memory(commands, command_count);
}

int main(int argc, char *argv[]) {
  char line[MAX_LINE];
  while (1) {
    printf("$ ");
    // Lê a linha de entrada, stdin
    if (fgets(line, MAX_LINE, stdin) == NULL)
      break;

    process_string(line); // parse dos comandos introduzidos pelo user
  }
  return EXIT_SUCCESS; // = return 0
}
