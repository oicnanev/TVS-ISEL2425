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

// Function to execute the requested command by calling the appropriate script
void execute_command(const char *command) {
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
    int main() {
    struct sockaddr_un server_addr;
    int server_sock, client_sock;
    char buffer[BUFFER_SIZE];

    // Ensure the parent directory of the socket exists
    const char *socket_dir = "/run/isel/tvsctld";
    struct group *grp = getgrnam("tvsgrp");   // to get tvsgrp group
    if (grp == NULL) {
        perror("Group 'tvsgrp' does not exist");
        exit(EXIT_FAILURE);
    }

    if (access(socket_dir, F_OK) == -1) { // Check if the directory exists
        if (mkdir(socket_dir, 0770) == -1) { // Create directory with appropriate permissions
            perror("Failed to create socket directory");
            exit(EXIT_FAILURE);
        }
        // Set ownership to root:tvsgrp
        if (chown(socket_dir, 0, grp->gr_gid) == -1) {  // user:root (0), group:tvsgrp
            perror("Failed to set ownership of socket directory");
            exit(EXIT_FAILURE);
        }
    }

    // Create a Unix domain socket
    if ((server_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure the socket path and bind it
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    unlink(SOCKET_PATH); // Ensure no stale socket exists

    if (bind(server_sock, (struct sockaddr *)&server_addr,
             sizeof(struct sockaddr_un)) == -1) {
        perror("Socket binding failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sock, 5) == -1) {
        perror("Socket listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Daemon running and waiting for commands...\n");

    // Main loop to handle incoming commands
    while (1) {
        if ((client_sock = accept(server_sock, NULL, NULL)) == -1) {
            perror("Socket accept failed");
            continue;
        }

        ssize_t num_bytes = read(client_sock, buffer, BUFFER_SIZE - 1);
        if (num_bytes > 0) {
            buffer[num_bytes] = '\0';
            // Call to your execute_command function here
        } else {
            perror("Failed to read from client socket");
        }

        close(client_sock);
    }

    close(server_sock);
    unlink(SOCKET_PATH);
    return 0;
}
