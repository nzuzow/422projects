// Modified from CSE422 FS09
// Modified from CSE422 FS12
// Modified from CSE422 FS13
//
// HTTP_Response - Class representing an HTTP response message.  May be used
// both to parse existing HTTP response into a comprehensible object, and to
// construct new responses from scratch and print them out to a text string.
// Makes no attempt to handle the body of the response -- only the response code
// and the headers will be captured.
//
// If you're planning on servicing GET and HEAD requests only, you can use
// the Create_standard_response() method to have a lot of headers automatically
// set up for you.  The HTTP specification mandates these headers, and some
// clients may expect them.
//
// Also see the HTTP_Message class for methods that can be used to query and
// set the response headers.

#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include "HTTP_Message.h"
#include "TCP_Socket.h"
#include <string>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <sstream>


class HTTP_Response : public HTTP_Message
{
public:
    // Purpose: Constructs a new HTTP_Response. Note that nothing is done 
    //          to check the validity of the arguments -- make sure you 
    //          trust your input and/or yourself.
    // Receive: code - The code representing the response status (e.g. 
    //                 200, 403).
    //          status_desc - A one-line textual description of the 
    //                        response.
    //          version - The HTTP version used to transmit the response.
    HTTP_Response(unsigned code = 0, const std::string& status_desc = "",
        const std::string& version = "HTTP/1.1", const std::string &data = "");

    virtual ~HTTP_Response();

    // Purpose: Parse the response from server to construct a HTTP_Response
    //          Object. Check if the response is formatted correctly.
    // Receive: data - the received data piece
    //          length-  the length of the data
    //          NOTE: People parse the response differently. The way they
    //                slice the header varies as well. In this implementation
    //                The header MUST END WITH \r\n\r\n.
    // Return:  a pointer to an HTTP_Response object, if this data is good.
    //          NULL otherwise
    static HTTP_Response *parse(const char* data, unsigned length);

    // Purpose: Constructs a new HTTP_Response with some mandatory header 
    //          fields convenienty set for you (unlike the constructor, 
    //          which sets no fields for you at all). Assumes that you 
    //          will be sending back some kind of message body, and that 
    //          the message body will be sent verbatim (i.e. not compressed 
    //          ).  Also assumes that the connection will be closed 
    //          immediately after the send ends.
    // Receive: content_length - The length of the message body that will 
    //                           be sent following this response.
    //          code - The code representing the response status (e.g. 500).
    //          status_desc - A short description of the response code's 
    //                        meaning.
    //          version - The HTTP version used to transmit the response.
    //
    // Return:  An HTTP_Response created for the given input, containing
    //          all of the mandatory headers.
    static HTTP_Response* create_standard_response(unsigned content_length,
        unsigned code = 0,
        const std::string& status_desc = "",
        const std::string& version = "HTTP/1.1");


    // Purpose: receive a pieve of data from the socket sock. Slice the
    //          received data into two pars, header and the body
    // Receive: sock - the TCP socket to receive from
    //          header - the string to hold the incoming header string
    //          data - the string to hold the incoming body string,
    //                 probably just partial
    // Return:  none
    void receive_header(TCP_Socket &sock, std::string &header, std::string &data);

    // Purpose: receive the desired number of bytes of data from the socket
    // Receive: sock - the TCP socket to receive from
    //          data - the string to hold the incoming body string
    //          bytes_left - the number of bytes to receive
    // Return:  the number of bytes received.
    int receive_data(TCP_Socket &sock, std::string &data, int bytes_left = BUFFER_SIZE);

    // Purpose: receive data char by char, until a newline char is found
    // Receive: sock - the TCP socket to receive from
    //          data - the string to hold incoming data
    // Return:  the number of bytes received
    int receive_line(TCP_Socket &sock, std::string &data);

    // Purpose: from the header, extract the "Content-Length"
    // Receive: none
    // Return:  the content length as int
    const int get_content_length() const;

    // Purpose: Looks up the version of the HTTP response (e.g. HTTP/1.1).
    // Receive: none
    // Returns: the response's HTTP version.
    const std::string& get_version() const;

    // Purpose: Looks up the status code of the HTTP response (e.g. 404, 
    //          500).
    // Receive: none
    // Return:  The response's status code.
    unsigned get_status_code() const;

    // Purpose: Looks up the description of the response (e.g. "OK").
    // Receive: none
    // Return:  The response's associated status_desc string.
    const std::string& get_status_desc() const;

    // Purpose: Looks up the content length of the response body
    // Receive: none
    // Reutrn:  the content length
    const int get_content_len() const;

    // Purpose: Looks up the content, the response body
    // Receive: none
    // Return:  the content as a string
    const std::string &get_content() const;

    // Purpose: Looks up if the response is chunked encoding transfer
    // Receive: none
    // Return:  true if this response is chunked, false otherwise
    const bool is_chunked() const;

    // Purpose: prints the response object to a text string, suitable 
    //          for sending to an HTTP client.  Includes the terminating 
    //          blank line and all response headers.
    // Recieve: output_string - Will be set to the response text.
    // Return:  none
    void print(std::string& output_string) const;

    // Purpose: prints the response object to a text string, suitable for 
    //          sending to an HTTP client.  Includes the terminating blank 
    //          line and all response headers.
    //
    // Receive: output_buffer - The text buffer into which the response 
    //                          should be printed.  Will be null-terminated.
    //          buffer_length - The number of characters available for writing
    //                          in the buffer.  printing stops after this 
    //                          many characters have been written.
    // Return:  none
    void print(char* output_buffer, unsigned buffer_length) const;


    // Purpose: Sets the HTTP version of the response (e.g. HTTP/1.1).
    // Receive: version -  The version to set.
    // Return:  none
    void set_version(const std::string& version);

    // Purpose: Sets the status code to indicate in the response.
    // Receive: code - The HTTP status code to set.
    // Return:  none
    void set_status_code(const unsigned code);

    // Purpose: Sets the status_desc for the given status code being sent.
    // Receive: status_desc - The text string to set as the response's 
    //                        description.
    // Return:  none
    void set_status_desc(const std::string& status_desc);

    // Purpose: Send this response to this TCP socket sock
    // Receive: sock - the socket to send to
    // Return:  none
    void send(TCP_Socket &sock);

    void send_no_error(TCP_Socket &sock);

    std::string content;
private:
    // Purpose: private function that builds a data header field that
    //          matches the spec of HTTP
    // Receive: none
    // Return:  none
    void build_status();

    // Purpose: private function that creates a current time for 
    //          time-stampign this message
    // Receive: none
    // Return:  the string for currnet time
    std::string build_time();

    unsigned status_code;
    std::string version;
    std::string status_desc;
    bool chunked;
    int content_len;
};

#endif //ndef _HTTP_RESPONSE_H_
