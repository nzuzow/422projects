// Example driver for Lab 3.

#include "HTTP_Request.h"
#include "HTTP_Response.h"
#include "Playlist.h"
#include "URI.h"
#include "Video_Player.h"
#include "TCP_Socket.h"
#include <climits>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <string>
#include <unistd.h>

using namespace std;


// Prints a brief usage string describing how to use the application, in case
// the user passes in something that just doesn't work.
void Print_usage(const char* exe_name, ostream& out)
{
    out << "Usage:  " << exe_name << " http://server.com/stream.m3u8";
    out << endl;
}



int main(int argc, char* argv[])
{
  char* server_addr = NULL;
  URI* server_uri = NULL;
  HTTP_Request* request = NULL;
  HTTP_Response* response = NULL;
  FILE* out = NULL;
  TCP_Socket client_sock;

  // Look at the command line and figure out what we're playing today.
  if (argc != 2)
  {
      Print_usage(argv[0], cout);
      return 1;
  }

  // Get the server address from the command line argument
  server_addr = argv[1];
  cout << "You entered " << server_addr << " as the address of the server." << endl;

  // Parse the playlist URI
  // Handle bad URI

  // Download the playlist at that URI.
  // Parse it together, too.

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
      /*if(proxy_url != NULL){
          delete proxy_url;
      }*/
      delete server_uri;
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
  out = Open_local_copy(server_uri);
  // check
  if(!out)
  {
      cout << "Error opening local copy for writing." << endl;
      // clean up if failed
      /*if(proxy_addr != NULL)
      {
          delete proxy_url;
      }*/
      delete server_uri;
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
              delete server_uri;
              /*if(proxy_addr != NULL)
              {
                  delete proxy_url;
              }*/
              fclose(out);
              client_sock.Close();
              exit(1);
          }
      } while (bytes_left > 0);
  }

  // Get a video player set up so we can see the video.

  // Download and stream each of the video segments in the playlist,
  // in order.

  // If we don't have a reason to want to quit early, wait for the user
  // to finish watch the video & close the window.

  // Clean up.
  return 0;
}
