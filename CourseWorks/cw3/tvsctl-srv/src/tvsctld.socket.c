#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <grp.h>    // For getgrnam
#include <sys/stat.h>

#define SOCKET_PATH "/run/isel/tvsctld/request"
#define BUFFER_SIZE 256


int main() {
    struct sockaddr_un server_addr;
    int server_sock, client_sock;
    char buffer[BUFFER_SIZE];

    // Ensure the parent directory of the socket exists
    const char *socket_dir = "/run/isel/tvsctld";
    const char *isel = "/run/isel/";
    
    struct group *grp = getgrnam("tvsgrp");   // to get tvsgrp group
    if (grp == NULL) {
        perror("Group 'tvsgrp' does not exist");
        exit(EXIT_FAILURE);
    }

    if (access(socket_dir, F_OK) == -1) { // Check if the directory exists
        umask(0);
        if (mkdir(isel, 0777) == -1)  { // Create directory with appropriate permissions
            perror("Failed to create isel socket directory");
            exit(EXIT_FAILURE);
        }
        if (mkdir(socket_dir, 0777) == -1)  { // Create directory with appropriate permissions
            perror("Failed to create socket directory");
            exit(EXIT_FAILURE);
        }
        // Set ownership to root:tvsgrp
        if (chown(isel, 0, grp->gr_gid) == -1) {  // user:root (0), group:tvsgrp
            perror("Failed to set ownership of socket directory");
            exit(EXIT_FAILURE);
        }
        if (chown(socket_dir, 0, grp->gr_gid) == -1) {  // user:root (0), group:tvsgrp
            perror("Failed to set ownership of socket directory");
            exit(EXIT_FAILURE);
        }
        umask(0022);
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

    char *socketp = "/run/isel/tvsctld/request";
    if (chmod(socketp, 0666) == -1) {
        perror("Failed to set permissions of socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sock, 5) == -1) {
        perror("Socket listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Daemon running and waiting for commands...\n");

    return 0;
}
