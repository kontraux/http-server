#include "server.h"
#include <stdio.h>
#include <time.h>
#include <assert.h>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

// To do: test on Linux
#ifdef __linux__
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif


char *get_time() {
    static char s[64];
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    size_t ret = strftime(s, sizeof(s), "%c", tm);
    assert(ret);
    return s;
}

char *get_res() {
    static char response[1024];
    time_t now = time(0);
    struct tm* gmt = gmtime(&now);
    char date_str[64];
    strftime(date_str, sizeof(date_str), "%a, %d %b %Y %H:%M:%S GMT", gmt);

    char body[] = "Hello placeholder.";
    int body_len = strlen(body);
    char content_length_str[16];
    sprintf(content_length_str, "%d", body_len);
    sprintf(response, 
    "HTTP/1.1 200 OK\r\nDate: %s\r\nServer: http-server\r\nContent-type: text/html\r\nContent-Length: %s\r\nConnection: Closed\r\n\r\n%s", 
    date_str, content_length_str, body);
    return response;
}

int server_run() {

    WSADATA wsaData;
    SOCKET sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    int portno = 8080;
    int clilen;
    char buffer[256];
    int n;

    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    if (listen(sockfd, SOMAXCONN) == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    n = recv(newsockfd, buffer, sizeof(buffer), 0);
    printf(buffer);
    if (n == SOCKET_ERROR) {
        printf("recv failed with error: %d\n", WSAGetLastError());
        closesocket(newsockfd);
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    char *response = get_res();
    printf(response);
    int reslen = strlen(response);
    response[reslen] = '\0';
    n = send(newsockfd, response, reslen + 1, 0);
    if (n == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(newsockfd);
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    closesocket(newsockfd);
    closesocket(sockfd);
    WSACleanup();

return 0;
}