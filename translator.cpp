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

#define PORT 44123
#define MAXLINE 1024
#define TRANS_LENGTH 5
// #define MSG_CONFIRM 0 // TEMP FOR USE ON MAC

const string translations[TRANS_LENGTH][2] = {{"hello", "bonjour"}, {"world", "monde"}, {"bye", "au revoir"}, {"yes", "oui"}, {"no", "non"}};

string getTranslation(string eWord)
{
    string fWord;
    for (auto &pair : translations)
    {
        if (strncmp(eWord.c_str(), pair[0].c_str(), strlen(pair[0].c_str())) == 0)
        {
            fWord = pair[1];
            return fWord;
        }
    }
    printf("\nTranslation NOT found!!\n");
    fWord = "Not in dictionary, please try another word";
    return fWord;
}

// Driver code
int main()
{
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;

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
        // Receive english word from client
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&cliaddr,
                     (socklen_t *)&len);
        buffer[n] = '\0';

        // Look for word match and return translation or error message
        string eWord(buffer);
        string fWord;
        fWord = getTranslation(eWord);

        // Send french word to client
        sendto(sockfd, fWord.c_str(), strlen(fWord.c_str()),
               MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
               len);
        printf("\nSending \"%s\" to client\n", fWord.c_str());
    }
    close(sockfd);

    return 0;
}
