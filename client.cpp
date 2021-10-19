#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <iostream>
#include <string.h>
#include <arpa/inet.h>

using namespace std;

/* Global manifest constants */
#define MAX_MESSAGE_LENGTH 100
// Need to get server IP and port from command line
#define PORT 12345

/* Global variable */
const char *COMMANDS[4] = {"translator", "currency", "voting", "exit"};

void translator(int sock)
{
    char buffer[MAX_MESSAGE_LENGTH];
    while (1)
    {
        // prompt user to enter word to translate or exit
        printf("\nEnter an English word,\nor type 'exit' to quit translator\n");

        // get word
        string input;
        cin >> input;
        printf("\nThe input is:\n%s\n", input.c_str());

        // Append input word to service key
        string mssg = "1 ";
        mssg.append(input);
        printf("\nThe mssg: %s", mssg.c_str());

        // send SERVICE KEY + word to sock
        if (send(sock, mssg.c_str(), strlen(mssg.c_str()) + 1, 0) == -1)
        {
            printf("send() call failed\n");
            return;
        }
        printf("\nInput sent to server\n\n........................\n\n");

        // if word is "exit"
        if (strncmp(input.c_str(), "exit", 4) == 0)
        {
            printf("\nLeaving translator microservice...");
            return;
        }

        int n;
        if ((n = read(sock, buffer, sizeof(buffer) - 1)) > 0)
        {
            buffer[n] = '\0';
            printf("Bytes returned: %d", n);
            printf("\nFrench translation: %s\n", buffer);
        }
        else
        {
            printf("\nERROR: NO RESPONSE FROM INDIRECTION SERVER");
        }
    }
}

void currency(int sock)
{
    char buffer[MAX_MESSAGE_LENGTH];
    cin.ignore();
    while (1)
    {
        // prompt user to enter word to translate or exit
        printf("\nEnter a currency to convert using format:\n   $<amount> <source> <dest>\n   ex. $10 CAD USD\n\n(type 'exit' to quit currency)\n");
        printf("\nSupported Currencies: USD, CAD, EUR, GBP, BTC, ETH, DOGE\n\n");
        // get data
        string input;
        cin.clear();
        getline(cin, input);
        printf("\nThe input is:\n%s\n", input.c_str());

        // Append input word to service key
        string mssg = "2 ";
        mssg.append(input);
        printf("\nThe mssg: %s", mssg.c_str());

        // send SERVICE KEY + word to sock
        if (send(sock, mssg.c_str(), strlen(mssg.c_str()) + 1, 0) == -1)
        {
            printf("send() call failed\n");
            return;
        }
        printf("\nInput sent to server\n\n........................\n\n");

        // if word is "exit"
        if (strncmp(input.c_str(), "exit", 4) == 0)
        {
            printf("\nLeaving currency microservice...");
            return;
        }

        int n;
        if ((n = read(sock, buffer, sizeof(buffer) - 1)) > 0)
        {
            buffer[n] = '\0';
            printf("Bytes returned: %d", n);
            printf("\nCurrency exchange: %s\n", buffer);
        }
        else
        {
            printf("\nERROR: NO RESPONSE FROM INDIRECTION SERVER");
        }
    }
}

int main()
{

    struct sockaddr_in serverAddress;
    int localSocket;
    //char output[1024];

    //get IP and PORT from command line

    // serverAddress initialization
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "0.0.0.0", &serverAddress.sin_addr) == -1)
    {
        printf("address creation failed\n");
        return -1;
    }
    // Socket creation
    if ((localSocket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("socket() call failed\n");
        return -1;
    }

    printf("\nWaiting for connection\n");
    // Connection request
    if (connect(localSocket, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_in)) == -1)
    {
        printf("connect() call failed\n");
        return -1;
    }
    printf("Before while loop");

    //start while loop to check for input
    int running = 1;
    while (running)
    {
        // request user to select microservice
        printf("\n\nPlease type one of the following commands:\n\n1. translator\n\n2. currency\n\n3. voting\n\n4. BYE\n\n");
        int input;
        cin >> input;
        printf("\nThe input is:\n%d\n", input);

        // switch for determining which service to call
        switch (input)
        {
        case 1:
            printf("\nSelected translator!\n");
            translator(localSocket);
            break;
        case 2:
            printf("\nSelected currency!\n");
            currency(localSocket);
            break;
        case 3:
            printf("\nSelected voting!\n");
            // voting(localSocket)
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

    // Connection termination
    close(localSocket);
    printf("\nSAFELY TERMINATING CLIENT APPLICATION, BYE!\n");

    return 0;
}