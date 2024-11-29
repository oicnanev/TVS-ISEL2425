#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h> // for O_RDONLY

#define SOCKET_PATH "/run/isel/tvsctld/request"
#define BUFFER_SIZE 256

void usage() {
    const char srt[] = "Start elasticsearch, all web apps instances and reloads nginx";
    const char rst[] = "Force an initial stopped configuration with scale (default=1) instances in consecutive ports starting at base (default=35000)";
    const char stp[] = "Deactivate the site from nginx and stop web app instances. If -db, also stop elasticsearch.";
    const char stt[] = "Prints a summary status of nginx, web apps and db. If -v, prints a verbose status";
    const char inc[] = "Add delta (default=1) instances of Node.js running the web application, using more consecutive ports";
    const char dec[] = "Remove delta (default=1) instances of Node.js with the highest ports in use, leaving at least 1";
    
    printf("Usage: tvsctl <command> [args]\n");
    printf("\nAvailable commads\n\n");
    printf("\tstart\t\t\t%s\n", srt);
    printf("\tstatus [-v]\t\t%s\n", stt);
    printf("\tstop [-db]\t\t%s\n", stp);
    printf("\treset [scale [base]]\t%s\n", rst);
    printf("\tinc [delta]\t\t%s\n", inc);
    printf("\tdec [delta]\t\t%s\n", dec);
}


int main(int argc, const char * argv[]) {
	// These must always be the first instructions. DON'T EDIT
	close(0); open("/dev/null", O_RDONLY);       // DON'T EDIT

	int sock;
    struct sockaddr_un server_addr;

    // Ensure there is at least one argument for the command
    if (argc < 2 || (argc > 2 && 
            (strcmp(argv[2], "--help") == 0 || strcmp(argv[2], "--h") == 0))) {
        usage();
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
