#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/wait.h>

#define SOCKET_PATH "/run/isel/tvsctld/request"
#define BUFFER_SIZE 256

// Function to execute the requested command by calling the appropriate script
void execute_command(const char *command) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process: execute the script
        char command_copy[BUFFER_SIZE];
        strncpy(command_copy, command, sizeof(command_copy));
        command_copy[sizeof(command_copy) - 1] = '\0'; // Ensure null-termination

        // Extract the command and arguments
        char *cmd = strtok(command_copy, " ");
        if (cmd == NULL) {
            fprintf(stderr, "Invalid command received\n");
            exit(EXIT_FAILURE);
        }

        char script_path[BUFFER_SIZE];
        snprintf(script_path, sizeof(script_path), "/opt/isel/tvs/tvsctld/bin/scripts/tvsapp-%s.sh", cmd);

        char *args[BUFFER_SIZE];
        args[0] = "bash";
        args[1] = script_path;
        int i = 2;
        char *arg = strtok(NULL, " ");
        while (arg != NULL && i < BUFFER_SIZE - 1) {
            args[i++] = arg;
            arg = strtok(NULL, " ");
        }
        args[i] = NULL;

        // DEBUG ------------- (delete please) --------------------
        fprintf(stderr, "DEBUG - args for execvp:\n");
        for (int i = 0; args[i] != NULL; i++) {
            fprintf(stderr, "  args[%d]: %s\n", i, args[i]);
            fflush(stderr);  // Force flush
        }
        if (access(script_path, X_OK) == -1) {
            perror("Script does not exist or is not executable");
            fprintf(stderr, "Script path: %s\n", script_path);
            exit(EXIT_FAILURE);
        } // ------------------------------------------------------

        execvp("/bin/bash", args);

        // If execvp fails
        perror("execvp failed");
        fprintf(stderr, "Script: %s\n", script_path);
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("fork failed");
    }
}

int main() {
    int client_sock;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];

    // Create a Unix domain socket to connect to the existing one
    // A new socket is created only to connect to the pre-existing one using connect(). 
    // No bind() or listen() is performed in this program.
    client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sock == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // Connect to the existing socket
    if (connect(client_sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("Socket connection failed. Ensure tvsctld.socket is running.");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    printf("Connected to socket at %s. Waiting for commands...\n", SOCKET_PATH);

    // Main loop to handle incoming commands
    while (1) {
        ssize_t num_bytes = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
        if (num_bytes > 0) {
            buffer[num_bytes] = '\0';
            fprintf(stderr, "DEBUG: command sent: %s", buffer);
            fflush(stderr);  // Force flush
            execute_command(buffer);
        } else if (num_bytes == 0) {
            fprintf(stderr, "Connection closed by server.\n");
            break;
        } else {
            perror("Failed to read from server socket");
            break;
        }
    }

    close(client_sock);
    return 0;
}
