#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include <cstring>

using namespace std;

/* Global manifest constants */
#define MAX_MESSAGE_LENGTH 2048
// Need to get server IP and port from command line
#define PORT 12345

/* Global variable */
const char *COMMANDS[4] = {"translator", "currency", "voting", "exit"};

bool isNums(const string &str)
{
    return str.find_first_not_of("0123456789") == string::npos;
}

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
        if ((n = read(sock, buffer, sizeof(buffer) - 1)) < 0)
        {
            perror("\nTIMEOUT ERROR: NO RESPONSE FROM INDIRECTION SERVER");
        }
        else
        {
            buffer[n] = '\0';
            printf("Bytes returned: %d", n);
            printf("\nFrench translation: %s\n", buffer);
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
        if ((n = read(sock, buffer, sizeof(buffer) - 1)) < 0)
        {
            perror("\nTIMEOUT ERROR: NO RESPONSE FROM INDIRECTION SERVER");
        }
        else
        {
            buffer[n] = '\0';
            printf("Bytes returned: %d", n);
            printf("\nCurrency exchange: %s\n", buffer);
        }
    }
}

void castVote(int sock)
{
    char buffer[MAX_MESSAGE_LENGTH];
    char conBuffer[MAX_MESSAGE_LENGTH];

    // Prompt user to select voting type
    printf("\nPlease cast your vote by typing the selected candidate's ID: ");
    int voteID;
    //cin.clear();
    cin >> voteID;
    printf("\nThe vote is:\n%d\n", voteID);

    // Format request
    string mssg = "3 3 ";
    mssg.append(to_string(voteID));

    // Send voting request to server
    // send SERVICE KEY + request type "3" (cast vote) to sock
    if (send(sock, mssg.c_str(), strlen(mssg.c_str()) + 1, 0) == -1)
    {
        printf("send() call failed\n");
        return;
    }

    printf("\nVoting request sent to server\n\n........................\n\n");

    // Receive encryption key
    int n;
    if ((n = read(sock, buffer, sizeof(buffer) - 1)) < 0)
    {
        perror("\nTIMEOUT ERROR: NO RESPONSE FROM INDIRECTION SERVER");
        return;
    }
    else
    {
        buffer[n] = '\0';
        printf("Bytes returned: %d", n);
        printf("\n%s\n", buffer);
    }

    // Parse key
    int key;
    char tempKey[2];
    strncpy(tempKey, buffer, 1);
    tempKey[1] = '\0';
    string keyString(tempKey);
    if (!isNums(keyString))
    {
        printf("\nReceived encryption key was not an int...Please try again");
        return;
    }
    printf("\ntempKey is: %s", tempKey);
    key = std::stoi(tempKey);
    printf("\nThe key is: %d", key);

    // Encrypt vote
    int cryptVoteNum = voteID * key;
    string cryptVote = to_string(cryptVoteNum);
    printf("\nThe encrypted vote is: %s", cryptVote.c_str());

    // Format cast vote request
    // We don't need the leading "3" here because we should already be in
    // casVote loop on the server side
    string voteReq = "4 ";
    voteReq.append(cryptVote);

    // Send encrypted vote
    if (send(sock, voteReq.c_str(), strlen(voteReq.c_str()) + 1, 0) == -1)
    {
        printf("send() call failed\n");
        return;
    }

    // Wait for confirmation
    if ((n = read(sock, conBuffer, sizeof(conBuffer) - 1)) < 0)
    {
        perror("\nTIMEOUT ERROR: NO RESPONSE FROM INDIRECTION SERVER");
        printf("\nVoting confirmation not received!\nWe can't be sure that your vote was cast\n");
    }
    else
    {
        conBuffer[n] = '\0';
        // Need to string compare to validate message
        printf("Bytes returned for confirmation: %d", n);
        printf("\n%s\n", conBuffer);
    }
}

void showSummary(int sock)
{
    // Format request
    string mssg = "3 2";
    // send SERVICE KEY + request type "2"(get summary) to sock
    if (send(sock, mssg.c_str(), strlen(mssg.c_str()) + 1, 0) == -1)
    {
        printf("send() call failed\n");
        return;
    }

    printf("\nInput sent to server\n\n........................\n\n");

    // get response
    int n;
    char buffer[MAX_MESSAGE_LENGTH];
    if ((n = read(sock, buffer, sizeof(buffer) - 1)) < 0)
    {
        perror("\nTIMEOUT ERROR: NO RESPONSE FROM INDIRECTION SERVER");
    }
    else
    {
        buffer[n] = '\0';
        printf("Bytes returned: %d", n);
        printf("\n%s\n", buffer);
    }
}

void showCandidates(int sock)
{
    // Format request
    string mssg = "3 1";
    // send SERVICE KEY + request type "1"(get candidates) to sock
    if (send(sock, mssg.c_str(), strlen(mssg.c_str()) + 1, 0) == -1)
    {
        printf("send() call failed\n");
        return;
    }

    printf("\nInput sent to server\n\n........................\n\n");

    // get response
    int n;
    char buffer[MAX_MESSAGE_LENGTH];
    if ((n = read(sock, buffer, sizeof(buffer) - 1)) < 0)
    {
        perror("\nTIMEOUT ERROR: NO RESPONSE FROM INDIRECTION SERVER");
    }
    else
    {
        buffer[n] = '\0';
        printf("Bytes returned: %d", n);
        printf("\n%s\n", buffer);
    }
}

void voting(int sock)
{
    cin.ignore();
    int running = 1;
    while (running)
    {
        // Prompt user to select voting type
        printf("\n\nPlease type one of the following commands:\n\n1. Show candidates\n\n2. Show voting summary\n\n3. Vote\n\n4. BYE\n\n");
        int input;
        //cin.clear();
        cin >> input;
        printf("\nThe input is:\n%d\n", input);
        if (!isNums(to_string(input)))
        {
            input = 0;
        }

        // switch for determining which service to call
        switch (input)
        {
        case 1:
            printf("\nSelected show candidates!\n");
            showCandidates(sock);
            break;
        case 2:
            printf("\nSelected show voting summary!\n");
            showSummary(sock);
            break;
        case 3:
            printf("\nSelected voting!\n");
            castVote(sock);
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
    printf("\nLeaving voting microservice...\n");
}

void closeCon(int sock)
{
    // Format request
    string mssg = "4 1";
    // send SERVICE KEY + request type "4"(Close connection) to sock
    if (send(sock, mssg.c_str(), strlen(mssg.c_str()) + 1, 0) == -1)
    {
        printf("send() call failed\n");
        return;
    }

    printf("\nInput sent to server\n\n........................\n\n");
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

    //Set timeout of 3s
    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    setsockopt(localSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

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
    //cin.ignore();
    while (running)
    {
        // request user to select microservice
        //cin.clear();
        printf("\n\nPlease type one of the following commands:\n\n1. translator\n\n2. currency\n\n3. voting\n\n4. BYE\n\n");
        int input;
        cin >> input;
        printf("\nThe input is:\n%d\n", input);
        if (!isNums(to_string(input)))
        {
            input = 0;
        }

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
            voting(localSocket);
            break;
        case 4:
            printf("\nSelected EXIT!\n");
            closeCon(localSocket);
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