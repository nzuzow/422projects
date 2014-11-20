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
#include <fstream>
#include <vector>

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
  //cout << "You entered " << server_addr << " as the address of the server." << endl;

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
  //cout << "Server uri is good" << endl;


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
  //cout << "TCP Socket successful" << endl;

  // Send a GET request for the specified file.
  request = HTTP_Request::Create_GET_request(server_uri->Get_path());
  request->Set_host(server_uri->Get_host());
  //cout << "Sent a get request" << std::endl;

  // Print the request to a string so we can send it to the HTTP server
  string request_str;
  request->Print(request_str);
  //cout << "Printed the request to a string to send to http server" << endl;

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
  //cout << "Sent data to server" << endl;

  /*** RECEIVE RESPONSE HEADER FROM SERVER ***/

  // Setup two strings for the response header and the response body from the
  // server.
  string response_header, response_body;
  //cout << "set up two string for response header and response body" << endl;

  // Now call read_header to get the proper information from the socket
  client_sock.read_header(response_header, response_body);
  //cout << "called read_header to get proper info from socket" << endl;


  // The HTTP_Response::parse construct a response object, and check if
  // the response is constructed correctly.
  response = HTTP_Response::Parse(response_header.c_str(), response_header.length());

  // Now make sure the response is legal
  if( response == NULL)
  {
    cerr << "Unable to parse the response header." << endl;
    // clean up if there's something wrong
    delete response;
    delete server_uri;
    exit(1);
  }
  //cout << "constructed a response object and it was constructed correctly" << endl;

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

  // Do a check to see if content length is greater than 0, which will
  // signify if it has been changed or not.
  if( content_len <= 0)
  {
    cout << "Error, no content was downloaded." << endl;
    exit(1);
  }

  /*** END OF RECEIVE RESPONSE HEADER FROM SERVER ***/

  //cout << "end of receive response header from server, get rest of body and store it" << endl;

  /*** GET REST OF THE MESSAGE BODY AND STORE IT ***/

  /* I'm not actually sure if we need to open a new file here or not. I will
  look at this more tomorrow. */

  // Open a local copy in which to store the file.
  //out = Open_local_copy(server_uri);
  // check
  /*if(!out)
  {
    cout << "Error opening local copy for writing." << endl;
    // clean up if failed
    delete server_uri;
    exit(1);
  }*/

  // Define some variables to use in downloading the content
  int bytes_written = 0;
  int bytes_left;
  int total_data;
  int bytes_read;

  // Define a string to store all the data without clearing the previous data
  string data_string;

  // Define a buffer for the segments of the video file. I got the size of
  // the buffer from the test_client, however I am not exactly sure where
  // this number comes from.

	//cout << "Content length is: " << content_len << endl;

  // content_len is defined above.
  bytes_left = content_len;

  do {
	//fwrite(response_body.c_str(), 1, response_body.length(), out);
 	bytes_written += response_body.length();
	bytes_left -= response_body.length();

	//cout << "bytes written is: " << bytes_written << endl;
	//cout << "bytes left is: " << bytes_left << endl;
	//cout << "response body length is: " << response_body.length() << endl;
  // Add elements to the segment buffer
  //segment_buffer

  // Add the number of bytes for this segment to the vector

  // Append the response from the socket to the data_string before clearing the
  // response_body.
  data_string.append(response_body.c_str(), response_body.length());

	response_body.clear();
	try
	{
		//response->receive_data(client_sock, response_body, bytes_left);
    bytes_read = client_sock.read_data(response_body, bytes_left);
	}
	catch (string msg)
	{
		cout << msg << endl;
		delete response;
		delete server_uri;
		//fclose(out);
		client_sock.Close();
		exit(1);
	}
  } while (bytes_left > 0);

  //cout << "The data string is: " << data_string << endl;
  //cout << "The size of the data string is: " << data_string.length() << endl << endl;

  //cout << "End of getting rest of body and storing it" << endl;

  /*** END OF GETTING THE REST OF THE MESSAGE BODY AND STORING IT ***/

  // Now try to parse the playlist
  playlist = Playlist::Parse(data_string.c_str(), data_string.length());
  cout << data_string << endl;

  if( playlist == NULL)
  {
    cout << "Error, there was a problem downloading the playlist." << endl;
    exit(1);
  }

  // Used for debugging
  cout << "The playlist contains " << playlist->Get_num_segments() << " segments." << endl;

  // Get a video player set up so we can see the video.
  Video_Player* player = Video_Player::Create();
  if (player == NULL) {
    cout << "Error initializing video player." << endl;
    return 3;
  }
  //cout << "Player created." << endl;

  player->Start();

  // Now download and stream each of the video segments in the playlist, in order
  int num_segments = playlist->Get_num_segments();
  for( int i = 0; i < num_segments; i++)
  {
		// Now get the URI for the individual segment. There is a method within
    // the playlist class called get_segment_uri which will return the uri
    // for the individual segment.
    string segment_uri = playlist->Get_segment_uri(i);

	  // Parse the playlist URI
		// Handle bad URI
		// Use URI::Parse() to get the proper components from the given string
		URI* seg_uri_obj = URI::Parse(segment_uri);

		// Now check to make sure the URI was parsed correctly
		// Not sure if we need to just check for NULL or if we need to check for just
		// a forward slash, or an undefined port also
		if( seg_uri_obj == NULL)
		{
		  cout << "There was an error parsing the URI." << endl;
		  exit(1);
		}
		//cout << "Server uri is good" << endl;


		// Download the playlist at that URI.
		// Parse it together, too.

		// TCP_Socket class to handle TCP communications.
		TCP_Socket seg_sock;

		try
		{
		  // Connect to the target server.
		  seg_sock.Connect(*seg_uri_obj);
		}
		// Give up if sock is not created correctly.
		catch(string msg)
		{
		  cout << msg << endl;
		  cout << "Unable to connect to server: "
		       << seg_uri_obj->Get_host() << endl;
		  delete seg_uri_obj;
		  exit(1);
		}

		// Send a GET request for the specified file.
		HTTP_Request* seg_request = HTTP_Request::Create_GET_request(seg_uri_obj->Get_path());
		seg_request->Set_host(seg_uri_obj->Get_host());

		// Print the request to a string so we can send it to the HTTP server
		string seg_request_str;
		seg_request->Print(seg_request_str);

		// Try to send the data to the server. If there is an error, then we will
		// print the error to the screen and then exit.
		try
		{
		  seg_sock.write_string(seg_request_str);
		}
		catch(string msg)
		{
		  cerr << msg << endl;
		  exit(1);
		}

		// Setup two strings for the response header and the response body from the
		// server.
		string seg_response_header, seg_response_body;

		// Now call read_header to get the proper information from the socket
		seg_sock.read_header(seg_response_header, seg_response_body);

		// The HTTP_Response::parse construct a response object, and check if
		// the response is constructed correctly.
		HTTP_Response* seg_response = HTTP_Response::Parse(seg_response_header.c_str(), seg_response_header.length());

		// Now make sure the response is legal
		if( seg_response == NULL)
		{
		  cerr << "Unable to parse the response header." << endl;
		  // clean up if there's something wrong
		  delete seg_response;
		  delete seg_uri_obj;
		  exit(1);
		}

		// Even if the response is constructed correctly, we still need to check for
		// the proper status of the server
		unsigned seg_status_code;

		seg_status_code = seg_response->Get_code();

		// Make sure the status is 200. If not then we need to print an error
		if( seg_status_code != 200)
		{
		  string seg_status_reason = seg_response->Get_reason();

		  cout << "Request failed:" << endl;
		  cout << seg_status_code << " " << seg_status_reason << endl;
		  delete seg_response;
		  exit(1);
		}

		// Create a variable for the content length
		int seg_content_len = 0;

		// Also check to make sure the response is in a form we can handle
		string seg_tmp;
		seg_response->Get_header_value("Transfer-Encoding", seg_tmp);
		if(seg_tmp.find("chunked") != string::npos) {
		  //response->chunked = true;
		  cout << "Unable to read chunked encoding." << endl;
		  delete seg_response;
		  exit(1);
		}
		else{
		  //response->chunked = false;
		  seg_tmp.clear();
		  seg_response->Get_header_value("Content-Length", seg_tmp);
		  istringstream s(seg_tmp);
		  s >> seg_content_len;
		  //s >> response->content_len;
		}

		// Do a check to see if content length is greater than 0, which will
		// signify if it has been changed or not.
		if( seg_content_len <= 0)
		{
		  cout << "Error, no content was downloaded." << endl;
		  exit(1);
		}

    // Add in another check to see if we are downloading some content after the
    // header unintentionally. If the data buffer had any content added to it
    // from the read_header, then we need to append that data to the response header buffer
    if( seg_response_body.length() > 0)
    {
      // I think we can actually just send this right to the video player
      player->Stream(seg_response_body.c_str(), seg_response_body.length());
      cout << seg_response_body.size() << "!!!!" << endl;

      //seg_response_header.append(seg_response_body.c_str(), seg_response_body.length());
    }

		/*** END OF RECEIVE RESPONSE HEADER FROM SERVER ***/

		//cout << "end of receive response header from server, get rest of body and store it" << endl;

		/*** GET REST OF THE MESSAGE BODY AND STORE IT ***/

		/* I'm not actually sure if we need to open a new file here or not. I will
		look at this more tomorrow. */

		// Open a local copy in which to store the file.
		//out = Open_local_copy(server_uri);
		// check
		/*if(!out)
		{
		  cout << "Error opening local copy for writing." << endl;
		  // clean up if failed
		  delete server_uri;
		  exit(1);
		}*/

		// Define some variables to use in downloading the content
		int seg_bytes_written = 0;
		int seg_bytes_left;
		int seg_total_data;
		int seg_bytes_read;

		// Define a string to store all the data without clearing the previous data
		string seg_data_string;
		stringstream seg_stream;

		// Define a buffer for the segments of the video file. I got the size of
		// the buffer from the test_client, however I am not exactly sure where
		// this number comes from.

		// content_len is defined above.
		seg_bytes_left = seg_content_len;


		do {
		//fwrite(response_body.c_str(), 1, response_body.length(), out);
		int num_of_seg_bytes = seg_response_body.size();
	 	seg_bytes_written += num_of_seg_bytes; //seg_response_body.length();
		seg_bytes_left -= num_of_seg_bytes; //seg_response_body.length();
                cout << "seg_bytes_left: " << seg_bytes_left << endl;
                if(seg_bytes_left == 0) { break; }

		// Append the response from the socket to the data_string before clearing the
		// response_body.
		//seg_data_string.append(seg_response_body.c_str(), num_of_seg_bytes); //seg_response_body.length());
		//seg_stream >> seg_response_body;

		seg_response_body.clear();
		try
		{
                  cout << "stuck here" << endl;
		  seg_bytes_read = seg_sock.read_data(seg_response_body, 40960);//seg_bytes_left);
		  cout << "seg bytes read: " << seg_bytes_read << endl;
		  cout << "num_of_seg_bytes: " << num_of_seg_bytes << endl;
                  cout << seg_bytes_left << endl;
 
      // This is a quick test. What if we just send to the player here? Maybe we don't need to get
      // all of the data into a buffer here.
      player->Stream(seg_response_body.c_str(), seg_bytes_read);
		}
		catch (string msg)
		{
			cout << msg << endl;
			delete seg_response;
			delete seg_uri_obj;
			//fclose(out);
			seg_sock.Close();
			exit(1);
		}
		} while (seg_bytes_left > 0);

		/*** END OF GETTING THE REST OF THE MESSAGE BODY AND STORING IT ***/

















    // Now get the URI for the individual segment. There is a method within
    // the playlist class called get_segment_uri which will return the uri
    // for the individual segment.
    //string segment_uri = playlist->Get_segment_uri(i);

    // For testing purposes
    //cout << "the segment uri for " << i << " is: " << segment_uri << endl;

    //int segment_length = playlist->Get_segment_duration(i);
    //int segment_length = segment_bytes[i];

    // For testing
    //cout << "the segment length for " << i << " is: " << segment_length << endl;

    // Now we need to print to the screen that we are obtaining the next segment
    cout << "Fetching segment " << i << endl;
  }

  // Won't know how long until it ends because it is in separate thread.
  // Therefore we wait until window is closed by user to guarantee video is not running.
  player->Wait_for_close();
  // Clean up.
  delete player;

  return 0;
}
