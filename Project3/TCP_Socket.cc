// Modified from CSE422 FS09
// Modified from CSE422 FS12
// Modified from CSE422 FS14

#include "TCP_Socket.h"
#include <sstream>

#define BUFFER_SIZE 40960

using namespace std;

/*
 * Purpose: Default constructor sets the socket to -1.
 * Receive: None
 * Return:  None
 */
TCP_Socket::TCP_Socket() {
    sock = -1;
}

/*
 * Purpose: Destructor closes the socket by calling the close function
 * Receive: None
 * Return:  None
*/
TCP_Socket::~TCP_Socket() {
    Close();
}

/*
 * Purpose: Private method that handles creating a socket, despite what
 *          Connect method is used
 * Receive: None
 * Return:  None
 */
void TCP_Socket::create_socket()
{
    //close the socket if it's already open
    Close();

    //first try to make the TCP socket
    cout << "In create_socket before socket" << endl;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	cout << "The sock is: " << sock << endl;
    if(sock < 0)
    {
        throw string("TCP_Socket Exception: Unable to create socket");
    }
}

/*
 * Purpose: Called to initiate a Connection to a server by a client
 * Receive: server_name is the hostname of the server, and server_port is
 *          the port number on the server
 * Return:  None
*/
void TCP_Socket::Connect(string &server_name, int server_port) {
    hostent *host;

    //create the socket
    create_socket();

    //convert the server name to a valid inet address
    if ((host = gethostbyname(server_name.c_str())) == NULL)
    {
        throw string("TCP_Socket Exception: could not resolve hostname");
    }

    //make sure it's zero to start
    memset(&server_addr, 0, sizeof(server_addr));
    //designate it as part of the Internet address family
    server_addr.sin_family = AF_INET;
    //specify the port
    server_addr.sin_port = htons(server_port);
    //specify the server IP address in network byte order
    server_addr.sin_addr.s_addr = *(unsigned long*) host->h_addr;

    //now actually try to Connect
    if(connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        throw string("TCP_Socket Exception: Connect failed");
    }
}

/*
 * Purpose: Called to initiate a Connection to a server by a client
 * Receive: host is the address of the server, and server_port is the
 *          port number on the server
 * Return:  None
 */
void TCP_Socket::Connect(hostent *host, int server_port) {
    //create the socket
    create_socket();

    //make sure it's zero to start
    memset(&server_addr, 0, sizeof(server_addr));
    //designate it as part of the Internet address family
    server_addr.sin_family = AF_INET;
    //specify the port
    server_addr.sin_port = htons(server_port);
    //specify the server IP address in network byte order
    memcpy(&server_addr.sin_addr, host->h_addr, host->h_length);

    //now actually try to Connect
    if(connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        throw string("TCP_Socket Exception: Connect failed");
    }
}

/*
 * Purpose: called to initiate a Connection to a server and port given
 *          by the URI
 * Reveive: uri is the URI object holding the server name and port, and
 *          resource name
 * Return:  None
 */
void TCP_Socket::Connect(const URI &uri) {
    hostent *hp = gethostbyname(uri.Get_host().c_str());

    if(hp == NULL)
    {
        throw string("TCP_Socket Exception: Unable to resolve URI");
    }
    else
    {
        // If the port is not defined, connect to 80
        if(uri.Is_port_defined())
        {
            Connect(hp, uri.Get_port());
        }
        else
        {
            Connect(hp, 80);
        }
    }
}

/*
 * Purpose: Creates and binds to a socket in a server process
 * Receive: server_port is the port number used for the service
 * Return:  None
 */
void TCP_Socket::Bind(int server_port) {
    //create the socket
    create_socket();

    //make sure it's zero to start
    memset(&server_addr, 0, sizeof(server_addr));
    //designate it as part of the Internet address family
    server_addr.sin_family = AF_INET;
    //specify the port
    server_addr.sin_port = htons(server_port);
    //specify the server IP address in network byte order
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sock, (sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        throw string("TCP_Socket Exception: could not bind to interface");
    }
}

/*
 * Purpose: Initiates listening on a bound socket
 * Receive: None
 * Return:  None
 */
void TCP_Socket::Listen() {
    // listen on socket sock
    // report error when fail
    if(listen(sock, 1) < 0)
    {
       throw string("TCP_Socket Exception: listen call failed");
    }

    socklen_t server_addr_len = sizeof(server_addr);
    if(getsockname(sock, (sockaddr *) &server_addr,
                   &server_addr_len) < 0)
    {
        cout << "TCP_Socket Exception: Unable to obtain TCP socket information."
             << endl;
    }
}

/*
 * Purpose: accepts a Connection waiting on a bound port
 * Receive: data_sock is the TCP_Socket object that will hold the new i
 *          Connection to the client
 * Return:  true if the accept was succesful, false otherwise
 */
bool TCP_Socket::Accept(TCP_Socket &data_sock) {
    int new_sock;
    socklen_t sin_size;

    sin_size = sizeof(struct sockaddr_in);
    // waiting for new incoming connection
    if((new_sock = accept(sock, (struct sockaddr *) &(data_sock.server_addr), &sin_size)) < 0)
    {
        throw string("TCP_Socket Exception: could not accept incoming Connection");
    }

    data_sock.sock = new_sock;
    return true;
}

/*
 * Purpose: Alternative form of accept that creates a new socket object
 * Receive: None
 * Return:  Returns the TCP_Socket object pointer for the new data socket
 */
TCP_Socket * TCP_Socket::Accept() {
    int new_sock;
    socklen_t sin_size;
    // The new TCP_Socket object to hold the connection socket
    TCP_Socket *data_sock = new TCP_Socket();

    sin_size = sizeof(struct sockaddr_in);
    if((new_sock = accept(sock, (struct sockaddr *) &(data_sock->server_addr), &sin_size)) < 0)
    {
        delete data_sock;
        throw string("TCP_Socket Exception: could not accept incoming Connection");
    }

    data_sock->sock = new_sock;
    return data_sock;
}

/*
 * Purpose: closes an open socket, only if it's open
 * Receive: None
 * Return:  None
*/
int TCP_Socket::Close() {
    if(sock != -1) // If this socket is in use
    {
        if(close(sock) < 0)
        {
            return -1;
        }
    }
    sock = -1;
    return 0;
}

/*
 * Purpose: Writes a string with a given length on this socket
 * Receive: data holds the string data to be written to the socket
 * Return:  The number of bytes actually written (should always be the
 *          length of the string)
 */
int TCP_Socket::write_string(string &data) {
    int l = 0;

    if ((l = write(sock, (void *)data.data(), data.size())) < 0)
    {
        throw string("TCP_Socket Exception: error sending data");
    }

    return l;
}

/*
 * Purpose: Read a string of a given length
 * Receive: data will hold the string data read from the socket
 * Return:  The number of bytes actually read (not counting the NULL
 *          appended to the string)
 */
int TCP_Socket::read_string(string &data) {
    int l;

    if((l = read(sock, (void *)data.data(), data.size())) < 0)
    {
        throw string("TCP_Socket Exception: error reading data from socket");
    }
    data = data.substr(0, l);
    data += '\0';

    return l;
}

/*
 * Purpose: Read n bytes from the socket
 * Receive: vptr: the pointer to the buffer that will be used to hold
 *          the data
 *          n: the number of bytes to be read
 * Return:  the number of bytes read
 */
int TCP_Socket::read_n_bytes(void *vptr, int n)
{
    size_t  n_left;
    ssize_t n_read;
    char    *ptr;

    ptr = (char *) vptr;
    n_left = n;

    while(n_left > 0) // keeps reading until n is satisfied
    {
		cout << "In while in read n bytes" << endl;
		cout << "n_left is: " << n_left << endl;
		
        if((n_read = read(sock, ptr, n_left)) < 0)
        {
            return -1;
        }
        else if(n_read == 0)
        {
            break;
        }
        n_left -= n_read;
        ptr    += n_read;
    }

    return (n - n_left);
}

/*
 * Purpose: read from the socket until a newline char is found
 * Receive: vptr: the pointer to the buffer that will hold the data
 * Return:  the number of bytes received.
 */
int TCP_Socket::read_line(void *vptr, int maxlen)
{
    int n, read_count;
    char c, *ptr;

    ptr = (char *) vptr;
    for(n = 1; n < maxlen; n++)
    {
        read_count = read(sock, &c, 1);
        // Keeps receiving, one byte by one byte
        if(read_count == 1)
        {
            *ptr++ = c;
            if(c == '\n') // check if the byte is newline
            {
                break; // break and end this function is yes
            }
        }
        else if(read_count == 0)
        {
            if(n == 1)
            {
                return 0;
            }
            else
            {
                return n;
            }
        }
        else // read_count < 0
        {
            return -1;
        }
    }
    *ptr = 0;
    return n;
}

/*
 * Name:    receive_response_header
 * Purpose: read from the socket until \r\n\r\n is captured. The purpose
 *          of this function is to receive HTTP message headers
 * Receive: buffer: buffer to hold the data
 *          buffer_len: the maximum length of the buffer
 *          total_received_len: the total number of data received
 * Return:  The end position of the HTTP message header.
 */

int TCP_Socket::receive_response_headers(char *buffer, int buffer_len,
                             int &total_received_len)
{
    static const char header_end[] = {'\r', '\n', '\r', '\n'};
    static const unsigned header_end_len = sizeof(header_end);

    // Piece-by-piece, buffer the server's response and look for the end
    // of the headers. Make a note of where in the buffer the end occurs.
    int bytes_recv = 0;
    int header_end_pos = -1;
    int header_end_read = 0;

    while((bytes_recv < buffer_len) && (header_end_pos < 0))
    {
        // Grab however many bytes are waiting for us right now.
        int recv_len = read(sock, buffer + bytes_recv,
                            buffer_len - bytes_recv);
        if(recv_len == -1)
        {
            // Something's wrong. If we cannot receive, reutrn -1
            return -1;
        }

        // Go over what we got in the buffer and look for the end of headers
        int i;
        for(i = bytes_recv;
            (i < (bytes_recv + recv_len)) &&
            (header_end_read < header_end_len);
            i++)
        {
            if(buffer[i] == header_end[header_end_read])
            {
                header_end_read++;
            }
            else
            {
                header_end_read = 0;
            }
        }

        // If we found the end, mark it.    Also keep track of how much
        // we've read total, for several reasons (not filling the
        // buffer; knowing how much we've read past the header, etc.).
        if(header_end_read >= header_end_len)
        {
            header_end_pos = i;
        }
        bytes_recv += recv_len;
    }
    total_received_len = bytes_recv;

    return header_end_pos;
    // Note that this header_end_pos here includes \r\n\r\n
}


/*
 * Purpose: read from the socket until \r\n\r\n is captured. Then store
 *          the header part and data part into two different variables
 * Receive: header: the variable to hold the header
 *          data: the variable to hold the data currently received
 * Return:  none
 */
// Receive a piece of response and extract the header portion from it.
// Stores the header in the string header and store the rest in the
// string data.
// One can check if the header is good by checking the length of header.
void TCP_Socket::read_header(string &header, string &data) {
    char buffer[BUFFER_SIZE];
    int l, total = 0;

    int header_end_pos = receive_response_headers
                         (buffer, BUFFER_SIZE - 1, total);

    if(header_end_pos < 0)
    {
        throw string("TCP_Socket Exception: Error receiving response header from server.");
    }

    else
    {
        // Store the received header and data into
        header.append(buffer, header_end_pos);
        data.append(buffer + header_end_pos, total - header_end_pos);
        //cout << "header length: " << header.length() << endl;
        //cout << "data length:   " <<data.length() << endl;
    }
}


/*
 * Name:    read_data
 * Purpose: Read bytes_left bytes from the socket
 * Receive: data: the string that will be used to hold the data
 *          bytes_left: the number of bytes to be read
 * Return:  the number of bytes read
 */
int TCP_Socket::read_data(string &data, int bytes_left)
{
    int total = 0, l;
    char buffer[BUFFER_SIZE];

    cout << "In read_data before the while." << endl;

    while(total < bytes_left)
    {
        cout << "In read_data in the while. the total is: " << total << " and bytes left is: " << bytes_left << endl;
        
        memset(buffer, 0, sizeof(buffer));
		cout << "In read_data before read_n_bytes" << endl;

        l = read_n_bytes(buffer, bytes_left);
		
		cout << "In read_data after read_n_bytes" << endl;

        if(l < 0)
        {
			cout << "In if in read_data" << endl;

            throw string("TCP_Socket Exception: error reading all data from socket");
            return -1;
        }
        else if(l == 0)
        {
			cout << "In else if in read_data" << endl;
            break;
        }
		
		cout << "after if/else statement in read_data" << endl;

        data.append(buffer, l);
        total += l;
    }
	
	cout << "In read_data outside the while loop" << endl;
    return total;
}

/*
 * Purpose: Reads a line of text from socket, terminated by a carriage
 *          return and line feed
 * Receive: data holds the string data read from the socket
 * Return:  The number of bytes actually read (should always be the
 *          length of the string)
*/
int TCP_Socket::read_line(string &data) {
    char buffer[BUFFER_SIZE];
    int l;

    memset(buffer, 0, BUFFER_SIZE);
    if((l = read_line(buffer, BUFFER_SIZE)) < 0)
    {
        throw string("TCP_Socket Exception: error reading line from socket");
    }

    buffer[l] = 0;
    data += buffer;

    return l;
}

void TCP_Socket::get_port(unsigned short &getting_port)
{
    getting_port = ntohs(server_addr.sin_port);
}
