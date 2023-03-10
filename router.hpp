#ifndef ROUTE_H
#define ROUTE_H
#include <winsock2.h>
#include <string>

class Router {
    public:
    std::string timestamp();
    std::string writeHeaders(int _status);
    std::string writeBody();
    std::string parseHeaders(std::string req);
    std::string getResponse(std::string req);
    int sendFile(const char * filename, SOCKET socket);
};

#endif