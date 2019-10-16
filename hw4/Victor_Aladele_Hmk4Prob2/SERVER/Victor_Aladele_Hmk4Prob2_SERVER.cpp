/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Oct 15, 2019
Description:
    http://www.linuxhowtos.org/C_C++/socket.htm
    A simple server in the internet domain using UDP
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
int sockfd, portno, n, command = -1;
socklen_t fromlen;
struct sockaddr_in serv_addr, from;


struct udpMessage
{
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    unsigned long lSeqNum;
    char chMsg[1000];
};

/////////////////////////////////////////////////
// Cross-platform socket close
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
/////////////////////////////////////////////////
// Output error message and exit
void error(const char *msg)
{
    perror(msg);
    // Make sure any open sockets are closed before calling exit
    exit(1);
}

/////////////////////////////////////////////////
// send messages if command == 0
void send_msgs()
{
    n = sendto(sockfd, buffer, 1023, 0, (struct sockaddr *)&from, fromlen);
    if (n < 0)
    {
        printf("There is no message to send. Buffer is empty.\n");
    }
}

/////////////////////////////////////////////////
// thread always running and waiting to receive messages
void rec_msgs() 
{
    while (true) 
    {
        n = recvfrom(sockfd, buffer, 1023, 0, (struct sockaddr *)&from, &fromlen);
        if (n < 0)
        {
            error("recvfrom");
        }
        buffer[n] = 0;  // Null terminate
    }
}
/////////////////////////////////////////////////
// Main
int main(int argc, char *argv[])
{
    
    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    // Create the socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    // Make sure the socket was created
    if (sockfd < 0)
    {
        error("ERROR opening socket");
    }
    // Zero out the variable serv_addr
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    // Convert the port number string to an int
    portno = atoi(argv[1]);
    
    // Initialize the serv_addr
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // Convert port number from host to network
    serv_addr.sin_port = htons(portno);
    // Bind the socket to the port number
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR on binding");
    }

    fromlen = sizeof(struct sockaddr_in);

    std::thread handleClients(rec_msgs);

    while (true)
    {
        printf("Please enter command: ");

        std::cin >> command;

        if (command == 0) 
        {
            send_msgs();
        }
        else if (command == 1) 
        {
            memset(buffer, 0, 1024);
        }
        else if (command == 2) 
        {
            printf("Composite Msg: %s\n", buffer);
        }
    }

    sockClose(sockfd);


    return 0;
}