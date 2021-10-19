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

#define PORT 5454
#define MAXLINE 1024
#define CURR_LENGTH 7
#define MSG_CONFIRM 0 // TEMP FOR USE ON MAC

// Exchange rates taken on October 18, 2021 19:05:00
const string currencies[CURR_LENGTH] = {"US", "CAD", "EUR", "GBP", "BTC", "ETH", "DOGE"};
const double conversionRates[CURR_LENGTH] = {1, 1.24, 0.86, 0.73, 0.000016, 0.00027, 3.95};
const string currencySymbol[CURR_LENGTH] = {"$", "$", "€", "£", "₿", "Ξ", "Đ"};

string getExchangeCurrency(string rawData)
{
    // rawData format: $10 CAD US
    string result;

    // Parse money
    int fSpace = rawData.find(" ");
    string money = rawData.substr(1, fSpace - 1);
    printf("\nmoney is: %s", money.c_str());

    // Parse source
    int sSpace = rawData.find(" ", fSpace + 1);
    string src = rawData.substr(fSpace + 1, sSpace - fSpace - 1);
    printf("\nsrc is: %s", src.c_str());

    // Parse dest
    string dest = rawData.substr(sSpace + 1);
    printf("\ndest is: %s", dest.c_str());

    // Get matching source index
    // Get matching dest index

    // If match not found, return error message

    // Convert money to USD, then convert to dest
    // Prepare result string
    // return result

    // string fWord;
    // for (auto &pair : translations)
    // {
    //     if (strncmp(eWord.c_str(), pair[0].c_str(), strlen(pair[0].c_str())) == 0)
    //     {
    //         fWord = pair[1];
    //         printf("\nTranslation found!! %s in french is %s\n", eWord.c_str(), fWord.c_str());
    //         return fWord;
    //     }
    // }
    // printf("\nTranslation NOT found!!\n");
    // fWord = "Not in dictionary, please try another word";
    // return fWord;
    return result;
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
        printf("Request data: %s\n", buffer);

        // Look for word match and return translation or error message
        string reqDat(buffer);
        string response;
        response = getExchangeCurrency(reqDat);

        // Send french word to client
        sendto(sockfd, response.c_str(), strlen(response.c_str()),
               MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
               len);
        printf("\nSending \"%s\" to client\n", response.c_str());
    }
    close(sockfd);

    return 0;
}
