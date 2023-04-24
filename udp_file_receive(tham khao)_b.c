#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

typedef struct {
    int sender_id;
    int file_size;
} Header;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <receiver_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int receiver_port = atoi(argv[1]);

    // Create socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        printf("Error: Unable to create socket\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(receiver_port);

    // Bind socket to port
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("Error: Unable to bind socket to port\n");
        exit(EXIT_FAILURE);
    }

    // Loop to receive files
    while (1) {
        Header header;

        // Receive header
        struct sockaddr_in sender_addr;
        socklen_t sender_addr_len = sizeof(sender_addr);

        int bytes_received = recvfrom(sock, &header, sizeof(header), 0, (struct sockaddr *)&sender_addr, &sender_addr_len);
        if (bytes_received < sizeof(header)) {
            printf("Error: Header not received completely\n");
            continue;
        }

        // Receive file content
        char file_path[100];
        sprintf(file_path, "received_%d.dat", header.sender_id);

        FILE *fp = fopen(file_path, "wb");
        if (fp == NULL) {
            printf("Error: Unable to create file\n");
            continue;
        }

        int bytes_remaining = header.file_size;
        while (bytes_remaining > 0) {
            char buffer[BUFFER_SIZE];
            int bytes_to_receive = bytes_remaining > BUFFER_SIZE ? BUFFER_SIZE : bytes_remaining;

            bytes_received = recvfrom(sock, buffer, bytes_to_receive, 0, (struct sockaddr *)&sender_addr, &sender_addr_len);
            if (bytes_received < 0) {
                printf("Error: Failed to receive data\n");
                break;
            }

            fwrite(buffer, 1, bytes_received, fp);
            bytes_remaining -= bytes_received;
        }

        fclose(fp);

        printf("File received from sender %d\n", header.sender_id);
    }

    close(sock);

    return 0;
}
