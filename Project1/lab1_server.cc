#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <iostream>
#include <sstream>
#include <cstring>

#include <unistd.h>

#include "packet.h" // defined by us
#include "Tic_Tac_Toe.h"
#include "lab1_server.h"

using namespace std;

// Function prototypes
void game_handler(int *tcp_player_fd);
bool handle_player_join(int &tcp_connection_fd, char mark);

int main(int argc, char *argv[])
{
    sockaddr_in tcp_server_addr;
    sockaddr_in tcp_connection_addr;
    socklen_t   tcp_server_addr_len = sizeof(tcp_server_addr);
    socklen_t   tcp_connection_addr_len = sizeof(tcp_connection_addr);

    int         tcp_server_fd;

    parse_argv(argc, argv); // In fact, this is not necessary.
                            // But I leave it here to capture invalid
                            // parameters.

    cout << "[SYS] Parent process for TCP communication." << endl; 
    cout << "[TCP] Tic-Tac-Toe server started..." << endl;

    // create a TCP listening socket for clients to connect
    tcp_server_fd = socket(AF_INET, SOCK_STREAM, 0);
  
    // if the return value is -1, the creation of socket is failed.
    if(tcp_server_fd < 0) {
        cerr << "[ERR] Unable to create TCP socket." << endl;
        exit(1);
    }

    // initialize the socket address strcut by setting all bytes to 0
    memset(&tcp_server_addr, 0, sizeof(tcp_server_addr));

    // details are covered in class/slides
    tcp_server_addr.sin_family      = AF_INET;
    tcp_server_addr.sin_port        = 0; // ask the OS to assign the port
    tcp_server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket address to the listening socket
    if(bind(tcp_server_fd, 
            (sockaddr *)&tcp_server_addr, 
            sizeof(tcp_server_addr)) < 0) {
        cerr << "[ERR] Unable to bind TCP socket." << endl;
        exit(1);
    }

    // listen to the socket, wait for incoming connections
    listen(tcp_server_fd, 1);

    // Obtain the socket information created by the OS
    if(getsockname(tcp_server_fd, (sockaddr *) &tcp_server_addr, 
                   &tcp_server_addr_len) < 0) {
        cout << "[ERR] Unable to obtain TCP socket information." << endl;
        exit(1);
    }
    cout << "[TCP] Port: " << ntohs(tcp_server_addr.sin_port) << endl;

    int tcp_player_fd[2];
           
    tcp_player_fd[0] = -1;
    tcp_player_fd[1] = -1;

    // first we need to accept two players
    for(int i = 0; i < 2; i++) {
        while(tcp_player_fd[i] == -1) {
            // accept the incoming connection as player1
            // store client addr in tcp_connection_addr
            tcp_player_fd[i] = accept(tcp_server_fd, 
                                      (sockaddr *) &tcp_connection_addr,
                                      &tcp_connection_addr_len);

            // If the server fail to accept
            if(tcp_player_fd[i] < 0) {
                cerr << "[ERR] Error on accepting player " << i << "." 
                     << endl;
            } else if(handle_player_join(tcp_player_fd[i],
                      Tic_Tac_Toe::get_mark(i)) == false) {
                tcp_player_fd[i] = -1;
                cerr << "[ERR] The TCP connection sends invalid message."
                     << endl;
                exit(1);
            }
        }
    }
    // when we reach here, we have two players

    game_handler(tcp_player_fd);

    if(close(tcp_server_fd) < 0) {
        cerr << "[ERR] Error when closing TCP server socket." << endl; 
        exit(1);
    }

    return 0;
}

/*********************************
 * Name:    handle_player_join
 * Purpose: The function to handle TCP communication from/to a client
 * Receive: The tcp_connection_fd socket. The client has to perform a connect
 *          through TCP to obtain a mark assignemnt (O or X) for gameplay 
 * Return:  true if this is a valid join
*********************************/
bool handle_player_join(int &tcp_connection_fd, char mark) {
    My_Packet   incoming_pkt;
    My_Packet   outgoing_pkt;
    int         bytes_received;
    int         bytes_sent;
    char        type_name[type_name_len];

    // wipe out anything in incoming_pkt's buffer
    memset(&incoming_pkt, 0, sizeof(incoming_pkt));
    // receive
    bytes_received = recv(tcp_connection_fd, 
                          &incoming_pkt, 
                          sizeof(incoming_pkt), 
                          0);

    // check
    if(bytes_received < 0) {
        cerr << "[ERR] Error receiving message from client" << endl;
        return false;
    } else {
        get_type_name(incoming_pkt.type, type_name);
        cout << "[TCP] Recv: " << type_name << " "
             << incoming_pkt.buffer << endl;
    }

    // if this pkt is not trying to establish a game connection JOIN,
    // we discard this message and terminate this child process
    if(incoming_pkt.type != JOIN) {
        cerr << "[ERR] Wrong packet type. Expecting JOIN" << endl;
        return false;
    } else {
        memset(&outgoing_pkt, 0, sizeof(outgoing_pkt));
        outgoing_pkt.type = JOIN_GRANT;
        outgoing_pkt.buffer[0] = mark;

        bytes_sent = send(tcp_connection_fd, 
                          &outgoing_pkt, 
                          sizeof(outgoing_pkt), 
                          0);

        // check
        if(bytes_sent < 0) {
            cerr << "[ERR] Error sending message to client." << endl;
            exit(1);
        } else {
            get_type_name(outgoing_pkt.type, type_name);
            cout << "[TCP] Sent: " << type_name << " "
                 << outgoing_pkt.buffer << endl;
        }
        return true;
    }
}

/*********************************
 * Name:    game_handler
 * Purpose: The function to create UDP socket and handle the actual gameplay 
 * Receive: The tcp_player_fd connectsions. The client has to perform a GET_UDP_PORT
 *          through TCP to login to the server and to obtain the UDP socket
 *          for gameplay 
 * Return:  None
*********************************/
void game_handler(int* tcp_player_fd)
{
    unsigned short int   udp_server_port;
    sockaddr_in udp_server_addr;
    socklen_t   udp_server_addr_len = sizeof(udp_server_addr);
    sockaddr_in udp_connection_addr;
    socklen_t   udp_connection_addr_len = sizeof(udp_connection_addr);

    int         udp_server_fd;

    My_Packet   incoming_pkt;
    My_Packet   outgoing_pkt;
    int         bytes_received;
    int         bytes_sent;

    char        port_string[16];
    char        type_name[type_name_len];

    Tic_Tac_Toe game;

    // create a UDP listening socket for clients to connect
    udp_server_fd = socket(AF_INET, SOCK_DGRAM, 0); 

    // if the return value is -1, the creation of UDP socket is failed.
    if(udp_server_fd < 0) {
        cerr << "[ERR] Unable to create UDP socket." << endl;
        exit(1);
    }

    // initialize the socket address strcut by setting all bytes to 0
    memset(&udp_server_addr, 0, sizeof(udp_server_addr));

    // details are covered in class/slides
    udp_server_addr.sin_family      = AF_INET;
    udp_server_addr.sin_port        = 0; // Ask the OS to assign the port
    udp_server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket address to the listening socket
    if(bind(udp_server_fd,
            (sockaddr *) &udp_server_addr,
            sizeof(udp_server_addr)) < 0) {
        cerr << "[ERR] Unable to bind UDP socket." << endl;
        exit(1);
    }

    udp_connection_addr_len = sizeof(udp_connection_addr);

    // Get the port number chosen by OS
    // Send it to the client, so the client know which UDP port 
    // to connect to.
    getsockname(udp_server_fd, (sockaddr *) &udp_server_addr, 
                &udp_server_addr_len);
    udp_server_port = ntohs(udp_server_addr.sin_port);

    cout << "[UDP:" << udp_server_port << "] Gameplay server started."
         << endl;
    
    snprintf(port_string, sizeof(port_string), "%d", udp_server_port);

    for(int i = 0; i < 2; i++) {
        // wipe out anything in incoming_pkt's buffer
        memset(&incoming_pkt, 0, sizeof(incoming_pkt));
        // receive
        bytes_received = recv(tcp_player_fd[i], 
                              &incoming_pkt, 
                              sizeof(incoming_pkt), 
                              0);

        // check
        if(bytes_received < 0) {
            cerr << "[ERR] Error receiving message from client" << endl;
            exit(1);
        } else {
            get_type_name(incoming_pkt.type, type_name);
            cout << "[TCP] Recv: " << type_name << " "
                 << incoming_pkt.buffer << endl;
        }
    
        // if this pkt is not trying to establish a game connection JOIN,
        // we discard this message and terminate this child process
        if(incoming_pkt.type != GET_UDP_PORT) {
            cerr << "[ERR] Wrong packet type. Expecting GET_UDP_PORT" << endl;
            exit(1);
        } else {
            memset(&outgoing_pkt, 0, sizeof(outgoing_pkt));
            outgoing_pkt.type = UDP_PORT;
            memcpy(outgoing_pkt.buffer, port_string, sizeof(port_string));

            bytes_sent = send(tcp_player_fd[i], 
                              &outgoing_pkt, 
                              sizeof(outgoing_pkt), 
                              0);
            // check
            if(bytes_sent < 0) {
                cerr << "[ERR] Error sending message to client." << endl;
                exit(1);
            } else {
                get_type_name(outgoing_pkt.type, type_name);
                cout << "[TCP] Sent: " << type_name << " "
                     << outgoing_pkt.buffer << endl;
            }
        }
        if(close(tcp_player_fd[i]) < 0) {
            cerr << "[ERR] Error when closing TCP client socket." << endl;
            exit(1);
        }
    }
    // When reaching here, both players have got the UDP port

    game.start_game();
    int current_player = 0;
    sockaddr_in opponent_addr;

    // Use the UDP for game play
    while(1) {
        // wipe out anything in incoming_pkt
        memset(&incoming_pkt, 0, sizeof(incoming_pkt));
        // receive
        bytes_received = recvfrom(udp_server_fd, &incoming_pkt, 
                                  sizeof(incoming_pkt), 0,  
                                  (sockaddr *) &udp_connection_addr, 
                                  &udp_connection_addr_len);
        //check
        if(bytes_received < 0) {
            cerr << "[ERR] Error receiving message from client (UDP:" 
                 << udp_server_port << ")" << endl;
            exit(1);
        }

        // wipe out anything in outging_pkt
        memset(&outgoing_pkt, 0, sizeof(outgoing_pkt));
        get_type_name(incoming_pkt.type, type_name);
        cout << "[UDP:" << udp_server_port << "] Rcvd: " 
             << type_name << " " 
             << incoming_pkt.buffer << endl;

        memset(&outgoing_pkt, 0, sizeof(outgoing_pkt));
        if(incoming_pkt.type == GET_BOARD) {
            char player_mark = incoming_pkt.buffer[0];
            if(player_mark == Tic_Tac_Toe::get_mark(current_player)) {
                outgoing_pkt.type = YOUR_TURN;
                memcpy(outgoing_pkt.buffer, game.get_board(), game.get_board_size()); 

                bytes_sent = sendto(udp_server_fd, &outgoing_pkt, 
                                    sizeof(outgoing_pkt), 0, 
                                    (sockaddr *) &udp_connection_addr,
                                    udp_connection_addr_len);
                // check       
                if(bytes_sent < 0) {
                    cerr << "[ERR] Error sending message to client." << endl;
                    exit(1);
                }
                // convert the type in int to char type_name
                get_type_name(outgoing_pkt.type, type_name);
                cout << "[UDP:" << udp_server_port << "] Sent: " << type_name << " "
                     << outgoing_pkt.buffer << endl;
            } else {
                memcpy(&opponent_addr, &udp_connection_addr, sizeof(udp_connection_addr));
            }
        }
        else if(incoming_pkt.type == PLAYER_MARK) {
            char position = incoming_pkt.buffer[0];
            game.mark(position, Tic_Tac_Toe::get_mark(current_player));

            memcpy(outgoing_pkt.buffer, game.get_board(), game.get_board_size()); 
            if(game.has_won() == true) {
                outgoing_pkt.type = YOU_WON;
            } else if (game.is_tie()) {
                outgoing_pkt.type = TIE; 
            } else {
                outgoing_pkt.type = UPDATE_BOARD;
            }

            bytes_sent = sendto(udp_server_fd, &outgoing_pkt, 
                                sizeof(outgoing_pkt), 0, 
                                (sockaddr *) &udp_connection_addr,
                                udp_connection_addr_len);
            // check       
            if(bytes_sent < 0) {
                cerr << "[ERR] Error sending message to client." << endl;
                exit(1);
            }
            // convert the type in int to char type_name
            get_type_name(outgoing_pkt.type, type_name);
            cout << "[UDP:" << udp_server_port << "] Sent: " << type_name << " "
                 << outgoing_pkt.buffer << endl;

            
            if(game.has_won() == true) {
                outgoing_pkt.type = YOU_LOSE;
            } else if(game.is_tie() == true) {
                outgoing_pkt.type = TIE;
            } 
            bytes_sent = sendto(udp_server_fd, &outgoing_pkt, 
                                sizeof(outgoing_pkt), 0, 
                                (sockaddr *) &opponent_addr,
                                udp_connection_addr_len);
            // check       
            if(bytes_sent < 0) {
                cerr << "[ERR] Error sending message to client." << endl;
                exit(1);
            }
            // convert the type in int to char type_name
            get_type_name(outgoing_pkt.type, type_name);
            cout << "[UDP:" << udp_server_port << "] Sent: " << type_name << " "
                 << outgoing_pkt.buffer << endl;

            if(current_player == 0) {
                current_player = 1;
            } else { 
                current_player = 0;
            }
        
            if((game.has_won() == true) || (game.is_tie() == true)) {
                break;
            }
        }
        else if(incoming_pkt.type == EXIT) {
            cout << "[UDP:" << udp_server_port << "] Player has left the game." 
                 << endl;
            memset(&outgoing_pkt, 0, sizeof(outgoing_pkt));
            outgoing_pkt.type = EXIT_GRANT;
            bytes_sent = sendto(udp_server_fd, &outgoing_pkt, 
                                sizeof(outgoing_pkt), 0, 
                                (sockaddr *) &udp_connection_addr,
                                udp_connection_addr_len);
            // check       
            if(bytes_sent < 0) {
                cerr << "[ERR] Error sending message to client." << endl;
                exit(1);
            }
            // convert the type in int to char type_name
            get_type_name(outgoing_pkt.type, type_name);
            cout << "[UDP:" << udp_server_port << "] Sent: " << type_name << " "
                 << outgoing_pkt.buffer << endl;

            bytes_sent = sendto(udp_server_fd, &outgoing_pkt, 
                                sizeof(outgoing_pkt), 0, 
                                (sockaddr *) &opponent_addr,
                                udp_connection_addr_len);
            // check       
            if(bytes_sent < 0) {
                cerr << "[ERR] Error sending message to client." << endl;
                exit(1);
            }
            // convert the type in int to char type_name
            get_type_name(outgoing_pkt.type, type_name);
            cout << "[UDP:" << udp_server_port << "] Sent: " << type_name << " "
                 << outgoing_pkt.buffer << endl;
            
            break;
        }
    }
    if(close(udp_server_fd) < 0) {
        cerr << "[ERR] Error when closing UDP server socket" << endl;
        exit(1);
    }
}

