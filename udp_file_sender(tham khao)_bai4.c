#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

typedef struct {
    int sender_id;
    int file_size;
} Header;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <receiver_ip> <receiver_port> <file_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *receiver_ip = argv[1];
    int receiver_port = atoi(argv[2]);
    char *file_path = argv[3];

    // Create socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        printf("Error: Unable to create socket\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(receiver_ip);
    addr.sin_port = htons(receiver_port);

    // Open file and read contents into buffer
    FILE *fp = fopen(file_path, "rb");
    if (fp == NULL) {
        printf("Error: Unable to open file\n");
        exit(EXIT_FAILURE);
    }

    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    rewind(fp);

    char *file_buffer = malloc(file_size);
    fread(file_buffer, 1, file_size, fp);
    fclose(fp);

    // Send header with sender ID and file size
    Header header;
    header.sender_id = 123; // example sender ID
    header.file_size = file_size;

    sendto(sock, &header, sizeof(header), 0, (struct sockaddr *)&addr, sizeof(addr));

    // Send file content
    int offset = 0;
    while (offset < file_size) {
        int bytes_to_send = file_size - offset > BUFFER_SIZE ? BUFFER_SIZE : file_size - offset;
        sendto(sock, file_buffer + offset, bytes_to_send, 0, (struct sockaddr *)&addr, sizeof(addr));
        offset += bytes_to_send;
        usleep(100); // delay to reduce send rate
    }

    free(file_buffer);
    close(sock);

    printf("File sent successfully\n");

    return 0;
}
