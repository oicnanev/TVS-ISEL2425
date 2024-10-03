#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 1024
#define MAX_ARGS 100
#define MAX_PROCS 100

#define clear() printf("\033[H\033[J")  // Clearing the shell using escape sequences


// New aproach based on https://www.geeksforgeeks.org/making-linux-shell-c/
void init_bach() {
  // Greeting shell during startup
  clear();
  printf("\n BACH - Bach Alternative from Chelas \n");
  sleep(1);
  clear();
}

int takeInput(char *str) {
  char line[MAX_LINE];
  printf("$ ");
  
  fgets(line, MAX_LINE, stdin);
  
  if (strlen(line) != 0) {
    strcpy(str, line);
    return 0;
  }
  return 1;
}

void execArgs(char** parsed) {
  // Fork a child
  pid_t pid = fork();

  if (pid < 0) {
    printf("\nError forking!!!");
    return;
  } else if (pid == 0) {
    if (execvp(parsed[0], parsed) < 0) {
      printf("\nCould not execute command...\n");
      return;
    }
  } else {
    // wait for child to terminate
    wait(NULL);
    return;
  }
}

void execArgsPiped(char** parsed, char** parsedpipe) {
  // 0 is read end, 1 is write end, penso que das aulas é 3 e 4!!!!
  int pipefd[2];
  pid_t p1, p2;

  if (pipe(pipefd) < 0) {
    printf("\nCould not initialize the pipe!!!");
    return;
  }

  p1 = fork();
  if (p1 < 0) {
    printf("\nError forking!!!");
    return;
  } else if (p1 == 0) {
    // Child 1 executing...
    // It only needs to write at the write end
    close(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);

    if (execvp(parsed[0], parsed) < 0) {
      printf("\nCould not execute command 1..");
      exit(0);
    }
  } else {
    // Parent executing
    p2 = fork();
    if (p2 < 0) {
      printf("\nError forking!!!");
      return;
    }

    // Child 2 executing..
    // It only needs to read at the read end
    if (p2 == 0) {
      close(pipefd[1]);
      dup2(pipefd[0], STDIN_FILENO);
      close(pipefd[0]);

      if (execvp(parsedpipe[0], parsedpipe) < 0) {
        printf("\nCould not execute command 1..");
        exit(0);
      }
    } else {
      // parent executing, wait for the two children
      wait(NULL);
      wait(NULL);
    }
  }
}

void executeLs() {
  // TODO: https://iq.opengenus.org/ls-command-in-c/
  return;
}

void openHelp() {
  puts("\n***BACH - Bash Alternative from Chelas***"
        "\n"
        "\nList of Commands supported:"
        "\n>cd"
        "\n>ls"
        "\n>exit"
        "\n>all other general commands available in UNIX shell"
        "\n>pipe handling"
        "\n>improper space handling"
        "\n>output redirection");

    return;
}

int ownCmdHandler(char** parsed) {
  int numOfOwnCmds = 4, i, switchOwnArg = 0;
  char* listOfOwnCmds[numOfOwnCmds];

  listOfOwnCmds[0] = "exit";
  listOfOwnCmds[1] = "cd";
  listOfOwnCmds[2] = "help";
  listOfOwnCmds[3] = "ls";

  for (i = 0; i < numOfOwnCmds; i++) {
    if (strcmp(parsed[0], listOfOwnCmds[i]) == 0) {
      switchOwnArg = i + 1;
      break;
    }
  }

  switch (switchOwnArg){
  case 1:
    exit(EXIT_SUCCESS);
  case 2:
    chdir(parsed[1]);
    return 1;
  case 3:
    openHelp();
    return 1;
  case 4:
    executeLs(parsed[1]);
    return 1;
  }
  return 0;
}

int parsePipe(char* str, char** strpiped) {
  for (int i = 0; i < 2; i++) {
    strpiped[i] = strsep(&str, "|");
    if (strpiped[i] == NULL)
      break;
  }
  if (strpiped[1] == NULL)  // no pipe was found
    return 0;
  
  return 1;
}

void parseSpace(char* str, char** parsed) {
  for (int i = 0; i < MAX_PROCS; i++) {
    parsed[i] = strsep(&str, " ");

    if (parsed[i] == NULL)
      break;
    if (strlen(parsed[i]) == 0)  // to remove aditional spaces
      i--;
  }
}

int processString(char* str, char** parsed, char** parsedpipe) {
  char* strpiped[2];
  int piped = 0;

  piped = parsePipe(str, strpiped);

  if (piped) {
    parseSpace(strpiped[0], parsed);
    parseSpace(strpiped[1], parsedpipe);
  } else {
    parseSpace(str, parsed);
  }

  if (ownCmdHandler(parsed))
    return 0;
  else
    return 1 + piped;
}

int main() {
  char inputString[MAX_LINE], *parsedArgs[MAX_PROCS];
  char* parsedArgsPiped[MAX_PROCS];
  int execFlag = 0;
  init_bach();

  while(1) {
    // take input ------------------------------------------------------------
    if (takeInput(inputString)) {
      printf("we have input\n");
      continue;
    }
      

    // process ---------------------------------------------------------------
    // execFlag returns:
    // 0 if there is no command or it is a builtin command
    // 1 if it is a simple a command
    // 2 if it is including pipe
    execFlag = processString(inputString, parsedArgs, parsedArgsPiped);
    printf("execFlag: %d\n", execFlag);

    // execute ----------------------------------------------------------------
    if (execFlag == 1)
      execArgs(parsedArgs);
    if (execFlag == 2)
      execArgsPiped(parsedArgs, parsedArgsPiped);

    return EXIT_SUCCESS;
  }
}
