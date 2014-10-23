// Modified from CSE422 FS09
// Modified from CSE422 FS12
// Modified from CSE422 FS13

#ifndef _PROXYWORKER_H_
#define _PROXYWORKER_H_

#include <iostream>
#include <string>

#include "HTTP_Response.h"
#include "HTTP_Request.h"
#include "TCP_Socket.h"
#include "URL.h"


/*
 * Purpose: Acts as a single worker to handle a request from a proxy. It 
 *          does not cache anything in this lab. It just pass the requst
 *          to the server and return the response to the client.
*/
class Proxy_Worker {
private:
    URL                *server_url;      // Server's URL, obtained from
                                         // each request.
    unsigned short int port;             // For a full blown proxy

    TCP_Socket         *client_sock;     // The socket for client
    TCP_Socket         server_sock;
    HTTP_Request       *client_request;  // Obj to handle client request
    HTTP_Response      *server_response; // Obj to handle server response

    // Details for these functions in Proxy_Worker.cc
    bool get_request();
    bool check_request();
    bool forward_request_get_response();
    bool return_response();
    bool proxy_response(int);
    bool subliminal_response(const std::string url, int duration = 0);
    int get_chunk_size(std::string &data);

public:
    Proxy_Worker(TCP_Socket *);
    ~Proxy_Worker();

    void handle_request();
};

#endif
