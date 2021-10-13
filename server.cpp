// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

#define PORT 12345
#define PORT2 56789

void sendToclient(const char *buff, int buffLength, int &client_sock)
{

    string temp;

    temp.append(buff);

    int totalsent = 0;
    int singlesend;
    char bufferResponseCopy[buffLength + 1];
    strncpy(bufferResponseCopy, buff, buffLength - 1);
    bufferResponseCopy[buffLength - 1] = '\0';
    while (totalsent < buffLength)
    {
        if ((singlesend = send(client_sock, (void *)(buff + totalsent), buffLength - totalsent, 0)) < 0)
        {
            fprintf(stderr, " Error in sending to server ! \n");
            exit(1);
        }
        totalsent += singlesend;
        //cout << buff;
        printf("\nbuff length: %d, total sent: %d, singlesend: %d", buffLength, totalsent, singlesend);
    }
}

int main(int argc, char const *argv[])
{
    int parent_socket, client_socket;
    struct sockaddr_in client_address;
    int opt = 1;
    int addrlen = sizeof(client_address);
    char buffer[2048] = {0};

    printf("Hello from server!\n");
    // Creating socket file descriptor
    if ((parent_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed\n");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(parent_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // client_address initialization
    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = INADDR_ANY;
    client_address.sin_port = htons(PORT);

    // Bind socket
    if (bind(parent_socket, (struct sockaddr *)&client_address,
             sizeof(client_address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listening
    char buffer1[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_address.sin_addr, buffer1, sizeof(buffer1));
    printf("client_address:%s\n", buffer1);
    if (listen(parent_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Listening...\n");

    int clientBuffer;
    while (1)
    {
        // Connection acceptance
        if ((client_socket = accept(parent_socket, (struct sockaddr *)&client_address,
                                    (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Get response from client
        if ((clientBuffer = recv(client_socket, buffer, 2048, 0)) > 0)
        {
            // Parse response from client
            //      e.g, TRNS hello, CURR 10 CAD US, _______
            //      serv = "TRNS", data = "hello"
            // switch for determining which service to call
            // 1: TRANSLATOR
            //      translator(data)
            // 2: CURRENCY
            //      currency(data)
            // 3: VOTING
            //      voting(data)
            // 4: EXIT //NOT SURE IF WE SHOULD BE ABLE TO KILL SERVER FROM CLIENT
            //      break out of loop
            // 5: DEFAULT
            //      Error message, invalid request

            // For each service we need to:
            //      Set up UDP client-socket to send info to service
            //      Send data to service
            //      Get response from service
            //      Parse response
            //      return response
            //      Send back to TCP client
            //      Close UDP client-socket
        }
    }
    printf("\nPROJECT EXIT WITH CODE 0\n");
    close(client_socket);
    close(parent_socket);
    return 0;
}
