// Modified from CSE422 FS09
// Modified from CSE422 FS12
// Modified from CSE422 FS14
//

#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include "HTTP_Request.h"
#include "HTTP_Response.h"
#include "URL.h"
#include "TCP_Socket.h"
#include "Proxy_Worker.h"

using namespace std;

int port = 80;
hostent *server = NULL;

void help_message(char *argv[])
{
    cout << "Usage " << argv[0] << " [options]" << endl;
    cout << "The following option is available:" << endl;
    cout << "    -h Display help message" << endl;
}

/*********************************
 * Name:    parse_argv
 * Purpose: parse the parameters
 * Recieve: argv and argc
 * Return:  none
 *********************************/
void parse_args(int argc, char *argv[]) 
{
    char *endptr; // for strtol

    for(int i = 1; i < argc; i++)
    {
        if((!strncmp(argv[i], "-h", 2)) ||
           (!strncmp(argv[i], "-H", 2)))
        {
            help_message(argv);
            exit(1);
        }
        else{
            cerr << "Invalid parameter:" << argv[i] << endl;    
            help_message(argv);
            exit(1);
        }
    }
}

