// Native Windows TCP server using Winsock.
// Build with: gcc main-windows.c -o tcpserver-windows.exe -lws2_32

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HOST "127.0.0.1"
#define PORT 8801
#define BACKLOG 5
#define BUFFER_SIZE 1024

int main(void) {
    WSADATA wsa_data;
    int startup_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (startup_result != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", startup_result);
        return EXIT_FAILURE;
    }

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        fprintf(stderr, "socket failed: %d\n", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(HOST);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        fprintf(stderr, "bind failed: %d (is port %d already in use?)\n", WSAGetLastError(), PORT);
        closesocket(server_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    if (listen(server_socket, BACKLOG) == SOCKET_ERROR) {
        fprintf(stderr, "listen failed: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("TCP server listening on %s:%d\n", HOST, PORT);
    printf("Press Ctrl+C to stop the server.\n");

    for (;;) {
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);
        SOCKET client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);

        if (client_socket == INVALID_SOCKET) {
            fprintf(stderr, "accept failed: %d\n", WSAGetLastError());
            break;
        }

        printf("Client connected: %s:%u\n",
               inet_ntoa(client_addr.sin_addr),
               (unsigned int)ntohs(client_addr.sin_port));

        const char greeting[] = "Hello from C TCP server!\r\n";
        if (send(client_socket, greeting, (int)(sizeof(greeting) - 1), 0) == SOCKET_ERROR) {
            fprintf(stderr, "send failed: %d\n", WSAGetLastError());
        }

        char buffer[BUFFER_SIZE];
        int received = recv(client_socket, buffer, (int)sizeof(buffer), 0);
        if (received > 0) {
            printf("Received: ");
            (void)fwrite(buffer, 1, (size_t)received, stdout);
            if (buffer[received - 1] != '\n') {
                putchar('\n');
            }
        } else if (received == 0) {
            printf("Client disconnected without sending data.\n");
        } else {
            fprintf(stderr, "recv failed: %d\n", WSAGetLastError());
        }

        shutdown(client_socket, SD_BOTH);
        closesocket(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();
    return EXIT_SUCCESS;
}
