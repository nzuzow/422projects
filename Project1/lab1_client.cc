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
	hostent *hp;
	tcp_connection_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_connection_fd < 0) cout << "error opening socket" << endl;
	server_addr.sin_family = AF_INET;
	hp = gethostbyname(server_name_str);
	memcpy(&server_addr.sin_addr, hp->h_addr, hp->h_length);
	server_addr.sin_port = htons(tcp_server_port);
	if (connect(tcp_connection_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		cout << "failed to connect to tcp socket" << endl;;
		exit(1);
	}
// Join request
	memset(&outgoing_pkt, 0, sizeof(outgoing_pkt));
        outgoing_pkt.type = JOIN;
	bytes_sent = send(tcp_connection_fd, &outgoing_pkt, sizeof(outgoing_pkt), 0);
// Error check the Join request
	if (bytes_sent < 0) {
		cerr << "[ERR] Error sending message to server." << endl;
		close(tcp_connection_fd);
		exit(1);
	} else {
		get_type_name(outgoing_pkt.type, type_name);
		cout << "[TCP] Sent: " << type_name << " " << outgoing_pkt.buffer << endl;
	}
// Check for incoming messages
	memset(&incoming_pkt, 0, sizeof(incoming_pkt));
	bytes_received = recv(tcp_connection_fd, &incoming_pkt, sizeof(incoming_pkt), 0);
	if (bytes_received < 0) {
		cerr << "[ERR] Error receiving message from server." << endl;
		close(tcp_connection_fd);
		exit(1);
	} else {
		get_type_name(incoming_pkt.type, type_name);
		cout << "[TCP] Recv: " << type_name << " " << incoming_pkt.buffer << endl;
		my_mark = incoming_pkt.buffer[0];
	}
// Join Grant was received, need to send out request to get udp port
	if (incoming_pkt.type == JOIN_GRANT) {
		outgoing_pkt.type = GET_UDP_PORT;
		bytes_sent = send(tcp_connection_fd, &outgoing_pkt, sizeof(outgoing_pkt), 0);
		if (bytes_sent < 0) {
			cerr << "[ERR] Error sending message to server." << endl;
			close(tcp_connection_fd);
			exit(1);
		} else {
			get_type_name(outgoing_pkt.type, type_name);
			cout << "[TCP] Sent: " << type_name << " " << outgoing_pkt.buffer << endl;
		}
	}
// Check for incoming messages, should be getting the UDP port
	memset(&incoming_pkt, 0, sizeof(incoming_pkt));
	bytes_received = recv(tcp_connection_fd, &incoming_pkt, sizeof(incoming_pkt), 0);
	if (bytes_received < 0) {
		cerr << "[ERR] Error receiving message from server." << endl;
		close(tcp_connection_fd);
		exit(1);
	} else {
		get_type_name(incoming_pkt.type, type_name);
		cout << "[TCP] Recv: " << type_name << " " << incoming_pkt.buffer << endl;
		udp_server_port = htons(atoi(incoming_pkt.buffer));
	}
// Set up the UDP socket exchange
	sockaddr_in udp_server_addr;
	int udp_server_fd;
	int slen = sizeof(udp_server_addr);

	udp_server_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_server_fd < 0) {
		cerr << "[ERR] Unable to create UDP Socket." << endl;
		close(tcp_connection_fd);
		exit(1);
	}
	memset(&outgoing_pkt, 0, sizeof(outgoing_pkt));
	udp_server_addr.sin_family = AF_INET;
	udp_server_addr.sin_port = udp_server_port;
	memcpy(&udp_server_addr.sin_addr, hp->h_addr, hp->h_length);
// Send a get board message to start the game
	outgoing_pkt.type = GET_BOARD;
	outgoing_pkt.buffer[0] = my_mark;
	bytes_sent = sendto(udp_server_fd, &outgoing_pkt, sizeof(outgoing_pkt), 0,
			(sockaddr *) &udp_server_addr, slen);
	if (bytes_sent < 0) {
		cerr << "[ERR] Error sending message to server." << endl;
		close(udp_server_fd);
		close(tcp_connection_fd);
		exit(1);
	} else {
		get_type_name(outgoing_pkt.type, type_name);
		cout << "[UDP] Sent: " << type_name << " " << outgoing_pkt.buffer << endl;
		cout << "[SYS] Waiting for response..." << endl;
	}

    Tic_Tac_Toe game;
    // my_mark is empty, the client program needs to get it from the server.
    while(true){
	memset(&incoming_pkt, 0, sizeof(incoming_pkt));
       	bytes_received = recv(udp_server_fd, &incoming_pkt, sizeof(incoming_pkt), 0);
	if (bytes_received < 0) {
                cerr << "[ERR] Error receiving message from server." << endl;
		close(udp_server_fd);
		close(tcp_connection_fd);
                exit(1);
        } 
	if (incoming_pkt.type == YOUR_TURN) {
		Tic_Tac_Toe game1(incoming_pkt.buffer);	
		game1.print_board();
		cout << "[SYS] Your turn." << endl;
        	get_command(outgoing_pkt, game1, my_mark);
        	bytes_sent = sendto(udp_server_fd, &outgoing_pkt, sizeof(outgoing_pkt), 0,
                        (sockaddr *) &udp_server_addr, slen);
		get_type_name(outgoing_pkt.type, type_name);
		if (bytes_sent < 0) {
                	cerr << "[ERR] Error sending message to server." << endl;
			close(udp_server_fd);
			close(tcp_connection_fd);
                	exit(1);
        	} else {
			if (outgoing_pkt.type == EXIT) 
				cout <<"[UDP] Sent: " << type_name << endl;
			else
                		cout << "[UDP] Sent: " << type_name << " " << outgoing_pkt.buffer << endl;
        	}

	} else if (incoming_pkt.type == UPDATE_BOARD) {
		get_type_name(incoming_pkt.type, type_name);
		cout << "[UDP] Rcvd: " << type_name << " " << incoming_pkt.buffer << endl;
		Tic_Tac_Toe game(incoming_pkt.buffer);
		memset(&outgoing_pkt, 0, sizeof(outgoing_pkt));
		outgoing_pkt.type = GET_BOARD;
		outgoing_pkt.buffer[0] = my_mark;
        	bytes_sent = sendto(udp_server_fd, &outgoing_pkt, sizeof(outgoing_pkt), 0,
                        (sockaddr *) &udp_server_addr, slen);
		if (bytes_sent < 0) {
			cout << "[ERR] Error sending message to server." << endl;
			close(udp_server_fd);
			close(tcp_connection_fd);
			exit(1);
		}
		get_type_name(outgoing_pkt.type, type_name);
		cout << "[UDP] Sent: " << type_name << " " << outgoing_pkt.buffer << endl;
		cout << "[SYS] Waiting for response..." << endl;
	//	continue;
	} else if (incoming_pkt.type == YOU_WON) {
		cout << "Congratulations, you won!" << endl;	
		close(udp_server_fd);
		close(tcp_connection_fd);
		exit(1);
	} else if (incoming_pkt.type == YOU_LOSE) {
		cout << "Sorry, you lose." << endl;
                close(udp_server_fd);
                close(tcp_connection_fd);
		exit(1);
	} else if (incoming_pkt.type == TIE) {
		cout << "The game resulted in a tie." << endl;
                close(udp_server_fd);
                close(tcp_connection_fd);
		exit(1);
	} else if (incoming_pkt.type == EXIT_GRANT) {
		get_type_name(incoming_pkt.type, type_name);
		cout << "[UDP] Rcvd: EXIT_GRANT" << endl;
		if (my_mark == incoming_pkt.buffer[0]) {
			cout << "You have left the game. You lose!" << endl;
		} else {
			cout << "Opponent has left the game. You win!" << endl;
		}
                close(udp_server_fd);
                close(tcp_connection_fd);
		exit(1);
	}
    }	
}

