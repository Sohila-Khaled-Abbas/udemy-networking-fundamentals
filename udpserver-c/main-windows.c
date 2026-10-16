/*
 * Native Windows UDP server using Winsock.
 *
 * Build in the w64devkit terminal:
 *   gcc main-windows.c -o udpserver-windows.exe -lws2_32
 *
 * Listens on 127.0.0.1:5501, receives one datagram, prints it, and exits.
 */

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    WSADATA wsaData;
    SOCKET sockfd = INVALID_SOCKET;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    char buffer[1024];
    int clientAddrLen = (int)sizeof(clientAddr);
    int received;
    int result = 1;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
        return 1;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == INVALID_SOCKET) {
        fprintf(stderr, "socket() failed with error %d.\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5501);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(sockfd, (struct sockaddr *)&serverAddr, (int)sizeof(serverAddr)) == SOCKET_ERROR) {
        fprintf(stderr, "bind() failed with error %d.\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    printf("Windows UDP server listening on 127.0.0.1:5501; waiting for one datagram...\n");

    received = recvfrom(
        sockfd,
        buffer,
        (int)sizeof(buffer) - 1,
        0,
        (struct sockaddr *)&clientAddr,
        &clientAddrLen
    );

    if (received == SOCKET_ERROR) {
        fprintf(stderr, "recvfrom() failed with error %d.\n", WSAGetLastError());
    } else {
        buffer[received] = '\0';
        printf(
            "Received %d byte(s) from %s:%u: %s\n",
            received,
            inet_ntoa(clientAddr.sin_addr),
            (unsigned int)ntohs(clientAddr.sin_port),
            buffer
        );
        result = 0;
    }

    closesocket(sockfd);
    WSACleanup();
    return result;
}
