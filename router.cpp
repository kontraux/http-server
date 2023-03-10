#include <iostream>
#include <fstream>
#include <sstream>
#include "router.hpp"

std::string routes[] = {"/", "/home"};

int Router::sendFile(const char* filename, SOCKET socket) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    char* fileBuffer = new char[fileSize];

    if (!file.read(fileBuffer, fileSize)) {
        std::cerr << "Error reading file" << std::endl;
        delete[] fileBuffer;
        return 1;
    }

    int bytesSent = send(socket, fileBuffer, fileSize, 0);
    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "Error sending data: " << WSAGetLastError() << std::endl;
        delete[] fileBuffer;
        return 1;
    }

    delete[] fileBuffer;
    return 0;
}

std::string Router::parseHeaders(std::string req) {
    std::string path;
    std::stringstream ss(req);
    std::getline(ss, path);
    size_t start = path.find("GET ") + 4;
    size_t end = path.find(" HTTP/1.");
    return path.substr(start, end - start);
}


std::string Router::timestamp() {
    time_t now = time(0);
    struct tm* gmt = gmtime(&now);
    static char date_str[64];
    strftime(date_str, sizeof(date_str), "%a, %d %b %Y %H:%M:%S GMT", gmt);
    return date_str;
}

std::string Router::writeHeaders(int _status) {
    std::string status;
    static std::string headers;

    std::string date = "Date: " + timestamp() + "\r\n";
    std::string content_len = "Content-Length: " + std::to_string(writeBody().length()) + "\r\n";
    if (_status == 0) { 
        status = "HTTP/1.1 200 OK\r\n"; } else {
        status = "HTTP/1.1 404 NOT FOUND\r\n";
    }
    std::string server = "Server: http-server\r\n";
    std::string content_type = "Content-type: text/html\r\n";
    std::string connection_status = "Connection: closed\r\n";
    headers = status + content_len + date + server + content_type + connection_status;
    return headers;
}

std::string Router::writeBody() {
    std::string body = "<!DOCTYPE html><html><head><title>A page</title></head> <body><p>Hello placeholder</p></body></html>\r\n";
    return body;
}

std::string Router::getResponse(std::string req) {
    std::string path = parseHeaders(req);
    std::string response;
    for (int i = 0; i < sizeof(routes); i++) {
        if (path == routes[i]) {
            response = writeHeaders(0) + "\r\n\r\n" + writeBody();
            return response;
        } else {
            response = writeHeaders(1);
            return response;
        }
    }
    return response;
}