#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>

#define BUF_SIZE 1024
#define MAX_WAIT_INTERVAL 8000

int main(int argc, char* argv[]) {
    int sockfd, n, count, max_retry;
    char buf[BUF_SIZE];
    struct sockaddr_in serv_addr;
    struct timeval tv; // {seconds; microseconds}
    fd_set readfds;    // File Descriptor Set (FD_SETSIZE = 1024)

    if (argc != 5) {
        fprintf(stderr, "Usage: %s <IP> <port> <message> <max_retry>\n", argv[0]);
        exit(1);
    }

    // create a UDP socket
    // socket(AF_INET for IPv4, SOCK_DGRAM (datagram) using UDP, Protocal(default = 0))
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // set the server address to send the message to
    memset(&serv_addr, 0, sizeof(serv_addr));       //set bits in struct serv_addr to 0
    serv_addr.sin_family = AF_INET;                 //for IPv4
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]); //convert IPv4 No.&Dots notation to binary data
    serv_addr.sin_port = htons(atoi(argv[2]));      //HostToNetworkShortInteger( StringToInt( Port Number))

    // set the max retry count and calculate the base timeout
    max_retry = atoi(argv[4]);
    int base_timeout = 500;              // in milliseconds

    // send the message and wait for the echo response
    count = 0;
    while (count <= max_retry) {
        // set the timeout for the current attempt using exponential backoff
        int timeout = base_timeout * (1 << count); // 2^count (exponential backoff)
        if (timeout > MAX_WAIT_INTERVAL) {
            timeout = MAX_WAIT_INTERVAL;
        }

        // set up the file descriptor set for the select() call
        FD_ZERO(&readfds);         // Initialize file descriptor to all 0
        FD_SET(sockfd, &readfds);  // Set the bit for sockfd in the fd set readfds

        // set up the timeout value for select()
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;

        // send the message to the server
        if (sendto(sockfd, argv[3], strlen(argv[3]), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
            perror("sendto");
            exit(1);
        }

        printf("Sent message to %s:%s: %s\n", argv[1], argv[2], argv[3]);

        // wait for the echo response from the server
        if (select(sockfd + 1, &readfds, NULL, NULL, &tv) == -1) {
            perror("select");
            exit(1);
        }

        // FD_ISSET to check if sockfd is in the fd set readfds
        if (FD_ISSET(sockfd, &readfds)) {
            if ((n = recvfrom(sockfd, buf, BUF_SIZE, 0, NULL, NULL)) == -1) {
                perror("recvfrom");
                exit(1);
            }
            buf[n] = '\0';

            printf("Received echo response: %s\n", buf);

            // exit the program with a success code if the echo response was received
            exit(0);
        } else {
            // the timeout expired, so increase the retry count and try again
            printf("Timeout expired after %d milliseconds. Retrying...\n\n", timeout);
            count++;
        }
    }

    // exit the program with an error code if the max retry count was reached
    fprintf(stderr, "Max retry count reached. Exiting...\n");
    exit(1);
}
