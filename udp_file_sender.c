#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    char file_name;
    printf("Nhap ten file: ");
    scanf("%c", file_name);

    // Tạo socket theo giao thức UDP
    int sender = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // Khai báo địa chỉ bên nhận
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9090);

    // Open file and read content
    FILE *fp = fopen(file_name, "rb");
    if (fp == NULL)
    {
        printf("Error: Unable to open file\n");
        exit(EXIT_FAILURE);
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *file_buffer = malloc(file_size);
    fread(file_buffer, 1, file_size, fp);
    fclose(fp);

    // Gửi tin nhắn

    send(sender, file_name, strlen(file_name), 0);
    printf("Da gui ten file");

    sendto(sender, file_buffer, file_size, 0, (struct sockaddr *)&addr, sizeof(addr));
    printf("Da gui noi dung file");
    return 0;
}
//  gcc udp_file_sender_1.c -o udp_file_sender_1
// ./udp_file_sender


// gcc udp_file_receive1.c -o udp_file_receive1
// ./udp_file_receive