#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/run/isel/tvsctld/request"
#define BUFFER_SIZE 256

// Function to execute the requested command by calling the appropriate script
void execute_command(const char *command) {
  	pid_t pid = fork();
    if (pid == 0) {
        // Child process: execute the command
        char script_path[BUFFER_SIZE];
        snprintf(script_path, sizeof(script_path), "/opt/isel/tvs/scripts/%s.sh", command);

        execlp(script_path, script_path, (char *)NULL);
        perror("execlp failed"); // This only executes if execlp fails
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process: wait for child to finish
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("fork failed");
    }
}


int main() {
	struct sockaddr_un server_addr;
  	int server_sock, client_sock;
  	char buffer[BUFFER_SIZE];

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
    	// Accept a client connection
    	if ((client_sock = accept(server_sock, NULL, NULL)) == -1) {
      		perror("Socket accept failed");
      		continue;
    	}

    	// Read the command from the client
    	ssize_t num_bytes = read(client_sock, buffer, BUFFER_SIZE - 1);
    	if (num_bytes > 0) {
      		buffer[num_bytes] = '\0'; // Null-terminate the string
      		execute_command(buffer);  // Execute the requested command
    	} else {
      		perror("Failed to read from client socket");
    	}

    	close(client_sock); // Close the client connection
  	}

  	close(server_sock);
  	unlink(SOCKET_PATH); // Clean up socket file
  	return 0;
}
