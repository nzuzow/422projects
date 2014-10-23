// Modified from CSE422 FS09
// Modified from CSE422 FS12
// Modified from CSE422 FS13
//
// HTTP_Request - Class representing an HTTP request message.  May be used both
// to parse an existing HTTP request into a comprehensible object, and to
// construct new requests from scratch and print them out to a text string.
// Makes no attempt to handle the body of the request -- only the request line
// and the headers will be captured.
//
// If all you want to do is download a file, call create_GET_request() with
// the path of the file that you want to download, and then call set_host()
// on the returned object with the hostname of the server from which you'll
// be downloading.  You should then be able to Print() the request out to a
// character buffer to get something that the server will accept.
//
// Also see the HTTP_Message class for methods that can be used to query and
// set the request's headers.

#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include "HTTP_Message.h"
#include "TCP_Socket.h"
#include <string>


class HTTP_Request : public HTTP_Message
{
public:
    // Purpose: constructor, constructs a new HTTP_Request. Note that 
    //          nothing is done to check the validity of the arguments 
    //          -- make sure you trust your input.
    // Receive: method - The action being requested (e.g. GET, POST, etc).
    //          url_str - The URL of the resource to which the request 
    //                    applies. In most cases, this will typically 
    //                    just be the path of the resource on the server
    //                    (e.g. /somedir/something.txt).
    //          version - The HTTP version of the client making the 
    //                    request. Default is HTTP 1.1 (which ought to 
    //                    be what you support).
    // Return:  none
    HTTP_Request(const std::string& method = "",
        const std::string& url_str = "",
        const std::string& version = "HTTP/1.1");

    virtual ~HTTP_Request();


    // Purpose: constructs an HTTP_Request object corresponding to the 
    //          actual request text in the given buffer. Use this if 
    //          you've received a request and want to know what it's 
    //          asking.
    // Receive: data - The text buffer in which the request is stored.
    //          length - The length of the request data, in bytes.
    //
    // Return:  an HTTP_Request parsed from the request text. If 
    //          parsing fails, a NULL pointer will be returned instead.
    static HTTP_Request* parse(const char* data, unsigned length);

    // Purpose: constructs a new HTTP GET request, with a header or 
    //          two set to make it more likely that the server will 
    //          return an easy-to-handle result.
    // Receive: url - The URL of the resource to get.
    //          version - The HTTP version to associate with the 
    //                    request.
    // Return:  A new HTTP_Request object for the GET request.
    static HTTP_Request* create_GET_request(const std::string& url ="",
        const std::string& version = "HTTP/1.1");

    // Purpose: send this request to the socket sock
    // Receive: the socket we want to send to
    // Return:  none
        void send(TCP_Socket &);

    // Purpose: receive data from the socket sock and create an 
    //          HTTP_Request object by parsing that piece of data.
    // Receive: the socket we want to receive from
    // Return:  receive a piece of data from the socket, until a line 
    //          with only CLRF is found, which means it is the end of 
    //          the header. Create an HTTP_Request object from that 
    //          header.
        static HTTP_Request *receive(TCP_Socket &);

    // Purpose: Looks up the method of the request (e.g. GET, PUT, 
    //          DELETE).
    // Receive: none
    // Return:  The request method.
    const std::string& get_method() const;

    // Purpose: Looks up the path targeted by the request 
    //          (e.g. /stuff.txt).
    // Receive: none
    // Return:  The request's path.
    const std::string& get_path() const;

    const std::string get_url() const;


    // Purpose: Looks up the HTTP version of the requesting client 
    //          (e.g. HTTP/1.1).
    // Receive: none
    // Return:  The request's HTTP version.
    const std::string& get_version() const;

    // Purpose: Looks up the host for which the request is intended, 
    //          from the request's Host header.
    // Receive: out_host - Will be set to the request's target host.  
    //          If the host has not yet been entered, it will be set 
    //          to a blank string.
    // Return:  none.
    void get_host(std::string& out_host) const;


    // Purpose: Prints the request object to a text string, suitable 
    //          for transmission to an HTTP server. Includes the 
    //          terminating blank line and all request headers.
    // Receive: output_string - Will be set to the request text.
    // Return:  none
    void print(std::string& output_string) const;

    // Purpose: Prints the request object to a text string, suitable 
    //          for transmission to an HTTP server.  Includes the 
    //          terminating blank line and all request headers.
    // Receive: output_buffer - The text buffer into which the request 
    //                          should be printed. Will be null-terminated.
    //          buffer_length - The number of characters available for 
    //                          writing in the buffer. Printing stops 
    //                          after this many characters have been
    //                          written.
    // Return:  none
    void print(char* output_buffer, unsigned buffer_length) const;


    // Purpose: Sets the method of the HTTP request (e.g. GET, PUT, 
    //          DELETE).
    // Receive: method - The method to set for the request.
    // Return:  none
    void set_method(const std::string& method);

    // Purpose: Sets the path that the request should target (e.g. 
    //          /stuff.txt).
    // Receive: path - The path to set for the request.
    // Return:  none
    void set_path(const std::string& url);

    // Purpose: Sets the HTTP version supported by the request's client.
    // Receive: version - The HTTP version the request should indicate.
    // Return:  none
    void set_version(const std::string& version);

    // Purpose: Sets the host for which the request is intended, into 
    //          the request's Host header.
    // Receive: host - The host to set for the request.
    // Return:  none
    void set_host(const std::string& host);


private:
    std::string method;
    std::string url;
    std::string version;
};

#endif //ndef _HTTP_REQUEST_H_
