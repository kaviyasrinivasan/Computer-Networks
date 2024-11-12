#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 8080
#define BUFFER_SIZE 256

int socket_fd;
struct sockaddr_in server_addr, client_addr;

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        char buffer[BUFFER_SIZE];
        int addr_len = sizeof(client_addr);
        int recv_len = recvfrom(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addr_len);

        if (recv_len > 0) {
            printf("Server received: %s\n", buffer);
            int ack = atoi(&buffer[7]); 
            sendto(socket_fd, (char*)&ack, sizeof(ack), 0, (struct sockaddr*)&client_addr, addr_len);
            printf("Acknowledgment for Packet %d sent\n", ack);
        }
    }

    closesocket(socket_fd);
    WSACleanup();
    return 0;
}
