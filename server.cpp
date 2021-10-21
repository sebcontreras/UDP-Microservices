// Server side C/C++ program to demonstrate Socket programming
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

using namespace std;

#define PORT 12345
#define TPORT 44111
#define CPORT 5454
#define VPORT 6767
#define MAXLINE 1024
#define MSG_CONFIRM 0 // TEMP FOR USE ON MAC

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

void translator(int clientSocket, string clientWord)
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

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(TPORT);
    // servaddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    servaddr.sin_addr.s_addr = INADDR_ANY;
    // 136.159.5.42 linuxlab
    // 136.159.5.41 from output
    // 172.19.2.145 first command line
    // 192.168.122.1 second command line
    // if (inet_pton(AF_INET, "136.159.5.41", &servaddr.sin_addr) == -1)
    // {
    //     printf("address creation failed\n");
    // }

    // if Eword is "exit", leave the loop
    while (strncmp(eWord.c_str(), "exit", 4) != 0)
    {
        int n, len;
        // Send data to UDP service
        sendto(UDPsock, eWord.c_str(), strlen(eWord.c_str()),
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));
        printf("Message sent to UDP Translator service: %s\n", eWord.c_str());

        // Get response from UDP service
        n = recvfrom(UDPsock, (char *)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     (socklen_t *)&len);
        buffer[n] = '\0';
        printf("Response from UDP Translator service: %s\n", buffer);

        // Parse response

        // Send translated word back to TCP client
        if (send(clientSocket, buffer, n, 0) == -1)
        {
            printf("send() call failed\n");
            return;
        }
        printf("\nSent translated word back to client: %s\n\n........................\n\n", buffer);

        // Here we should get response from TCP
        int bytes;
        if ((bytes = recv(clientSocket, bufferServer, MAXLINE - 1, 0)) > 0)
        {
            printf("Loop response from TCP client: %s\n", bufferServer);

            // Get data
            char bufferCopy[sizeof(bufferServer)];
            strcpy(bufferCopy, bufferServer);
            char tempData[bytes - 2];
            strncpy(tempData, bufferCopy + 2, bytes - 2);
            string data(tempData);
            printf("\nThe data is: %s", data.c_str());
            printf("\nCalling trans function");

            // Set new eWord
            eWord = data;
        }
    }
    printf("\nClosing translator microservice...\n");
    close(UDPsock);
}

void currency(int clientSocket, string currencyDat)
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

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(CPORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // if requestDat is "exit", leave the loop
    while (strncmp(requestDat.c_str(), "exit", 4) != 0)
    {
        int n, len;
        // Send data to UDP service
        sendto(UDPsock, requestDat.c_str(), strlen(requestDat.c_str()),
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));
        printf("Message sent to UDP Currency service: %s\n", requestDat.c_str());

        // Get response from UDP service
        n = recvfrom(UDPsock, (char *)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     (socklen_t *)&len);
        buffer[n] = '\0';
        printf("Response from UDP Currency service: %s\n", buffer);

        // Parse response

        // Send translated word back to TCP client
        if (send(clientSocket, buffer, n, 0) == -1)
        {
            printf("send() call failed\n");
            return;
        }
        printf("\nSent translated word back to client: %s\n\n........................\n\n", buffer);

        // Here we should get response from TCP
        int bytes;
        if ((bytes = recv(clientSocket, bufferServer, MAXLINE - 1, 0)) > 0)
        {
            printf("Loop response from TCP client: %s\n", bufferServer);

            // Get data
            char bufferCopy[sizeof(bufferServer)];
            strcpy(bufferCopy, bufferServer);
            char tempData[bytes - 2];
            strncpy(tempData, bufferCopy + 2, bytes - 2);
            string data(tempData);
            printf("\nThe data is: %s", data.c_str());
            printf("\nRunning currency function");

            // Reset data with new request data
            requestDat = data;
        }
    }
    printf("\nClosing translator microservice...\n");
    close(UDPsock);
}

void showCandidates(int clientSocket, int UDPsock, struct sockaddr_in servaddr)
{

    int n, len;
    char buffer[MAXLINE];
    char bufferServer[MAXLINE];
    // Send data to UDP service
    string requestData = "1";

    sendto(UDPsock, requestData.c_str(), strlen(requestData.c_str()),
           MSG_CONFIRM, (const struct sockaddr *)&servaddr,
           sizeof(servaddr));
    printf("Message sent to UDP Voting service: %s\n", requestData.c_str());

    // Get response from UDP service
    n = recvfrom(UDPsock, (char *)buffer, MAXLINE,
                 MSG_WAITALL, (struct sockaddr *)&servaddr,
                 (socklen_t *)&len);
    buffer[n] = '\0';
    printf("Response from UDP Voting service:\n%s\n", buffer);

    // Parse response

    // Send translated word back to TCP client
    if (send(clientSocket, buffer, n, 0) == -1)
    {
        printf("send() call failed\n");
        return;
    }
    printf("\nSent candidate list back to client: %s\n\n........................\n\n", buffer);

    // Here we could get CONFIRMATION response from TCP
}

void showSummary(int clientSocket, int UDPsock, struct sockaddr_in servaddr)
{

    int n, len;
    char buffer[MAXLINE];
    char bufferServer[MAXLINE];
    // Send data to UDP service
    string requestData = "2";

    sendto(UDPsock, requestData.c_str(), strlen(requestData.c_str()),
           MSG_CONFIRM, (const struct sockaddr *)&servaddr,
           sizeof(servaddr));
    printf("Message sent to UDP Voting service: %s\n", requestData.c_str());

    // Get response from UDP service
    n = recvfrom(UDPsock, (char *)buffer, MAXLINE,
                 MSG_WAITALL, (struct sockaddr *)&servaddr,
                 (socklen_t *)&len);
    buffer[n] = '\0';
    printf("Response from UDP Voting service:\n%s\n", buffer);

    // Parse response

    // Send translated word back to TCP client
    if (send(clientSocket, buffer, n, 0) == -1)
    {
        printf("send() call failed\n");
        return;
    }
    printf("\nSent candidate list back to client: %s\n\n........................\n\n", buffer);

    // Here we could get CONFIRMATION response from TCP
}

void voting(int clientSocket, string votDat)
{
    // Create UDP socket for voting microservice
    int UDPsock;
    char buffer[MAXLINE];
    char bufferServer[MAXLINE];
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ((UDPsock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("showCandidates: socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(VPORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // votDat should have the PARSED request
    // (first char is not microservice ID)

    // First char should be the ID for which
    // voting service we want
    string sITemp = votDat.substr(0, 1);
    int serviceID = std::stoi(sITemp);
    printf("\nVotingService ID: %d", serviceID);

    // switch to determine which voting service was selected
    // NEED TO ADD WHILE LOOP TO CHECK FOR EXIT or else we need
    // to recreate UDPsock for voting each time
    int running = 1;
    //while (running)
    //{
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
        //castVote(sock);
        break;
    case 4:
        printf("\nSelected EXIT!\n");
        running = 0;
        break;
    default:
        printf("\nInvalid request from client....\n");
        break;
    }
    // Get next request, set serviceID
    //}
    printf("\nClosing voting microservice...\n");
    close(UDPsock);
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
    printf("Server running at IP: %s on port: %d\n", buffer1, PORT);
    if (listen(parent_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Listening...\n");

    int clientBytes;

    // Connection acceptance
    if ((client_socket = accept(parent_socket, (struct sockaddr *)&client_address,
                                (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        printf("\nListening for client request...\n");

        // Get initial response from client
        if ((clientBytes = recv(client_socket, buffer, 2048, 0)) > 0)
        {
            printf("recv() %d bytes from client:\n\n%s\n", clientBytes, buffer);
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
            printf("\ntempServ is: %s", tempServ);
            serv = std::stoi(tempServ);
            printf("\nThe command is: %d", serv);

            // Get data
            char tempData[clientBytes - 2];
            strncpy(tempData, bufferCopy + 2, 100);
            string data(tempData);
            printf("\nThe data is: %s", data.c_str());
            printf("\nCalling trans function");

            // switch for determining which service to call
            switch (serv)
            {
            case 1:
                printf("\nSelected translator!\n");
                translator(client_socket, data);
                break;
            case 2:
                printf("\nSelected currency!\n");
                currency(client_socket, data);
                break;
            case 3:
                printf("\nSelected voting!\n");
                voting(client_socket, data);
                break;
            case 4:
                printf("\nSelected EXIT!\n");
                break;
            default:
                printf("\nInvalid request....\n");
                break;
            }

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
