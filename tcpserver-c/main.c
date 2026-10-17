// POSIX TCP server for Linux, macOS, and WSL.
// Windows users should build main-windows.c with w64devkit instead.

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define HOST "127.0.0.1"
#define PORT 8801
#define BACKLOG 5
#define BUFFER_SIZE 1024

int main(void) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }

    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("setsockopt");
        close(server_fd);
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, HOST, &server_addr.sin_addr) != 1) {
        fprintf(stderr, "Invalid bind address: %s\n", HOST);
        close(server_fd);
        return EXIT_FAILURE;
    }

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        return EXIT_FAILURE;
    }

    if (listen(server_fd, BACKLOG) == -1) {
        perror("listen");
        close(server_fd);
        return EXIT_FAILURE;
    }

    printf("TCP server listening on %s:%d\n", HOST, PORT);
    printf("Press Ctrl+C to stop the server.\n");

    for (;;) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

        if (client_fd == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("accept");
            break;
        }

        char client_ip[INET_ADDRSTRLEN] = "unknown";
        (void)inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("Client connected: %s:%u\n", client_ip, (unsigned int)ntohs(client_addr.sin_port));

        const char greeting[] = "Hello from C TCP server!\n";
        if (send(client_fd, greeting, sizeof(greeting) - 1, 0) == -1) {
            perror("send");
        }

        char buffer[BUFFER_SIZE];
        ssize_t received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (received > 0) {
            printf("Received: ");
            (void)fwrite(buffer, 1, (size_t)received, stdout);
            if (buffer[received - 1] != '\n') {
                putchar('\n');
            }
        } else if (received == 0) {
            printf("Client disconnected without sending data.\n");
        } else {
            perror("recv");
        }

        close(client_fd);
    }

    close(server_fd);
    return EXIT_SUCCESS;
}
