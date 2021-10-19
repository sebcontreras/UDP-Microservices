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
const string CURRENCIES[CURR_LENGTH] = {"USD", "CAD", "EUR", "GBP", "BTC", "ETH", "DOGE"};
const double CONVERSION_RATES[CURR_LENGTH] = {1, 1.24, 0.86, 0.73, 0.000016, 0.00027, 3.95};
const string CURRENCY_SYMBOL[CURR_LENGTH] = {"$", "$", "€", "£", "₿", "Ξ", "Đ"};

string getExchangeCurrency(string rawData)
{
    // rawData format: $10 CAD US
    printf("\nThe raw data:%s", rawData.c_str());
    string result;

    // Parse money
    int fSpace = rawData.find(" ");
    if (fSpace == -1)
    {
        result = "WRONG FORMAT! Please ensure your request is in the following format: $<amount> <source> <dest> ";
        printf("\n%s\n", result.c_str());
        return result;
    }
    string strMoney = rawData.substr(1, fSpace - 1);
    double money = std::stoi(strMoney);
    printf("\nmoney is: %f", money);

    // Parse source
    int sSpace = rawData.find(" ", fSpace + 1);
    if (sSpace == -1)
    {
        result = "WRONG FORMAT! Please ensure your request is in the following format: $<amount> <source> <dest> ";
        printf("\n%s\n", result.c_str());
        return result;
    }
    string src = rawData.substr(fSpace + 1, sSpace - fSpace - 1);
    printf("\nsrc is: %s", src.c_str());

    // Parse dest
    string dest = rawData.substr(sSpace + 1);
    printf("\ndest is: %s", dest.c_str());

    // if any of the data is missing, send try again message
    if (money <= 0 || src.empty() || dest.empty() || fSpace == -1 || sSpace == -1)
    {
        result = "WRONG FORMAT! Please ensure your request is in the following format: $<amount> <source> <dest> ";
        printf("\n%s\n", result.c_str());
        return result;
    }

    // Get matching source and dest index
    int srcIn, destIn;
    int found = 0;
    for (int i = 0; i < CURR_LENGTH; i++)
    {
        if (strncmp(src.c_str(), CURRENCIES[i].c_str(), strlen(CURRENCIES[i].c_str())) == 0)
        {
            srcIn = i;
            found++;
            printf("\nMatching src found: %s = %s\n", src.c_str(), CURRENCIES[i].c_str());
        }
        if (strncmp(dest.c_str(), CURRENCIES[i].c_str(), strlen(CURRENCIES[i].c_str())) == 0)
        {
            destIn = i;
            found++;
            printf("\nMatching dest found: %s = %s\n", dest.c_str(), CURRENCIES[i].c_str());
        }
        if (found == 2)
        {
            printf("\nMatches found, early break");
            break;
        }
    }

    // If match not found, return error message
    if (found < 2)
    {
        result = "Currency NOT found, supported currencies are; USD, CAD, EUR, GBP, BTC, ETH, DOGE";
        printf("\n%s\n", result.c_str());
        return result;
    }

    // Convert money to USD, then convert to dest
    // (USD/SRC) * DEST * money
    double conMoney = ((CONVERSION_RATES[0] / CONVERSION_RATES[srcIn]) * CONVERSION_RATES[destIn] * money);
    string finMoney = to_string(conMoney);
    printf("\n%f %s = %f %s", money, src.c_str(), conMoney, dest.c_str());

    // Prepare result string
    // $10 USD = *8.06 BPD
    result.append(CURRENCY_SYMBOL[srcIn]);
    result.append(strMoney);
    result.append(" ");
    result.append(CURRENCIES[srcIn]);
    result.append(" = ");
    result.append(CURRENCY_SYMBOL[destIn]);
    result.append(finMoney);
    result.append(" ");
    result.append(CURRENCIES[destIn]);

    printf("\nThe final result is:\n%s\n", result.c_str());

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
