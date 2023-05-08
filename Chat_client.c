#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8888
#define BUFFER_SIZE 1024

int main() {
    int sockfd = 0;
    char buffer[BUFFER_SIZE] = {0};
    struct sockaddr_in serv_addr;

    // create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation error");
        exit(EXIT_FAILURE);
    }

    // setup server address
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_ADDRESS, &serv_addr.sin_addr) <= 0) {
        perror("invalid address");
        exit(EXIT_FAILURE);
    }

    // connect to server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    // prompt user to enter their name
    char name[BUFFER_SIZE];
    printf("Enter your name: ");
    fgets(name, BUFFER_SIZE, stdin);

    // send client's name to server
    char message[BUFFER_SIZE];
    snprintf(message, BUFFER_SIZE, "%s\n", name);
    if (send(sockfd, message, strlen(message), 0) == -1) {
        perror("send failed");
        exit(EXIT_FAILURE);
    }

    while (true) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        FD_SET(STDIN_FILENO, &read_fds);

        // wait for input from stdin or server socket
        if (select(sockfd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select failed");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(sockfd, &read_fds)) {  // receive message from server
            if (recv(sockfd, buffer, BUFFER_SIZE, 0) <= 0) {
                printf("server disconnected\n");
                break;
            }
            printf("%s", buffer);
            memset(buffer, 0, sizeof(buffer));
        }

        if (FD_ISSET(STDIN_FILENO, &read_fds)) {  // send message to server
            fgets(buffer, BUFFER_SIZE, stdin);
            snprintf(message, BUFFER_SIZE, "%s: %s", name, buffer);
            if (send(sockfd, message, strlen(message), 0) == -1) {
                perror("send failed");
                exit(EXIT_FAILURE);
            }
            memset(buffer, 0, sizeof(buffer));
        }
    }

    // close socket
    close(sockfd);

    return 0;
}