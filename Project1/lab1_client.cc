#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <sstream>
#include <cstring>

#include <netdb.h>
#include <unistd.h>

#include "packet.h"      // defined by us
#include "lab1_client.h" // some supporting functions.
#include "Tic_Tac_Toe.h"

using namespace std;

int main(int argc, char *argv[])
{
    sockaddr_in          server_addr;
    int                  tcp_connection_fd;
    int                  udp_socket_fd;

    int                  bytes_received;
    int                  bytes_sent;

    char                 *server_name_str = 0;
    unsigned short int   tcp_server_port;
    unsigned short int   udp_server_port;

    My_Packet            incoming_pkt;
    My_Packet            outgoing_pkt;

    char                 type_name[type_name_len];
    char                 my_mark;

    hostent              *hp; //address of remote host


    // parse the argvs, obtain server_name and tcp_server_port
    parse_argv(argc, argv, &server_name_str, tcp_server_port);

    cout << "[TCP] Tic Tac Toe client started..." << endl;
    cout << "[TCP] Connecting to server: " << server_name_str
         << ":" << tcp_server_port << endl;

    //added by me
    //create the socket
    tcp_connection_fd = socket(AF_INET, SOCK_STREAM, 0);

    //designate the addressing family
    server_addr.sin_family = AF_INET;

    //get the address of the remote host and store
    //hp = gethostbyname(tcp_server_port);
    hp = gethostbyname(server_name_str);
    memcpy(&server_addr.sin_addr, hp->h_addr, hp->h_length);

    //get the port used on the remote side and store
    server_addr.sin_port = htons(tcp_server_port);

    //connect to other side
    if(connect(tcp_connection_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
      cout << "Connection error!\n";
      close(tcp_connection_fd);
      exit(1);
    }
    else
    {
      //let the user know the connection was successful and then send a join request to the server
      cout << "[TCP] Connection successful!" << endl;
    }

    //clear the buffer of the outgoing packet before sending.
    memset(&outgoing_pkt, 0, sizeof(outgoing_pkt));

    //set the type of the outgoing packet
    outgoing_pkt.type = JOIN;

    //send the JOIN request to the server
    bytes_sent = send(tcp_connection_fd, &outgoing_pkt, sizeof(outgoing_pkt), 0);

    // check for errors in sending the information
    if(bytes_sent < 0)
    {
      cerr << "[ERR] Error sending message to server." << endl;
      exit(1);
    }
    else
    {
        get_type_name(outgoing_pkt.type, type_name);
        cout << "[TCP] Sent: " << type_name << " " << outgoing_pkt.buffer << endl;
    }

    // wipe out anything in incoming_pkt's buffer
    memset(&incoming_pkt, 0, sizeof(incoming_pkt));

    // receive
    bytes_received = recv(tcp_connection_fd, &incoming_pkt, sizeof(incoming_pkt), 0);

    //check for errors in receiving the bytes
    if(bytes_received < 0)
    {
      cerr << "[ERR] Error receiving message from server." << endl;
      exit(1);
    }
    else
    {
      get_type_name(incoming_pkt.type, type_name);
      cout << "[TCP] Received: " << type_name << " " << incoming_pkt.buffer << endl;
    }

    //set the mark for the client to the mark assigned by the server
    my_mark = incoming_pkt.buffer[0];

    Tic_Tac_Toe game;
    // my_mark is empty, the client program needs to get it from the server.
    while(get_command(outgoing_pkt, game, my_mark) == false){}
}
