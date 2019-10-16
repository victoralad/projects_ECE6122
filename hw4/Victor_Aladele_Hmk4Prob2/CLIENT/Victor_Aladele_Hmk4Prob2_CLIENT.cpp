/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Oct 15, 2019
Description:
    http://www.linuxhowtos.org/C_C++/socket.htm
    A simple client in the internet domain using UDP
    The port number is passed as an argument 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <iostream>
#include <thread>

/* Assume that any non-Windows platform uses POSIX-style sockets instead. */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#include <unistd.h> /* Needed for close() */

typedef int SOCKET;
char buffer[1024];
int sockfd, portno, n;
struct sockaddr_in serv_addr;
struct hostent *server;
socklen_t fromlen = 0;
struct sockaddr  from;

/* Note: For POSIX, typedef SOCKET as an int. */

int sockClose(SOCKET sock)
{
    int status = 0;

    status = shutdown(sock, SHUT_RDWR);
    if (status == 0)
    {
        status = close(sock);
    }

    return status;

}

void error(const char *msg)
{
    perror(msg);

    exit(0);
}

void rec_msgs()
{
    while (true) 
    {
        n = recvfrom(sockfd, buffer, 1023, 0, (sockaddr *)&from, &fromlen);
        
        if (n < 0)
        {
            error("ERROR reading from socket");
        }
        buffer[n] = 0;
        printf("\nReceived Msg Type: %s", buffer);
        // std::cout << "Please: " << std::endl;
        printf("Please enter command: \n");
        // std::cout << std::flush;

    }
}

int main(int argc, char *argv[])
{
    memset((char *)&from, 0, sizeof(sockaddr));

    if (argc < 3) 
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    
    // Convert string to int
    portno = atoi(argv[2]);
    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        error("ERROR opening socket");
    }

    server = gethostbyname(argv[1]);

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    
    // Zero out serv_addr variable
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;

    memmove((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);

    serv_addr.sin_port = htons(portno);

    fromlen = sizeof(serv_addr);

    std::thread t1(rec_msgs);
    
    while (true)
    {
        printf("Please enter command: ");

        memset(buffer, 0, 1024);
        fgets(buffer, 1023, stdin);

        if (buffer[0] == 'q')
        {
            t1.join();
            break;
        }

        n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

        if (n < 0)
        {
            error("ERROR writing to socket");
        }

        memset(buffer, 0, 1024);

    }

    sockClose(sockfd);

    return 0;
}