#ifndef SERVER_H
#define SERVER_H
#include <string>
#include <iostream>
#include <cstring>


class Server {
    private:
    static int port;
    int clilen;
    char buffer[1024];
    int received;
    int sent;
    std::string m;

    public:
    int run();
    int init();
    void closeConnections(std::string m);
};

#endif