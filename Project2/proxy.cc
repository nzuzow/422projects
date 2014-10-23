// Modified from CSE422 FS09
// Modified from CSE422 FS12
// Modified from CSE422 FS14
//

#include "proxy.h"

/*
 * Name:    ClientHandler
 * Purpose: Handles a single web request
 * Receive: The data socket pointer
 * Return:  None
 */
void* connection_handler(void *arg){
    TCP_Socket *client_sock = (TCP_Socket *) arg;
    Proxy_Worker *pw = new Proxy_Worker(client_sock);

    //tell the worker to handle the request
    pw->handle_request();

    client_sock->Close();
    //cleanup
    delete pw;
    delete client_sock;
    pthread_exit(0);
}

/*
 * Purpose: Contains the main server loop that handles requests by 
 *          spawing child threads
 * Receive: argc is the number of command line params, argv are the 
 *          parameters
 * Return:  0 on clean exit, and 1 on error
 */
int main(int argc, char *argv[]) {
    TCP_Socket listen_sock;
    TCP_Socket *client_sock;

    unsigned short int port = 0;
    int req_id = 0;

    int rc; // return code for pthread

    parse_args(argc, argv);

    //bind to a port and listen
    try
    {
        listen_sock.Bind(port);
        listen_sock.Listen();
    }
    catch (string msg)
    {
        cout << msg << endl;
        exit(1);
    }

    listen_sock.get_port(port);
    cout << "Proxy running at " << port << "..." << endl;

    //start the infinite server loop
    while(true) {
        //accept incoming connections
        try
        {
            client_sock = new TCP_Socket();
            if(listen_sock.Accept(*client_sock) == false)
            {
                cout << "Cannot accept ...";
                continue;
            }
        }
        catch (string msg)
        {
            cout << msg << endl;
            continue;
        }

        // create new thread
        pthread_t thread;
        rc = pthread_create(&thread, NULL, connection_handler, client_sock);

        // if rc !=, the creation of threadis failed.
        if(rc)
        {
            cerr << "Thread create error! Error code: " << rc << endl;
            exit(1);
        }
    }

    // close the listening sock
    if(listen_sock.Close() < 0)
    {
        cerr << "Error when closing listening socket." << endl;
        exit(1);
    }
    cout << "Parent process termianted." << endl;

    return 0;
}

