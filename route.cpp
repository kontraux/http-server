#include <iostream>
#include "route.hpp"

std::string Route::getPath(std::string req) {
    std::cout << req;
    return req;
}

std::string Route::timestamp() {
    time_t now = time(0);
    struct tm* gmt = gmtime(&now);
    static char date_str[64];
    strftime(date_str, sizeof(date_str), "%a, %d %b %Y %H:%M:%S GMT", gmt);
    return date_str;
}

std::string Route::writeHeaders() {
    static std::string headers;
    std::string date = "Date: " + timestamp() + "\r\n";
    std::string content_len = "Content-Length: " + std::to_string(writeBody().length()) + "\r\n";
    std::string status = "HTTP/1.1 200 OK\r\n";
    std::string server = "Server: http-server\r\n";
    std::string content_type = "Content-type: text/html\r\n";
    std::string connection_status = "Connection: closed\r\n";
    headers = status + content_len + date + server + content_type + connection_status;
    return headers;
}

std::string Route::writeBody() {
    std::string body = "<!DOCTYPE html><html><head><title>A page</title></head> <body><p>Hello placeholder</p></body></html>\r\n";
    return body;
}