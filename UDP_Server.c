#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[]) {
    int sockfd, len, n;
    char buf[BUF_SIZE];
    struct sockaddr_in serv_addr, cli_addr;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    // socker() to create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // bind the socket to a specific IP and port
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("bind");
        exit(1);
    }

    printf("Listening for UDP messages on %s:%s...\n", argv[1], argv[2]);

    // loop to receive incoming messages and send them back to the client
    while (1) {
        len = sizeof(cli_addr);
        if ((n = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr*)&cli_addr, &len)) == -1) {
            perror("recvfrom");
            exit(1);
        }
        buf[n] = '\0';

        printf("Received message from %s:%d: %s\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buf);

        // send the same message back to the client
        if (sendto(sockfd, buf, n, 0, (struct sockaddr*)&cli_addr, len) == -1) {
            perror("sendto");
            exit(1);
        }

        printf("Sent echo response to %s:%d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
    }

    close(sockfd);

    return 0;
}
