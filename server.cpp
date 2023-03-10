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

void Server::closeConnections(std::string m) {
    std::cout << m + std::to_string(WSAGetLastError()) << std::endl;
    closesocket(sockfd);
    closesocket(newsock);
    WSACleanup();
}

int Server::init() {

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        m = "WSAStartup failed with error: ";
        closeConnections(m);
        return 1;
    }
    
    // Create a new socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        m = "Socket create failed with error: ";
        closeConnections(m);
        return 1;
    }

    // Initialize server address struct
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    
    // Bind socket to server address
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        m = "Bind failed with error: ";
        closeConnections(m);
        return 1;
    }
    return 0;
}

int Server::run(){
    init();
    while (true) {
        // Listen for incoming connections
        if (listen(sockfd, SOMAXCONN) == SOCKET_ERROR) {
            m = "Listen failed with error: ";
            closeConnections(m);
            return 1;
        }

        // Accept incoming connections
        int clilen;
        clilen = sizeof(cli_addr);
        newsock = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsock == INVALID_SOCKET) {
            m = "Accept failed with error: ";
            closeConnections(m);
            return 1;
        }

        // Read data from client
        received = recv(newsock, buffer, sizeof(buffer), 0);
        printf("Bytes received: %i\n", received);
        std::cout << buffer;
        if (received == SOCKET_ERROR) {
            m = "Receive failed with error: ";
            closeConnections(m);
            return 1;
        }

        std::string req = buffer;

        // Send response
        std::string response = router.getResponse(req);
        std::cout << "\n" + response;

        long total_bytes = 0;
        while (total_bytes < response.size()) {
            sent = send(newsock, response.c_str(), response.size(), 0);
            if (sent < 0) {
                break;
            }
            total_bytes += sent;
        }
            if (sent == SOCKET_ERROR) {
            m = "Send failed with error: ";
            closeConnections(m);
            return 1;
        }
    }
    m = "Server shut down with exit code: ";
    closeConnections(m);
    return 0;
}
