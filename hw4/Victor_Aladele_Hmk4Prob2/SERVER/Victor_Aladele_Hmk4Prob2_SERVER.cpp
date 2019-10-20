/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Oct 19, 2019
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
#include <vector>

/* Assume that any non-Windows platform uses POSIX-style sockets instead. */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#include <unistd.h> /* Needed for close() */
#include <map>

typedef int SOCKET;
char command;
int sockfd, portno, n, maxMsgLen = 1000; 
socklen_t fromlen;
struct sockaddr_in serv_addr, from;
bool port_used = false;
std::vector<sockaddr_in> client;
std::map<int, struct udpMessage> seq;
std::map<int, struct udpMessage>::iterator it;

struct udpMessage
{
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    unsigned long lSeqNum;
    char chMsg[1000];
}udpMsg, udpMsg_recv;


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
    if (udpMsg.nVersion != '1')
    {
        return;
    }
    for (int i = 0; i < client.size(); ++i)
    {
        n = sendto(sockfd, (char*)&udpMsg, sizeof(udpMessage), 0, (struct sockaddr *)&client[i], fromlen);
        if (n < 0)
        {
            printf("There is no message to send. Buffer is empty.\n");
        }
    }
}

/////////////////////////////////////////////////
// thread always running and waiting to receive messages
void rec_msgs() 
{
    while (true) 
    {
        n = recvfrom(sockfd, (char*)&udpMsg_recv, sizeof(udpMessage), 0, (struct sockaddr *)&from, &fromlen);
        if (n < 0)
        {
            error("recvfrom");
        }
        
        // check if this is the first client that is connecting
        if (client.size() < 1)
        {
            client.push_back(from);
        }
        else
        {
            for (int i = 0; i < client.size(); ++i)
            {
                // check if server has received messages from this client before. 
                if (client[i].sin_addr.s_addr == from.sin_addr.s_addr && client[i].sin_port == from.sin_port)
                {
                    port_used = true;
                    break;
                }
            }
            if (port_used == false) 
            {
               client.push_back(from); 
            }
        }

        // check if the version number of the incoming message is valid
        if (udpMsg_recv.nVersion == '1')
        {
            memcpy(&udpMsg, &udpMsg_recv, sizeof(udpMessage));
        }
        // the composite message is immediately cleared and anything in the chMsg buffer is ignored
        if (udpMsg_recv.nType == '0')
        {
            memset(udpMsg.chMsg, 0, sizeof(udpMsg.chMsg));
            seq.clear();
        }
        // the composite message is immediately cleared and the message in chMsg is used as the start of a new composite message
        else if (udpMsg_recv.nType == '1')
        {
            memset(udpMsg.chMsg, 0, sizeof(udpMsg.chMsg));
            if (udpMsg_recv.nMsgLen > maxMsgLen)
            {
                memcpy(udpMsg.chMsg, udpMsg_recv.chMsg, maxMsgLen - 1);
                send_msgs();
            }
            else
            {
                memcpy(udpMsg.chMsg, udpMsg_recv.chMsg, sizeof(udpMsg.chMsg));
            }
            
            seq.clear();
        }
        // the message in chMsg is added to the composite message based on its lSeqNum
        else if (udpMsg_recv.nType == '2')
        {
            // memset(udpMsg.chMsg, 0, sizeof(udpMsg.chMsg));
            seq[udpMsg_recv.lSeqNum] = udpMsg_recv;
            for (it = seq.begin(); it != seq.end(); ++it)
            {
                if (udpMsg.nMsgLen + udpMsg_recv.nMsgLen > maxMsgLen)
                {
                    std::cout << "greater than!" << std::endl;
                    memcpy(udpMsg.chMsg + udpMsg.nMsgLen, udpMsg_recv.chMsg, maxMsgLen - udpMsg.nMsgLen - 1);
                }
                else
                {
                    memcpy(udpMsg.chMsg + udpMsg.nMsgLen - it->second.nMsgLen, it->second.chMsg, it->second.nMsgLen);
                    // strcpy(udpMsg.chMsg + udpMsg.nMsgLen, it->second.chMsg);
                    udpMsg.nMsgLen += it->second.nMsgLen;
                    // std::cout << udpMsg.chMsg << " ";
                }
                
            }
            // std::cout << "\n";
            // memset(udpMsg.chMsg, 0, sizeof(seq.begin()->second.nMsgLen));
            std::cout << udpMsg.chMsg << std::endl;
            // std::cout << " seq: " << seq.size() << std::endl;

        }
        // the server immediately sends to all clients the current composite message and clears out the composite message.
        else if (udpMsg_recv.nType == '3')
        {
            send_msgs();
            memset(udpMsg.chMsg, 0, sizeof(udpMsg.chMsg));
            seq.clear();
        }
        
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

    // Initialize udpMessage struct
    udpMsg.nVersion = '0';
    udpMsg.nType = '0';
    udpMsg.nMsgLen = 0;
    udpMsg.lSeqNum = 0;
    memset(udpMsg.chMsg, 0, maxMsgLen);

    while (true)
    {
        printf("Please enter command: ");

        std::cin >> command;

        while (command != '0' && command != '1' && command != '2')
        {
            printf("Please enter command: ");
            std::cin >> command;
        }

        if (command == '0') 
        {
            send_msgs();
        }
        else if (command == '1') 
        {
            memset(udpMsg.chMsg, 0, maxMsgLen);
        }
        else if (command == '2') 
        {
            printf("Composite Msg: %s\n", udpMsg.chMsg);
        }
        
    }

    sockClose(sockfd);


    return 0;
}