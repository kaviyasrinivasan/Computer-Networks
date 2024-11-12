
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>

#define PORT 8080
#define WINDOW_SIZE 4
#define TIMEOUT 5

int socket_fd;
struct sockaddr_in server_addr;
int last_ack_received = -1;
int base = 0;

void send_packet(int seq_no) {
    char packet[256];
    sprintf(packet, "Packet %d", seq_no);
    sendto(socket_fd, packet, strlen(packet), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("Packet %d sent\n", seq_no);
}

void resend_window() {
    printf("Timeout, resending window\n");
    for (int i = base; i < base + WINDOW_SIZE; i++) {
        send_packet(i);
    }
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int total_packets = 10; 
    int ack;
    struct timeval timeout = {TIMEOUT, 0}; 
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

    while (last_ack_received < total_packets - 1) {
        while (base < total_packets && base < last_ack_received + WINDOW_SIZE + 1) {
            send_packet(base);
            base++;
        }

        char ack_buffer[sizeof(int)];
        int server_len = sizeof(server_addr);
        int recv_len = recvfrom(socket_fd, ack_buffer, sizeof(ack_buffer), 0, (struct sockaddr*)&server_addr, &server_len);

        if (recv_len > 0) {
            memcpy(&ack, ack_buffer, sizeof(int));
            printf("Acknowledgment for packet %d received\n", ack);
            if (ack >= last_ack_received + 1) {
                last_ack_received = ack;
            }
        } else {
            resend_window(); 
            base = last_ack_received + 1;
        }
    }

    closesocket(socket_fd);
    WSACleanup();
    return 0;
}