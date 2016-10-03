#include "Http.h"
extern "C" {
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <sys/types.h>
}
#include <iostream>
#include <string>
#include <strings.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 10000

void Http::connect(std::string url) {
    extern int h_errno;
    const char *nameC = url.c_str();
    struct hostent host = *gethostbyname(nameC);
    if (h_errno != 0) std::cout<<"gethostbyname failure\n";

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    bcopy((char*)host.h_addr, (char*)&addr.sin_addr.s_addr, host.h_length);

    Http::sck = socket(AF_INET, SOCK_STREAM, 0);
    if (Http::sck == -1) std::cout<<"socket creation failure\n";
    if (::connect(Http::sck, (const struct sockaddr*)&addr, sizeof(addr)) != 0) {
        std::cout<<"connection failure\n";
    } else {
        //std::cout<<"successful connection\n";
    }
}

std::string Http::makeRequest(const char *req) {
    send(Http::sck, req, std::string(req).length(), 0);
    char* buffer = (char *)malloc(BUFFER_SIZE);
    memset(buffer, 0, BUFFER_SIZE);
    std::string response;
    sleep(1);
    read(Http::sck, buffer, BUFFER_SIZE-1);
    response = std::string(buffer);
    //std::cout<<response;
    return response;
}

std::string Http::dropHeaders(std::string doc) {
    int loc = doc.find("\r\n\r\n");
    std::string pure = doc.substr(loc + 4);
    return pure;
}

std::string Http::entryPoint(std::string url) {
    connect(url);
    std::string req = "GET /andreessen.html HTTP/1.1\nHost: "+url+"\n\n";
    std::string response = makeRequest(req.c_str());
    response = dropHeaders(response);
    return response;
}

