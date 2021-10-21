#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;

#define PORT 6767
#define MAXLINE 1024
#define CAND_LENGTH 5
#define MSG_CONFIRM 0 // TEMP FOR USE ON MAC

const int canID[CAND_LENGTH] = {5678, 4444, 8981, 2442, 1298};
const string canName[CAND_LENGTH] = {"Yuto", "Milton", "Shane", "Max", "Gifted"};

string showSummary(int *votes)
{
    string resp = "ID             Name           Votes";
    for (int i = 0; i < CAND_LENGTH; i++)
    {
        resp.append("\n");
        resp.append(to_string(canID[i]));
        resp.append(" --------- ");
        resp.append(canName[i]);
        resp.append(" --------- ");
        resp.append(to_string(votes[i]));
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

// Driver code
int main()
{
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;
    int voteCount[CAND_LENGTH] = {142, 23, 207, 52, 10};

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
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&cliaddr,
                     (socklen_t *)&len);
        buffer[n] = '\0';
        printf("Request from server: %s\n", buffer);

        // Get service ID
        int serv;
        char tempServ[2];
        strncpy(tempServ, buffer, 1);
        tempServ[1] = '\0';
        printf("\ntempServ is: %s", tempServ);
        serv = std::stoi(tempServ);
        printf("\nThe command is: %d", serv);

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
            resp = showSummary(voteCount);
            break;
        case 3:
            printf("\nSelected get encryption key\n");
            //getKey(sock);
            break;
        case 4:
            printf("\nSelected cast vote!\n");
            //castVote(sock); //returns confirmation
            break;
        case 5:
            printf("\nSelected EXIT!\n");
            //running = 0;
            break;
        default:
            printf("\nInvalid request....\n");
            break;
        }

        // Send french word to client
        sendto(sockfd, resp.c_str(), strlen(resp.c_str()),
               MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
               len);
        printf("\nSending to server:\n%s\n", resp.c_str());
    }
    close(sockfd);

    return 0;
}