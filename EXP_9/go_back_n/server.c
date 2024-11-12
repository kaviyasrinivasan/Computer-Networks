#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 8080
#define WINDOW_SIZE 4

int socket_fd;
struct sockaddr_in server_addr, client_addr;
int expected_seq_no = 0;

void send_ack(int ack_no) {
    char ack_buffer[sizeof(int)];
    memcpy(ack_buffer, &ack_no, sizeof(int));
    sendto(socket_fd, ack_buffer, sizeof(ack_buffer), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
    printf("Acknowledgment for packet %d sent\n", ack_no);
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    char buffer[256];
    int client_len = sizeof(client_addr);

    while (1) {
        int recv_len = recvfrom(socket_fd, buffer, 256, 0, (struct sockaddr*)&client_addr, &client_len);
        buffer[recv_len] = '\0';

        int seq_no;
        sscanf(buffer, "Packet %d", &seq_no);
        printf("Received packet %d\n", seq_no);

        if (seq_no == expected_seq_no) {
            printf("Packet %d received in order\n", seq_no);
            send_ack(seq_no);
            expected_seq_no++;
        } else {
            printf("Out-of-order packet %d received, expected %d\n", seq_no, expected_seq_no);
            send_ack(expected_seq_no - 1); // Resend last ACK for the expected sequence number
        }
    }

    closesocket(socket_fd);
    WSACleanup();
    return 0;
}