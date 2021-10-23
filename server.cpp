// Server side C/C++ program to demonstrate Socket programming
// 136.159.5.42 linuxlab
// 136.159.5.25 csx1
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include <time.h>

using namespace std;

#define PORT 54344
#define TPORT 44123
#define CPORT 5454
#define VPORT 6767
#define MAXLINE 2048
// #define MSG_CONFIRM 0 // TEMP FOR USE ON MAC

void sendToTCPclient(const char *buff, int buffLength, int &client_sock)
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
    }
}

void translator(int clientSocket, string clientWord, string IP)
{
    int UDPsock;
    string eWord = clientWord;
    char buffer[MAXLINE];
    char bufferServer[MAXLINE];
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ((UDPsock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    //Set timeout of 3s
    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    setsockopt(UDPsock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(TPORT);
    servaddr.sin_addr.s_addr = inet_addr(IP.c_str());

    // if Eword is "exit", leave the loop
    while (strncmp(eWord.c_str(), "exit", 4) != 0)
    {
        int n, len;
        // Send data to UDP service
        sendto(UDPsock, eWord.c_str(), strlen(eWord.c_str()),
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));

        // Get response from UDP service
        if ((n = recvfrom(UDPsock, (char *)buffer, MAXLINE,
                          MSG_WAITALL, (struct sockaddr *)&servaddr,
                          (socklen_t *)&len)) < 0)
        {
            perror("\nError TIMEOUT");
            return;
        }
        buffer[n] = '\0';

        // Send translated word back to TCP client
        sendToTCPclient(buffer, n, clientSocket);

        // Here we should get response from TCP
        int bytes;
        if ((bytes = recv(clientSocket, bufferServer, MAXLINE - 1, 0)) > 0)
        {
            // Get data
            char bufferCopy[sizeof(bufferServer)];
            strcpy(bufferCopy, bufferServer);
            char tempData[bytes - 2];
            strncpy(tempData, bufferCopy + 2, bytes - 2);
            string data(tempData);

            // Set new eWord
            eWord = data;
        }
    }
    printf("\nDone with translator microservice...\n");
    close(UDPsock);
}

void currency(int clientSocket, string currencyDat, string IP)
{
    int UDPsock;
    string requestDat = currencyDat;
    char buffer[MAXLINE];
    char bufferServer[MAXLINE];
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ((UDPsock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    //Set timeout of 3s
    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    setsockopt(UDPsock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(CPORT);
    servaddr.sin_addr.s_addr = inet_addr(IP.c_str());

    // if requestDat is "exit", leave the loop
    while (strncmp(requestDat.c_str(), "exit", 4) != 0)
    {
        int n, len;
        // Send data to UDP service
        sendto(UDPsock, requestDat.c_str(), strlen(requestDat.c_str()),
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));

        setsockopt(UDPsock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        // Get response from UDP service
        if ((n = recvfrom(UDPsock, (char *)buffer, MAXLINE,
                          MSG_WAITALL, (struct sockaddr *)&servaddr,
                          (socklen_t *)&len)) < 0)
        {
            perror("\nError UDP TIMEOUT");
            return;
        }
        buffer[n] = '\0';

        // Parse response

        // Send translated word back to TCP client
        sendToTCPclient(buffer, n, clientSocket);

        // Here we should get response from TCP
        int bytes;
        if ((bytes = recv(clientSocket, bufferServer, MAXLINE - 1, 0)) > 0)
        {
            // Get data
            char bufferCopy[sizeof(bufferServer)];
            strcpy(bufferCopy, bufferServer);
            char tempData[bytes - 2];
            strncpy(tempData, bufferCopy + 2, bytes - 2);
            string data(tempData);

            // Reset data with new request data
            requestDat = data;
        }
    }
    printf("\nDone with translator microservice...\n");
    close(UDPsock);
}

void castVote(int clientSocket, int UDPsock, struct sockaddr_in servaddr)
{
    int n, clientBytes, len, con;
    char keyBuffer[MAXLINE];
    char confBuffer[MAXLINE];
    char voteBuffer[MAXLINE];

    // 2: Send encryption key request to UDP service
    string requestData = "3";
    sendto(UDPsock, requestData.c_str(), strlen(requestData.c_str()),
           MSG_CONFIRM, (const struct sockaddr *)&servaddr,
           sizeof(servaddr));

    // 3: Receive encryption key from UDP service
    if ((n = recvfrom(UDPsock, (char *)keyBuffer, MAXLINE,
                      MSG_WAITALL, (struct sockaddr *)&servaddr,
                      (socklen_t *)&len)) < 0)
    {
        perror("\nError TIMEOUT");
        return;
    }
    keyBuffer[n] = '\0';
    string voteKey(keyBuffer);

    // 4: Send key to TCP client
    sendToTCPclient(keyBuffer, n, clientSocket);

    // 5: Receive encrypted vote from TCP client
    if ((clientBytes = recv(clientSocket, voteBuffer, MAXLINE, 0)) <= 0)
    {
        printf("recv() call failed");
        return;
    }
    voteBuffer[clientBytes] = '\0';
    string encVote(voteBuffer);

    // Format vote request: <service> <vote> <key> (eg. 4 4327894 8)
    string voteReq = "";
    voteReq.append(encVote);
    voteReq.append(" ");
    voteReq.append(voteKey);

    // 6: Send encrypted vote with key to UDP service
    sendto(UDPsock, voteReq.c_str(), strlen(voteReq.c_str()),
           MSG_CONFIRM, (const struct sockaddr *)&servaddr,
           sizeof(servaddr));

    // 7: Receive vote confirmation from UDP service
    if ((con = recvfrom(UDPsock, (char *)confBuffer, MAXLINE,
                        MSG_WAITALL, (struct sockaddr *)&servaddr,
                        (socklen_t *)&len)) < 0)
    {
        perror("\nError TIMEOUT");
        return;
    }
    confBuffer[con] = '\0';

    // 8: Send vote confirmation to TCP client
    sendToTCPclient(confBuffer, strlen(confBuffer), clientSocket);
    printf("Voting confirmation sent to TCP client: %s\n", confBuffer);

    printf("\nDONE CASTING VOTE...\n");
}

void showCandidates(int clientSocket, int UDPsock, struct sockaddr_in servaddr)
{

    int n, len;
    char buffer[MAXLINE];

    // Send data to UDP service
    string requestData = "1";
    sendto(UDPsock, requestData.c_str(), strlen(requestData.c_str()),
           MSG_CONFIRM, (const struct sockaddr *)&servaddr,
           sizeof(servaddr));

    // Get response from UDP service
    if ((n = recvfrom(UDPsock, (char *)buffer, MAXLINE,
                      MSG_WAITALL, (struct sockaddr *)&servaddr,
                      (socklen_t *)&len)) < 0)
    {
        perror("\nError TIMEOUT");
        return;
    }
    buffer[n] = '\0';

    // Send list back to TCP client
    sendToTCPclient(buffer, n, clientSocket);

    printf("\nSent candidate list back to client: %s\n\n........................\n\n", buffer);
}

void showSummary(int clientSocket, int UDPsock, struct sockaddr_in servaddr)
{

    int n, len;
    char buffer[MAXLINE];
    // Send data to UDP service
    string requestData = "2";

    sendto(UDPsock, requestData.c_str(), strlen(requestData.c_str()),
           MSG_CONFIRM, (const struct sockaddr *)&servaddr,
           sizeof(servaddr));
    printf("Message sent to UDP Voting service: %s\n", requestData.c_str());

    // Get response from UDP service
    if ((n = recvfrom(UDPsock, (char *)buffer, MAXLINE,
                      MSG_WAITALL, (struct sockaddr *)&servaddr,
                      (socklen_t *)&len)) < 0)
    {
        perror("\nError TIMEOUT");
        return;
    }
    buffer[n] = '\0';

    // Send summary back to TCP client
    sendToTCPclient(buffer, n, clientSocket);

    printf("\nSent candidate list back to client: %s\n\n........................\n\n", buffer);
}

void voting(int clientSocket, string votDat, string IP)
{
    // Create UDP socket for voting microservice
    int UDPsock;
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ((UDPsock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("showCandidates: socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    //Set timeout of 3s
    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    setsockopt(UDPsock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(VPORT);
    servaddr.sin_addr.s_addr = inet_addr(IP.c_str());

    // First char should be the ID for which
    // voting service we want
    string sITemp = votDat.substr(0, 1);
    int serviceID = std::stoi(sITemp);

    // switch to determine which voting service was selected
    int running = 1;
    switch (serviceID)
    {
    case 1:
        printf("\nSelected show candidates!\n");
        showCandidates(clientSocket, UDPsock, servaddr);
        break;
    case 2:
        printf("\nSelected show voting summary!\n");
        showSummary(clientSocket, UDPsock, servaddr);
        break;
    case 3:
        printf("\nSelected voting!\n");
        castVote(clientSocket, UDPsock, servaddr);
        break;
    case 4:
        printf("\nSelected EXIT!\n");
        running = 0;
        break;
    default:
        printf("\nInvalid request from client....\n");
        break;
    }
    printf("\nClosing voting microservice...\n");
    close(UDPsock);
}

int main(int argc, char const *argv[])
{
    int parent_socket, client_socket;
    struct sockaddr_in client_address;
    int opt = 1;
    int addrlen = sizeof(client_address);
    char buffer[MAXLINE] = {0};
    string IP = "0.0.0.0";

    if (argc > 1)
    {
        string cl(argv[1]);
        IP = cl;
    }
    printf("\nThe IP address of the microservices is: %s\n", IP.c_str());

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
        perror("server bind failed");
        exit(EXIT_FAILURE);
    }

    // Listening
    char buffer1[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_address.sin_addr, buffer1, sizeof(buffer1));
    printf("Indirection Server running at IP: %s on port: %d\n", buffer1, PORT);
    if (listen(parent_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Listening...\n");

    int clientBytes;
    while (1)
    {

        // Connection acceptance
        if ((client_socket = accept(parent_socket, (struct sockaddr *)&client_address,
                                    (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("\nConnected to client, listening for microservice requests...\n");
        int running = 1;
        while (running)
        {
            // Get initial response from client
            if ((clientBytes = recv(client_socket, buffer, MAXLINE - 1, 0)) > 0)
            {
                char bufferCopy[sizeof(buffer)];
                strcpy(bufferCopy, buffer);

                // We want to send the full request to voting service
                // string reqBody(bufferCopy);
                //      e.g, 1 hello, 2 10 CAD US, _______
                //      serv = "1", data = "hello"

                // Get service requested
                int serv;
                char tempServ[2];
                strncpy(tempServ, bufferCopy, 1);
                tempServ[1] = '\0';
                serv = std::stoi(tempServ);

                // Get data
                char tempData[clientBytes - 2];
                strncpy(tempData, bufferCopy + 2, clientBytes - 2);
                string data(tempData);

                // switch for determining which service to call
                switch (serv)
                {
                case 1:
                    printf("\nSelected translator!\n");
                    translator(client_socket, data, IP);
                    break;
                case 2:
                    printf("\nSelected currency!\n");
                    currency(client_socket, data, IP);
                    break;
                case 3:
                    printf("\nSelected voting!\n");
                    voting(client_socket, data, IP);
                    break;
                case 4:
                    printf("\nSelected EXIT!\n");
                    running = 0;
                    break;
                default:
                    printf("\nInvalid request....\n");
                    break;
                }
            }
        }
        printf("\nClient connection closed!\n");
        close(client_socket);
    }
    close(client_socket);
    close(parent_socket);
    printf("\nINDIRECTION SERVER EXIT WITH CODE 0\n");
    return 0;
}
