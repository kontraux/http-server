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

int Router::getMethod(std::string str) {
    std::string method = "";
    int i = 0;
    while (i < 3 && str[i] != ' ') {
        method += str[i];
        i++;
    }
    if (method.compare("GET") == 0) {
        return 200;
    } else {
        std::cout << method;
        return 405;
    }
}

std::string Router::parseHeaders(std::string req) {
    std::string path;
    std::stringstream ss(req);
    std::getline(ss, path);
    size_t start = path.find("GET ");
    size_t end = path.find(" HTTP/1.");
    if (start != std::string::npos && end != std::string::npos) {
        start += 4;
        return path.substr(start, end - start);
    } else {
        std::cout << "error parsing headers for path.\n";
        return "";
    }
}


std::string Router::timestamp() {
    time_t now = time(0);
    struct tm* gmt = gmtime(&now);
    static char date_str[64];
    strftime(date_str, sizeof(date_str), "%a, %d %b %Y %H:%M:%S GMT", gmt);
    return date_str;
}

std::string Router::writeHeaders(int status) {
    std::string server_status;
    static std::string headers;

    std::string date = "Date: " + timestamp() + "\r\n";
    std::string content_len = "Content-Length: " + std::to_string(writeBody().length()) + "\r\n";
    switch (status) {
        case 200:
            server_status = "HTTP/1.1 200 OK\r\n";
            break;
        case 404:
            server_status = "HTTP/1.1 404 NOT FOUND\r\n";
            break;
        case 405:
            server_status = "HTTP/1.1 405 METHOD NOT ALLOWED";
            break;
    }
    std::string server = "Server: http-server\r\n";
    std::string content_type = "Content-type: text/html\r\n";
    std::string connection_status = "Connection: closed\r\n";
    headers = server_status + content_len + date + server + content_type + connection_status;
    return headers;
}

std::string Router::writeBody() {
    std::string body = "<!DOCTYPE html><html><head><title>A page</title></head> <body><p>Hello placeholder</p></body></html>\r\n";
    return body;
}

std::string Router::getResponse(std::string req) {
    int status = getMethod(req);

    std::string path = parseHeaders(req);
    std::string response;
    
    response = writeHeaders(status) + "\r\n\r\n" + writeBody();
    std::cout << req;
    std::cout << response;
    return response;
}