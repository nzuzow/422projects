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
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;


// Prints a brief usage string describing how to use the application, in case
// the user passes in something that just doesn't work.
void Print_usage(const char* exe_name, ostream& out)
{
    out << "Usage:  " << exe_name << " http://server.com/stream.m3u8";
    out << endl;
}

/*********************************
 * Name:    Open_local_copy
 * Purpose: Open a file pointer to store the data in ./Download
 * Receive: uri - the uri for the object
 * Return:  The file pointer
 *********************************/
// Opens a local copy of the file referenced by the given request URL, for
// writing.  Ignores any directories in the URL path, instead opening the file
// in the current directory.  Makes up a filename if none is given.
//
// Returns a pointer to the open file, or a NULL pointer if the open fails.
FILE* Open_local_copy(const URI* uri)
{
  FILE* outfile = NULL;

  struct stat sb; // For checking if ./Download exists

  if(stat("./Download", &sb) == -1) // if ./Download does not exist
  {
    mkdir("./Download", 0700);   // create it
  }

  const string& full_path = uri->Get_path();
  size_t filename_pos = full_path.rfind('/');
  // find the last '/', the substring after it should be the filename

  if ((filename_pos != string::npos) && // if found a '/'
      ((filename_pos + 1) < full_path.length())) // or / is not the end of
                                                 // the string
  {
    string fn = string("Download/") + full_path.substr(filename_pos + 1);
    outfile = fopen(fn.c_str(),"wb");
  }
  else
  {
    outfile = fopen("Download/index.html", "wb");
  }

  return outfile;
}


int main(int argc, char* argv[])
{
  std::string server_addr;
  URI* server_uri = NULL;
  HTTP_Request* request = NULL;
  HTTP_Response* response = NULL;
  FILE* out = NULL;
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

  /*** RECEIVE RESPONSE HEADER FROM SERVER ***/

  // Setup two strings for the response header and the response body from the
  // server.
  //string response_header, response_body;

  // Setup a buffer array and value for the total length of the received message.
  // The value for the buffer came from the value within test_client. I'm not
  // actually sure why this value was picked though.
  char buff[65536];
  int total_received_len = 0;

  // Now call read_header to get the proper information from the socket
  //client_sock.read_header(response_header, response_body);

  // Actually need to call receive_response_headers since we don't have one
  // header followed by one large body. This method takes three arguments,
  // the buffer to hold the data in, the maximum length of the buffer, and
  // the total number of bytes received so far.
  int header_stop = client_sock.receive_response_headers(buff, buff.size(), total_received_len);

  // Make sure the end position of the header is not 0 meaning the header is
  // not empty
  if( header_stop <= 0)
  {
    cout << "There was an error receiving the response from the server." << endl;
    exit(1);
  }


  // The HTTP_Response::parse construct a response object, and check if
  // the response is constructed correctly. This will take the buffer of message
  // headers and the last ending position of the header as arguments.
  response = HTTP_Response::Parse(buff, header_stop);

  // Now make sure the response is legal
  if( response == NULL)
  {
    cerr << "Unable to parse the response header." << endl;
    // clean up if there's something wrong
    delete response;
    delete server_uri;
    exit(1);
  }

  // Even if the response is constructed correctly, we still need to check for
  // the proper status of the server
  unsigned status_code;

  status_code = response->Get_code();

  // Make sure the status is 200. If not then we need to print an error
  if( status_code != 200)
  {
    string status_reason = response->Get_reason();

    cout << "Request failed:" << endl;
    cout << status_code << " " << status_reason << endl;
    delete response;
    exit(1);
  }

  // Create a variable for the content length
  int content_len = 0;

  // Also check to make sure the response is in a form we can handle
  string tmp;
  response->Get_header_value("Transfer-Encoding", tmp);
  if(tmp.find("chunked") != string::npos) {
    //response->chunked = true;
    cout << "Unable to read chunked encoding." << endl;
    delete response;
    exit(1);
  }
  else{
    //response->chunked = false;
    tmp.clear();
    response->Get_header_value("Content-Length", tmp);
    istringstream s(tmp);
    s >> content_len;
    //s >> response->content_len;
  }

  /*** END OF RECEIVE RESPONSE HEADER FROM SERVER ***/


  /*** GET REST OF THE MESSAGE BODY AND STORE IT ***/
  // Open a local copy in which to store the file.
  out = Open_local_copy(server_uri);
  // check
  if(!out)
  {
    cout << "Error opening local copy for writing." << endl;
    // clean up if failed
    delete server_uri;
    exit(1);
  }

  /*** END OF GETTING THE REST OF THE MESSAGE BODY AND STORING IT ***/

  // Get a video player set up so we can see the video.

  // Download and stream each of the video segments in the playlist,
  // in order.

  // If we don't have a reason to want to quit early, wait for the user
  // to finish watch the video & close the window.

  // Clean up.
  return 0;
}
