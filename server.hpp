#ifndef SERVER_H
#define SERVER_H
#include <string>
#include <iostream>
#include <cstring>
#include <pthread.h>


class Server {
    private:
    static int port;
    int clilen;
    char buffer[1024];
    std::string m;

    public:
    int run();
    int init();

};

#endif