// POSIX UDP server for Linux, macOS, and WSL.
// Windows users should build main-windows.c with a Winsock-capable toolchain.

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define HOST "127.0.0.1"
#define PORT 5501
#define BUFFER_SIZE 1024

int main(void) {
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd == -1) {
        perror("socket");
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

    printf("UDP server listening on %s:%d; waiting for one datagram...\n", HOST, PORT);

    struct sockaddr_in client_addr;
    socklen_t client_len = (socklen_t)sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Reserve one byte so the received data can be printed as a C string safely.
    ssize_t received = recvfrom(
        server_fd,
        buffer,
        sizeof(buffer) - 1,
        0,
        (struct sockaddr *)&client_addr,
        &client_len
    );

    if (received == -1) {
        perror("recvfrom");
        close(server_fd);
        return EXIT_FAILURE;
    }

    buffer[received] = '\0';

    char client_ip[INET_ADDRSTRLEN] = "unknown";
    (void)inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    printf(
        "Received %zd byte(s) from %s:%u: %s\n",
        received,
        client_ip,
        (unsigned int)ntohs(client_addr.sin_port),
        buffer
    );

    close(server_fd);
    return EXIT_SUCCESS;
}
