#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>

using namespace std;

#define PORT 6767
#define MAXLINE 2048
#define CAND_LENGTH 5
// #define MSG_CONFIRM 0 // TEMP FOR USE ON MAC

const int canID[CAND_LENGTH] = {5678, 4444, 8981, 2442, 1298};
const string canName[CAND_LENGTH] = {"Yuto", "Milton", "Shane", "Max", "Gifted"};

string showSummary(int votes[], int &hasVoted)
{
    string resp;
    if (hasVoted)
    {
        resp = "ID             Name           Votes";
        for (int i = 0; i < CAND_LENGTH; i++)
        {
            resp.append("\n");
            resp.append(to_string(canID[i]));
            resp.append(" --------- ");
            resp.append(canName[i]);
            resp.append(" --------- ");
            resp.append(to_string(votes[i]));
        }
    }
    else
    {
        resp = "You need to vote first before you can see the results!";
    }
    return resp;
}

string showCandidates()
{
    string resp = "ID             Name";
    for (int i = 0; i < CAND_LENGTH; i++)
    {
        resp.append("\n");
        resp.append(to_string(canID[i]));
        resp.append(" --------- ");
        resp.append(canName[i]);
    }
    return resp;
}

string getKey()
{
    int r = rand() % 10;
    string key = to_string(r);
    return key;
}

string castVote(int voteCount[], string data, int &hasVoted)
{
    // data is: 258582 8
    string resp = "VOTE FAIL";

    // Get encrypted vote
    int endEn = data.find(" ");
    string encVoteStr = data.substr(0, endEn);
    int encVote = std::stoi(encVoteStr);

    // Get key
    string keyStr = data.substr(endEn + 1, 1);
    int key = std::stoi(keyStr);

    // Decrypt vote
    int decVote = encVote / key;

    // Update voteCount
    for (int i = 0; i < CAND_LENGTH; i++)
    {
        if (canID[i] == decVote)
        {
            voteCount[i]++;
            hasVoted = 1;
            resp = "VOTE SUCCESS!";
            printf("\nVote count updated!");
        }
    }

    return resp;
}
// Driver code
int main()
{
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;
    int voteCount[CAND_LENGTH] = {142, 23, 207, 52, 10};
    int hasVoted = 0;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr,
             sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listening
    char buffer1[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &servaddr.sin_addr, buffer1, sizeof(buffer1));
    printf("Server running at IP: %s on port: %d\n", buffer1, PORT);

    int len, n;

    len = sizeof(cliaddr); //len is value/resuslt

    while (1)
    {
        // Receive voting request from server
        n = recvfrom(sockfd, (char *)buffer, MAXLINE - 1,
                     MSG_WAITALL, (struct sockaddr *)&cliaddr,
                     (socklen_t *)&len);
        buffer[n] = '\0';

        // Get service ID
        int serv;
        char tempServ[2];
        strncpy(tempServ, buffer, 1);
        tempServ[1] = '\0';
        serv = std::stoi(tempServ);

        // switch for determining which service to call
        string resp;
        switch (serv)
        {
        case 1:
            printf("\nSelected show candidates!\n");
            resp = showCandidates();
            break;
        case 2:
            printf("\nSelected show voting summary!\n");
            resp = showSummary(voteCount, hasVoted);
            break;
        case 3:
            printf("\nSelected get encryption key\n");
            resp = getKey();
            break;
        case 4:
        {
            printf("\nSelected cast vote!\n");
            char tempData[n - 2];
            strncpy(tempData, buffer + 2, n - 2);
            string data(tempData);
            resp = "Sorry, you can only vote once";
            if (!hasVoted)
            {
                resp = castVote(voteCount, data, hasVoted); //returns confirmation
            }
            break;
        }
        case 5:
            printf("\nSelected EXIT!\n");
            break;
        default:
            printf("\nInvalid request....\n");
            break;
        }

        // Send resp to client
        sendto(sockfd, resp.c_str(), strlen(resp.c_str()),
               MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
               len);
        printf("\nSending to server:\n%s\n", resp.c_str());
    }
    close(sockfd);

    return 0;
}
