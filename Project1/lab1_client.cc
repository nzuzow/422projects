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

    // parse the argvs, obtain server_name and tcp_server_port
    parse_argv(argc, argv, &server_name_str, tcp_server_port);

    cout << "[TCP] Tic Tac Toe client started..." << endl;
    cout << "[TCP] Connecting to server: " << server_name_str
         << ":" << tcp_server_port << endl;

    Tic_Tac_Toe game;
    // my_mark is empty, the client program needs to get it from the server.
    while(get_command(outgoing_pkt, game, my_mark) == false){}
}

