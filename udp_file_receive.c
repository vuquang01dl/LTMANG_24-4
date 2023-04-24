#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#define BUFFER_SIZE 1024

int main()
{

    // Tạo socket theo giao thức UDP
    int receiver = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    // Khai báo địa chỉ bên nhận
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9090);

    bind(receiver, (struct sockaddr *)&addr, sizeof(addr));

// Receive file name and file content from udp_file_sender
    char file_name[BUFFER_SIZE];
    struct sockaddr_in sender_addr;
    socklen_t sender_addr_len = sizeof(sender_addr);
    recv(receiver, file_name, BUFFER_SIZE, 0);
    int file_size = recvfrom(receiver, NULL, 0, MSG_PEEK, (struct sockaddr *)&sender_addr, &sender_addr_len);

    char *file_buffer = malloc(file_size);
    recvfrom(receiver, file_buffer, file_size, 0, (struct sockaddr *)&sender_addr, &sender_addr_len);

    // Write file content to file
    FILE *fp = fopen(file_name, "wb");
    if (fp == NULL) {
        printf("Error: Unable to create file\n");
        exit(EXIT_FAILURE);
    }

    fwrite(file_buffer, 1, file_size, fp);
    fclose(fp);

    printf("File received successfully\n");
    return 0;
}