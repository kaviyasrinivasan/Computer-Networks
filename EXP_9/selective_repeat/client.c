#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>

#define PORT 8080
#define WINDOW_SIZE 4
#define TIMEOUT 5
#define TOTAL_PACKETS 10

int socket_fd;
struct sockaddr_in server_addr;
int last_ack_received = -1;
int base = 0;
int acked[TOTAL_PACKETS]; // Array to track which packets have been acknowledged

void send_packet(int seq_no) {
    char packet[256];
    sprintf(packet, "Packet %d", seq_no);
    sendto(socket_fd, packet, strlen(packet), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("Packet %d sent\n", seq_no);
}

void resend_packet(int seq_no) {
    printf("Timeout, resending Packet %d\n", seq_no);
    send_packet(seq_no);
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    struct timeval timeout = {TIMEOUT, 0}; // Timeout of 5 seconds
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

    // Initialize the acknowledgment array to track received packets
    memset(acked, 0, sizeof(acked));

    while (last_ack_received < TOTAL_PACKETS - 1) {
        // Send new packets within the window
        while (base < TOTAL_PACKETS && base < last_ack_received + WINDOW_SIZE + 1) {
            if (!acked[base]) {
                send_packet(base);
            }
            base++;
        }

        // Receive acknowledgment for the packets
        char ack_buffer[sizeof(int)];
        int server_len = sizeof(server_addr);
        int recv_len = recvfrom(socket_fd, ack_buffer, sizeof(ack_buffer), 0, (struct sockaddr*)&server_addr, &server_len);

        if (recv_len > 0) {
            int ack;
            memcpy(&ack, ack_buffer, sizeof(int));
            printf("Acknowledgment for Packet %d received\n", ack);

            if (ack >= last_ack_received + 1) {
                // Update last ack received and mark the acknowledged packet
                last_ack_received = ack;
                acked[ack] = 1;
            }
        } else {
            // Handle timeout: Resend only unacknowledged packets
            for (int i = 0; i < TOTAL_PACKETS; i++) {
                if (!acked[i]) {
                    resend_packet(i);
                }
            }
        }
    }

    closesocket(socket_fd);
    WSACleanup();
    return 0;
}
