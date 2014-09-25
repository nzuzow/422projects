// Simple packet structure
#ifndef _PACKET_H_
#define _PACKET_H_

#define JOIN            1000
#define JOIN_GRANT      1001
#define GET_UDP_PORT    2000
#define UDP_PORT        2001
#define GET_BOARD       3000
#define YOUR_TURN       3001
#define OPPONENT_TURN   3002
#define PLAYER_MARK     4001
#define UPDATE_BOARD    4002
#define YOU_WON         4003
#define YOU_LOSE        4004
#define TIE             4005
#define EXIT            9000
#define EXIT_GRANT      9001

using namespace std;
const unsigned int type_name_len = 16;
const unsigned int buffer_len = 16;

struct My_Packet
{
    unsigned int type;
    char buffer[buffer_len];
};

/*********************************
 * Name:    get_type_name
 * Purpose: When receiving a message, we can only see the type as integer
 *          (ie, 2000, 2001, 3000 ... etc). This function converts those
 *          integers into coresponding char arrays.
 * Receive: The type in integer
 * Return:  the type in char array, length = 16
 *********************************/
void get_type_name(int type, char *type_name)
{
    memset(type_name, 0, type_name_len);
    switch(type)
    {
        case JOIN:
            memcpy(type_name, "JOIN\0", type_name_len); 
            break;
        case JOIN_GRANT:
            memcpy(type_name, "JOIN_GRANT\0", type_name_len); 
            break;
        case GET_UDP_PORT:
            memcpy(type_name, "GET_UDP_PORT\0", type_name_len); 
            break;
        case UDP_PORT:
            memcpy(type_name, "UDP_PORT\0", type_name_len); 
            break;
        case GET_BOARD:
            memcpy(type_name, "GET_BOARD\0", type_name_len); 
            break;
        case YOUR_TURN:
            memcpy(type_name, "YOUR_TURN\0", type_name_len); 
            break;
        case OPPONENT_TURN:
            memcpy(type_name, "OPPONENT_TURN\0", type_name_len); 
            break;
        case PLAYER_MARK:
            memcpy(type_name, "PLAYER_MARK\0", type_name_len); 
            break;
        case UPDATE_BOARD:
            memcpy(type_name, "UPDATE_BOARD\0", type_name_len); 
            break;
        case YOU_WON:
            memcpy(type_name, "YOU_WON\0", type_name_len); 
            break;
        case YOU_LOSE:
            memcpy(type_name, "YOU_LOSE\0", type_name_len); 
            break;
        case TIE:
            memcpy(type_name, "TIE\0", type_name_len); 
            break;
        case EXIT:
            memcpy(type_name, "EXIT\0", type_name_len); 
            break;
        case EXIT_GRANT:
            memcpy(type_name, "EXIT_GRANT\0", type_name_len); 
            break;
        default:
            cerr << "[SYS] Invalid command type: " << type << "."<<  endl;
            break; 
    }
}

#endif
