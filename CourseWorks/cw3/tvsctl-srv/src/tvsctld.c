#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>
#include <grp.h>    // For getgrnam
#include <sys/stat.h>

#define SOCKET_PATH "/run/isel/tvsctld/request"
#define BUFFER_SIZE 256
#define SOCKET_FD   3

// Function to execute the requested command by calling the appropriate script
void execute_command(const char *command) {
    fprintf(stderr, "Comand passed: %s\n", command);
    pid_t pid = fork();
    if (pid == 0) {
        // Child process: parse command and execute
        char command_copy[BUFFER_SIZE];
        strncpy(command_copy, command, sizeof(command_copy));
        command_copy[sizeof(command_copy) - 1] = '\0'; // Ensure null-termination

        // Extract the base command (first token)
        char *cmd = strtok(command_copy, " ");
        if (cmd == NULL) {
            fprintf(stderr, "Invalid command received\n");
            exit(EXIT_FAILURE);
        }

        // Build the script path based on the command
        char script_path[BUFFER_SIZE];
        snprintf(script_path, sizeof(script_path), "/opt/isel/tvs/tvsctld/bin/scripts/tvsapp-%s.sh", cmd);

        // Prepare arguments for /bin/bash execution
        char *args[BUFFER_SIZE];
        args[0] = "bash";       // Explicitly use Bash to run the script
        args[1] = script_path;  // Pass the script path as the first argument
        int i = 2;

        // Collect all additional arguments
        char *arg = strtok(NULL, " ");
        while (arg != NULL && i < BUFFER_SIZE - 1) {
            args[i++] = arg;
            arg = strtok(NULL, " ");
        }
        args[i] = NULL; // Null-terminate the argument list

        // Execute the script with arguments
        execvp("/bin/bash", args);

        // If execvp fails, print an error and exit
        perror("execvp failed");
        // Debug: Print the arguments
        fprintf(stderr, "Executing script: %s\n", script_path);
        for (int j = 0; args[j] != NULL; j++) {
            fprintf(stderr, "arg_%d: %s\n", j, args[j]);
        }
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process: wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);
    } else {
        // Fork failed
        perror("fork failed");
    }
}



int main() {
    int client_sock;
    char buffer[BUFFER_SIZE];

    // Main loop to handle incoming commands
    while (1) {
        if ((client_sock = accept(SOCKET_FD, NULL, NULL)) == -1) {
            perror("Socket accept failed");
            continue;
        }

        ssize_t num_bytes = read(client_sock, buffer, BUFFER_SIZE - 1);
        if (num_bytes > 0) {
            buffer[num_bytes] = '\0';
            execute_command(buffer);
        } else {
            perror("Failed to read from client socket");
        }

        close(client_sock);
    }

    unlink(SOCKET_PATH);
    return 0;
}
