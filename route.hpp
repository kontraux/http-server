#ifndef ROUTE_H
#define ROUTE_H
#include <string>

class Route {
    public:
    std::string timestamp();
    std::string writeHeaders();
    std::string writeBody();
    std::string getPath(std::string req);
};

#endif