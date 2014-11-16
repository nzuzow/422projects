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
  std::string server_addr;
  URI* server_uri = NULL;
  HTTP_Request* request = NULL;
  Playlist* playlist;

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
  // Use URI::Parse() to get the proper components from the given string
  server_uri = URI::Parse(server_addr);

  // Now check to make sure the URI was parsed correctly
  // Not sure if we need to just check for NULL or if we need to check for just
  // a forward slash, or an undefined port also
  if( server_uri == NULL)
  {
    cout << "There was an error parsing the URI." << endl;
    exit(1);
  }

  // Use playlist::Parse() to get the contents from the string that is passed in
  //playlist = Playlist::Parse(server_addr);

  // Now check to make sure the playlist was parsed correctly.
  //if( playlist == NULL)
  //{
  //  cout << "There was an error parsing the playlist." << endl;
  //  exit(1);
  //}

  // Download the playlist at that URI.
  // Parse it together, too.

  // TCP_Socket class to handle TCP communications.
  TCP_Socket client_sock;

  try
  {
    // Connect to the target server.
    client_sock.Connect(*server_uri);
  }
  // Give up if sock is not created correctly.
  catch(string msg)
  {
    cout << msg << endl;
    cout << "Unable to connect to server: "
         << server_uri->Get_host() << endl;
    delete server_uri;
    exit(1);
  }

  // Send a GET request for the specified file.
  request = HTTP_Request::Create_GET_request(server_uri->Get_path());
  request->Set_host(server_uri->Get_host());

  // Print the request to a string so we can send it to the HTTP server
  string request_str;
  request->Print(request_str);

  // Try to send the data to the server. If there is an error, then we will
  // print the error to the screen and then exit.
  try
  {
    client_sock.write_string(request_str);
  }
  catch(string msg)
  {
    cerr << msg << endl;
    exit(1);
  }

  // Get a video player set up so we can see the video.

  // Download and stream each of the video segments in the playlist,
  // in order.

  // If we don't have a reason to want to quit early, wait for the user
  // to finish watch the video & close the window.

  // Clean up.
  return 0;
}
