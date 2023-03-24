#include "server.hpp"
#include "router.hpp"

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

WSADATA wsaData;
SOCKET newsock, sockfd;
Router router;
struct sockaddr_in serv_addr, cli_addr;
int Server::port = 8080;


DWORD WINAPI handleConnection(LPVOID lpParam) {
    int newsock = *((int*) lpParam);
    int sent;
    int received;
    char buffer[8192];
    memset(buffer, 0, sizeof(buffer));

    received = recv(newsock, buffer, sizeof(buffer), 0);
    printf("Bytes received: %i\n", received);
    if (received == SOCKET_ERROR) {
        std::cout << "Receive failed with error: " << std::endl;
        closesocket(sockfd);
        closesocket(newsock);
        WSACleanup();
        return 1;
    }

    std::string req = buffer;
    std::string response = router.getResponse(req);
    std::cout << req << "\n" << response;

    long total_bytes = 0;
    while (total_bytes < response.size()) {
        int sent = send(newsock, response.c_str(), response.size(), 0);
        if (sent < 0) {
            break;
        }
        total_bytes += sent;
    }
        if (sent == SOCKET_ERROR) {
        std::cout << "Send failed with error: " << WSAGetLastError();
        closesocket(sockfd);
        closesocket(newsock);
        WSACleanup();
        return 1;
    }

    return 0;
}

int Server::init() {

    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cout << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
        closesocket(sockfd);
        closesocket(newsock);
        WSACleanup();
        return 1;
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    return 0;
}

int Server::run(){
    init();
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        std::cout << "Socket create failed with error: " << WSAGetLastError() << std::endl;
        closesocket(sockfd);
        closesocket(newsock);
        WSACleanup();
        return 1;
    }

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        std::cout << "Bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(sockfd);
        closesocket(newsock);
        WSACleanup();
        return 1;
    }

    if (listen(sockfd, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(sockfd);
        closesocket(newsock);
        WSACleanup();
        return 1;
    }

    while (true) {

        int clilen = sizeof(cli_addr);
        newsock = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsock == INVALID_SOCKET) {
            std::cout << "Accept failed with error: " << WSAGetLastError() << std::endl;
            closesocket(sockfd);
            closesocket(newsock);
            WSACleanup();
            return 1;
        }

        HANDLE thread_handle = CreateThread(nullptr, 0, handleConnection, &newsock, 0, nullptr);
        if (thread_handle == nullptr) {
            std::cerr << "Error creating thread\n";
            closesocket(sockfd);
            closesocket(newsock);
            WSACleanup();
            continue;
        }
    }

    std::cout << "Server shut down with exit code: " << WSAGetLastError();
    closesocket(sockfd);
    closesocket(newsock);
    WSACleanup();
    return 0;
}
