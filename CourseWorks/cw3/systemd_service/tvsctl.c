#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/run/isel/tvsctld/request"
#define BUFFER_SIZE 256

int main(int argc, char *argv[]) {
  int sock;
  struct sockaddr_un server_addr;

  // Ensure there is at least one argument for the command
  if (argc < 2) {
    fprintf(stderr, "Usage: tvsctl <command> [args]\n");
    exit(EXIT_FAILURE);
  }

  // Build the command from arguments
  char command[BUFFER_SIZE] = {0};
  for (int i = 1; i < argc; i++) {
    strcat(command, argv[i]);
    if (i < argc - 1) {
      strcat(command, " ");
    }
  }

  // Create a Unix domain socket
  if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  // Set up the server address structure
  memset(&server_addr, 0, sizeof(struct sockaddr_un));
  server_addr.sun_family = AF_UNIX;
  strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

  // Connect to the server socket
  if (connect(sock, (struct sockaddr *)&server_addr,
              sizeof(struct sockaddr_un)) == -1) {
    perror("Socket connect failed");
    close(sock);
    exit(EXIT_FAILURE);
  }

  // Send the command to the daemon
  if (write(sock, command, strlen(command)) == -1) {
    perror("Failed to send command");
    close(sock);
    exit(EXIT_FAILURE);
  }

  close(sock); // Close the socket connection
  return 0;
}
