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
    while (1)
    {
        // prompt user to enter word to translate or exit
        printf("\nPlease type the word that you would like to be translated,\nor type 'exit' to quit translator\n");

        // get word
        string input;
        getline(cin, input);

        // if word is "exit"
        if (strncmp(input.c_str(), "exit", 4) == 0)
        {
            printf("\nLeaving translator microservice...");
            return;
        }

        // Append input word to service key
        string mssg = "TRNS ";
        mssg.append(input);
        printf("\nThe mssg: %s", mssg);

        // send SERVICE KEY + word to sock
        if (send(localSocket, mssg.c_str(), strlen(mssg.c_str()) + 1, 0) == -1)
        {
            printf("send() call failed\n");
            return -1;
        }
        printf("\nInput sent to server\n\n........................\n\n");
        //      if (recv response):
        //          if success:
        //              print translated word
        //          if no good:
        //              print error message
        //      end
    }
}

int main()
{

    struct sockaddr_in serverAddress;
    int localSocket;
    char output[1024];

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
    while (1)
    {
        // Connection request
        if (connect(localSocket, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_in)) == -1)
        {
            printf("connect() call failed\n");
            return -1;
        }
        printf("Before while loop");
        //start while loop to check for input
        while (1)
        {
            // request user to select microservice
            printf("\n\nPlease type one of the following commands:\n\nADD ____\nDEL ____\nBYE\n\n");
            string input;
            getline(cin, input);
            printf("\nThe input is:\n%s\n", input.c_str());

            // switch for determining which service to call
            // 1: TRANSLATOR
            //      translator(localSocket)
            // 2: CURRENCY
            //      currency(localSocket)
            // 3: VOTING
            //      voting(localSocket)
            // 4: EXIT
            //      break out of loop
            // 5: DEFAULT
            //      Error message, invalid request
        }
    }

    // Connection termination
    close(localSocket);

    return 0;
}