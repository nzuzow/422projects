// Modified from CSE422 FS09
// Modified from CSE422 FS12
// Modified from CSE422 FS14
//

// Simple HTTP client for CSE422 F14 lab 02.

#include <climits>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <netdb.h>
#include <string>
#include "client.h"
#include "HTTP_Request.h"
#include "HTTP_Response.h"
#include "TCP_Socket.h"

using namespace std;


int main(int argc, char* argv[])
{
    char*          server_addr = NULL;
    char*          proxy_addr  = NULL;
    URL*           server_url  = NULL;
    URL*           proxy_url   = NULL;
    HTTP_Request*  request     = NULL;
    HTTP_Response* response    = NULL;
    FILE*          out         = NULL;
    
    parse_args(argc, argv, &server_addr, &proxy_addr);

    /***PARSING THE ADDRS RECEIVED TO URL OBJECTS***/
    // Must have a server to get data from
    if(server_addr == NULL)
    {
        cout << "You did not specify the host address." << endl;
        help_message(argv[0], cout);
        exit(1);
    }

    server_url = URL::parse(server_addr);
    if(server_url == NULL) // If URL parsing is failed
    {
        cout << "Unable to parse host address: " << server_addr << endl;
        cout << endl;
        help_message(argv[0], cout);
        exit(1);
    }

    // Proxy is an otional argument
    // If a proxy URL is specified, the client connects to the proxy and
    // downlods from the proxy. Otherwise, the client connects to the host 
    // and downloads from the host.
    proxy_url = NULL; 
    if(proxy_addr != NULL)
    {
        proxy_url = URL::parse(proxy_addr);
        if(proxy_url == NULL)
        {
            cout << "Unable to parse proxy address: " << proxy_addr << endl;
            cout << endl;
            help_message(argv[0], cout);
            exit(1);
        }
        if(proxy_url->is_port_defined() == false)
        {
            cout << "Proxy port is not defined, assumed to be 8080" << endl;
            proxy_url->set_port(8080);
        }
    }

    // TCP_Socket class to handle TCP communications.
    TCP_Socket client_sock;
    if(proxy_addr == NULL)
    // If proxy is not specified, connect to the host directly.
    {
        try{
            // Connect to the target server.
            client_sock.Connect(*server_url);
        }
        // Give up if sock is not created correctly.
        catch(string msg)
        {
            cout << msg << endl;
            cout << "Unable to connect to server: " 
                 << server_url->get_host() << endl;
            delete server_url;
            exit(1);
        }
    }
    else // proxy is specified, connect to proxy
    {
        try
        {
            // Connect to the proxy, instead of server
            client_sock.Connect(*proxy_url);
        }
        // Give up if sock is not created correctly.
        catch(string msg)
        {
            cout << msg << endl;
            cout << "Unable to connect to proxy: " 
                 << proxy_url->get_host() << endl;
            delete server_url;
            delete proxy_url;
            exit(1);
        }
    }
    /***END OF PARSING THE ADDRS RECEIVED TO URL OBJECTS***/










    /***SENDING THE REQUEST TO THE SERVER***/
    // Send a GET request for the specified file.
    // No matter connecting to the server or the proxy, the request is 
    // alwasy destined to the server.
    request = HTTP_Request::create_GET_request(server_url->get_path());
    request->set_host(server_url->get_host());
    // This request is non-persistent.
    request->set_header_field("Connection", "close");
    // For real browsers, If-Modified-Since field is always set.
    // if the local object is the latest copy, the browser does not 
    // respond the object.
    request->set_header_field("If-Modified-Since", "0");

    try 
    {   
        request->send(client_sock);
    }   
    catch(string msg)
    {
        cerr << msg << endl;
        exit(1);
    }   

    // output the request
    cout << "Request sent..." << endl;
    cout << "==========================================================" 
         << endl;
    string print_buffer;
    request->print(print_buffer);
    cout << print_buffer.substr(0, print_buffer.length() - 4) << endl;
    cout << "==========================================================" 
         << endl;

    delete request; // We do not need it anymore
    /***END OF SENDING REQUEST***/








    /***RECEIVING RESPONSE HEADER FROM THE SERVER***/
    // The server response is a stream starts with a header and then 
    // the body/data. A blank line separates the header and the body/data.
    // 
    // Read enough of the server's response to get all of the headers,
    // then have that response interpreted so we at least know what
    // happened.
    //
    // We create two strings to hold the incoming data. As described in the 
    // hanout, a HTTP message is composed of two portions, a header and a body.
    string response_header, response_body;

    // Read enough of the server's response to get all of the headers,
    // then have that response interpreted so we at least know what
    // happened.
    // The client receives the response stream. Check if the data it has
    // contains the whole header. 
    // read_header separates the header and data by finding the blank line.
    response->receive_header(client_sock, response_header, response_body);

    // The HTTP_Response::parse construct a response object. and check if 
    // the response is constructed correctly. Also it tries to determine 
    // if the response is chunked or not. This program does not handle 
    // chunked encoded transfer.
    response = HTTP_Response::parse(response_header.c_str(), 
                                    response_header.length());

    // The response is illegal.
    if(response == NULL)
    {
        cerr << "Unable to parse the response header." << endl;
        // clean up if there's something wrong
        delete response;
        if(proxy_url != NULL){
            delete proxy_url;
        }
        delete server_url;
        exit(1);
    }

    // output the response header
    cout << endl << "Response header received" << endl;
    cout << "=========================================================="
         << endl;
    response->print(print_buffer);
    cout << print_buffer.substr(0, print_buffer.length() - 4) << endl;
    cout << "==========================================================" 
         << endl;

    /***END OF RECEIVING RESPONSE HEADER FROM THE SERVER***/












    /***GET REST OF THE MESSAGE BODY AND STORE IT***/
    // Open a local copy in which to store the file.
    out = Open_local_copy(server_url);
    // check
    if(!out)
    {
        cout << "Error opening local copy for writing." << endl;
        // clean up if failed
        if(proxy_addr != NULL)
        {
            delete proxy_url;
        }
        delete server_url;
        exit(1);
    }

    cout << endl << "Downloading rest of the file ... " << endl;

    int bytes_written = 0, bytes_left;
    int total_data;

    if(response->is_chunked() == false)
    {
        // none-chunked encoding transfer does not split the data into 
        // chunks. The header specifies a content_length. The client knows 
        // exactly how many data it is expecting. The client keeps receiving
        // the response until it gets the amount.

        cout << "Default encoding transfer" << endl;
        cout << "Content-length: " << response->get_content_len() << endl;
        bytes_left = response->get_content_len();
        do
        {
            // If we got a piece of the file in our buffer for the headers,
            // have that piece written out to the file, so we don't lose it.
            fwrite(response_body.c_str(), 1, response_body.length(), out);
            bytes_written += response_body.length();
            bytes_left -= response_body.length();
            //cout << "bytes written:" <<  bytes_written << endl;
            //cout << "data gotten:" <<  response_body.length() << endl;

            response_body.clear();
            try
            {
                // Keeps receiving until it gets the amount it expects.
                response->receive_data(client_sock, response_body, 
                                       bytes_left);
            }
            catch(string msg)
            {
                // something bad happend
                cout << msg << endl;
                // clean up
                delete response;
                delete server_url;
                if(proxy_addr != NULL)
                {
                    delete proxy_url;
                }
                fclose(out);
                client_sock.Close();
                exit(1);        
            }
        } while (bytes_left > 0);
    }
    else // chunked encoding
    {
        cout << "Chunked encoding transfer" << endl;

        // As mentioned above, receive_header function already split the
        // data from the header from us. The beginning of this respnse_data
        // now holds the first chunk size.
        //cout << response_body.substr(0,15) << endl;
        int chunk_len = get_chunk_size(response_body);
        cout << "chunk length: " << chunk_len << endl;
        total_data = chunk_len;
        while(1)
        {
            // If current data holding is less than the chunk_len, this 
            // piece of data contains only part of this chunk. Receive more
            // until we have a complete chunk to store!
            //if(response_body.length() < (chunk_len + 4))
            if(response_body.length() < (chunk_len + 4))
            {
                try
                {
                    // receive more until we have the whole chunk.
                    response->receive_data(client_sock, response_body, 
                                (chunk_len - response_body.length()));
                    response->receive_line(client_sock, response_body);
                    // get the blank line between chunks
                    response->receive_line(client_sock, response_body);
                    // get next chunk, at least get the chunk size
                    //cout << response_body.length() << endl;
                }
                catch(string msg)
                {
                    // something bad happend
                    cout <<  msg << endl;
                    // clean up
                    delete response;
                    delete server_url;
                    if(proxy_addr != NULL)
                    {
                        delete proxy_url;
                    }
                    fclose(out);
                    client_sock.Close();
                    exit(1);        
                }
            }
            // If current data holding is longer than the chunk size, this
            // piece of data contains more than one chunk. Store the chunk.
            else//response_body.length() >= chunk_len
            {
                fwrite(response_body.c_str(), 1, chunk_len, out);
                bytes_written += chunk_len;

                // reorganize the data, remove the chunk from it
                // the + 2 here is to consume the extra CLRF
                
                response_body = response_body.substr(chunk_len + 2, 
                                response_body.length() - chunk_len - 2);
                //get next chunk size
                chunk_len = get_chunk_size(response_body);
                total_data += chunk_len;
                cout << "chunk length: " << chunk_len << endl;

                if(chunk_len == 0)
                {
                    break;
                }
            }
        }
    }
    

    cout << "Download complete (" << bytes_written;
    cout << " bytes written)" << endl;
   
    // This checks if the chunked encoding transfer mode is downloading
    // the contents correctly.
    if((total_data != bytes_written) && response->is_chunked() == true)
    {
        cout << "WARNING" << endl 
             << "Data received does not match chunk size." << endl;
    }


    // If the response is not OK, something is wrong.
    // However, we still downloaded the content, because even the response
    // is not 200. The server still replies with an error page (403, 404 ...)
    if(response->get_status_code() != 200)
    {
        cerr << response->get_status_code() << " " 
             << response->get_status_desc() << endl;
    }

    // everything's done.
    client_sock.Close();

    delete response;
    delete server_url;
    if(proxy_addr != NULL)
    {
        delete proxy_url;
    }
    fclose(out);

    return 0;
}
