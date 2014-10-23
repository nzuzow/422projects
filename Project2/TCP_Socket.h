// Modified from CSE422 FS09
// Modified from CSE422 FS12
// Modified from CSE422 FS14

#ifndef _TCPSOCKET_H_
#define _TCPSOCKET_H_
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include "URL.h"
#include <string.h>

/*
Purpose: Wraps the functionality of a TCP socket in a class. Errors are returned by 
         throwing exceptions when occur. Additional comment can be found with the code.
*/
class TCP_Socket {
private:
  int sock;
  struct sockaddr_in server_addr;
  int read_n_bytes(void *, int);
  int read_line(void *, int);
  int receive_response_headers(char *, int, int &);

  void create_socket();

public:
  TCP_Socket();
  ~TCP_Socket();

  void Connect(std::string &, int);
  void Connect(hostent *, int);
  void Connect(const URL &);

  int Close();

  void Bind(int);
  void Listen();

  bool Accept(TCP_Socket &);
  TCP_Socket *Accept();

  int write_string(std::string &);
  int read_string(std::string &);
  int ReadUntilClose(std::string &);
  void read_header(std::string &, std::string &);
  int read_data(std::string &, int bytes_left);
  int read_line(std::string &);
  
  void get_port(unsigned short int &getting_port);
};
#endif
