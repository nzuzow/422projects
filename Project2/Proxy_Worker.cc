// Modified from CSE422 FS09
// Modified from CSE422 FS12
// Modified from CSE422 FS13
//
#include "Proxy_Worker.h"
#include <sstream>
using namespace std;

/*
 * Purpose: constructor
 * receive: cs is the socket that is already connected to the requesting
 *          client
 * Return:  none
 */
Proxy_Worker::Proxy_Worker(TCP_Socket *cs) {
    client_sock = cs;
    port = 80; // For a full blown proxy, the server information should be
               // obtained from each request. However, we simply assume it
               // to be 80 for our labs.
    server_url = NULL;
               // Must be obtain from each request.
    server_response = NULL;
    client_request = NULL;
}

/*
 * Purpose: destructor
 * receive: none
 * Return:  none
 */
Proxy_Worker::~Proxy_Worker() {
    if(server_url != NULL)
    {
        delete server_url;
    }

    if(client_request != NULL)
    {
        delete client_request;
    }

    if(server_response != NULL)
    {
        delete server_response;
    }
    server_sock.Close();
}

/*
 * Purpose: check if the string "fullString" ends with the string "ending"
 * Receive: string fullString is the string to be checked
 *          string ending is the target
 * Return: true if fullString ends with ending, false otherwise
 * Example: string fullString = "1234";
 *          string end34;
 *          string end12;
 *          bool result34 = hasEnding(fullString, end34);
 *          bool result12 = hasEnding(fullString, end12);
 *          // result34 is true
 *          // result12 is false
 */
bool hasEnding (std::string const &fullString, std::string const &ending)
{
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

/*
 * Purpose: check if a path being requested is an html file
 * Receive: string path is the string to be checked
 * Return: true if the path is an html, false otherwise
 */
bool is_html(string path) {
    string end1 = "/";
    string end2 = "html";
    string end3 = "htm";

    if(hasEnding(path, end1) || hasEnding(path, end2) || hasEnding(path, end3)) {
        return true;
    }
    else {
        return false;
    }
}

/*
 * Purpose: handles a request by requesting it from the server_url
 * Receive: none
 * Return:  none
*/
void Proxy_Worker::handle_request() {
    string buffer;

    // Get HTTP request from the client, check if the request is valid by
    // parsing it. (parsing is done using HTTP_Request::receive)
    // Please refer to HTTP_Request.cc for the usage of HTTP_Request::receive
    // and/or HTTP_Request::prarse.
    // From the parsed request, obtain the server address (in code,
    // server_url).
    cout << "New connection established." << endl;
    cout << "New proxy child thread started." << endl;
    cout << "Getting request from client..." << endl;

    // DONE: get request from client

    // Call the get_request function
    bool get_the_request = get_request();

    // Make sure the request was received properly
    if( get_the_request == false)
    {
      // I print an error message in get_request
      exit(1);
    }

    // Just outputting the requrst.
    cout << endl << "Received request:" << endl;
    cout << "=========================================================="
         << endl;
    client_request->print(buffer);
    cout << buffer.substr(0, buffer.length() - 4) << endl;
    cout << "=========================================================="
         << endl;

    // Check if the request is valid
    // Terminate this Proxy_Worker if it is not a valid request
    cout << "Checking request..." << endl;

    // DONE: check if the request is valid.
    bool check_the_request = check_request();

    if( check_the_request == false)
    {
      // I print the error message in check_request
      exit(1);
    }

    cout << "Done. The request is valid." << endl;

    // Check if the request contains our tag by checking the header field
    // SUBLIMINAL_POPPED. (You can use HTTP_Request::get_header_value() to
    // get the valud of the field. Please refer to HTTP_Request for more
    // information.

    // DONE: obtain the value of the header SUBLIMINAL_POPPED

    string header_contain;

    bool sub_popped = client_request->get_header_value("SUBLIMINAL_POPPED", header_contain);

    // Check if this request contains the field SUBLIMINAL_POPPOP and if the
    // request is requesting an html file (using is_html() function)
    if(sub_popped == 0 && is_html(client_request->get_url())) { //DONE: check the above condition
        // If this request does not contain the field SUBLIMINAL_POPPED, the
        // proxy does not forward this request to the serer. Instead, the proxy
        // returns a subliminal message response to the client.

        //if( is_html(client_request->get_url()))
        //{
          subliminal_response(client_request->get_url(), 1);
        //}
    } else {
        // If this request contains the field SUBLIMNAL_POPPED, the request has
        // been served before. The proxy handles the request like a normal proxy.
        //
        // Forward the request to the server.
        // Receive the response header and modify the server header field
        // Receive the response body. Handle the default and chunked transfor
        // encoding.
        cout << endl << "Forwarding request to server..." << endl;
        // TODO: forward the request to the server and get the response from
        //       the server
        // TODO: return the response to the client

        // Send the request to the server
        bool forward_request = forward_request_get_response();

        // Return the response to the client
        //bool send_response = return_response();

        cout << "Connection served. Proxy child thread terminating." << endl;
    }

    return;
}

/*
 * Purpose: receives the request from a client and parse it.
 * Receive: none
 * Return:  a boolean indicating if getting the request was succesful or
 *          not
 */
bool Proxy_Worker::get_request() {
    // DONE:
    // Get the request from the client (HTTP_Request::receive)
    // Chck if the request is received correctly
    //
    // Obtain the server_url from the request (HTTP_Request::get_host
    // and HTTP_Request::get_path). url = host + path
    // parse the url using URL::parse
    //

    // Create a flag to return at the end
    bool flag = 1;

    // The clients socket is stored in client_sock
    client_request = HTTP_Request::receive(*client_sock);

    //string requested_address;
    //client_request->HTTP_Request::get_host(requested_address);

    //cout << "in get request the requested address is: " << requested_address << endl;

    //string requested_path;
    //requested_path = client_request->HTTP_Request::get_path();

    //cout << "in get request the requested path is: " << requested_path << endl;

    // Checking if the request was received correctly
    if( client_request == NULL)
    {
      cerr << "The request was not received correctly" << endl;
      flag = 0;
    }

    return flag;
}

/*
 * Purpose: Check if the request just gotten is valid
 * Receive: none
 * Return:  the request is valid or not.
 */
bool Proxy_Worker::check_request()
{
    // DONE:
    // 1. Make sure we're pointing to a server URL
    //    Respond a 404 Not Found if the server is invalid
    //    (That is server_url == NULL)
    // 2. Filter out any "host" with the keyword "facebook"
    //    Note that we are filtering out "host" with "facebook".
    //    "path" with facebook is allowed.
    //    Respond a 403 forbidden for host with facebook.

    // Call get_host to get the server address
    string requested_address;
    client_request->HTTP_Request::get_host(requested_address);

    cout << "The client address is: " << requested_address << endl;

    string second_request_url = client_request->HTTP_Request::get_url();
    cout << "The url from get url is: " << second_request_url << endl;

    //Need to check if this is a request to facebook
    if( requested_address.find("facebook") != string::npos)
    {
      bool valid_address = proxy_response(403);
      return 0;
    }

    // Call get_path to get the path of the file
    string requested_path;
    requested_path = client_request->HTTP_Request::get_path();

    cout << "The client path is: " << requested_path << endl;

    // Put the address and the path together
    string requested_url = requested_address + requested_path;

    cout << "The full path is: " << requested_url << endl;

    // Pass the client address to URL::Parse to check the validity of the server
    server_url = URL::parse(requested_url);
	  cout << "requested url: " << requested_url << endl;

    // Check the validity of the URL object
    if( server_url == NULL)
    {
      bool valid_url = proxy_response(404);
      return 0;
    }

    // Create a connection to the server url on the server socket
    //server_sock.Connect(*server_url);

    try
    {
        // Create a connection to the server url on the server socket
    	server_sock.Connect(*server_url);
    }
    catch(string msg)
    {
        bool valid_address = proxy_response(404);
        return 0;
    }

    return 1;
}

/*
 * Purpose: Forwards a client request to the server and get the response
 *          1. Forward the request to the server
 *          2. Receive the response header and modify the server field
 *          3. Receive the response body. Handle both chunk/default encoding.
 * Receive: none
 * Return:  a boolean indicating if forwarding the request was succesful or not
 */
bool Proxy_Worker::forward_request_get_response() {
    // TODO: Pass the client request to the server and get response from the
    //       server. This is the most difficult part of this lab. However, you
    //       can find all information you need in client.cc

    client_request = HTTP_Request::create_GET_request(server_url->get_path());
    client_request->set_host(server_url->get_host());
    // This request is non-persistent.
    client_request->set_header_field("Connection", "close");
    // For real browsers, If-Modified-Since field is always set.
    // if the local object is the latest copy, the browser does not
    // respond the object.
    client_request->set_header_field("If-Modified-Since", "0");

    try
    {
        //client_request->send(*client_sock);
        client_request->send(server_sock);
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
    client_request->print(print_buffer);
    cout << print_buffer.substr(0, print_buffer.length() - 4) << endl;
    cout << "=========================================================="
         << endl;

    //delete client_request; // We do not need it anymore


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

    //server_response->receive_header(*client_sock, response_header, response_body);
    server_response->receive_header(server_sock, response_header, response_body);



    // The HTTP_Response::parse construct a response object. and check if
    // the response is constructed correctly. Also it tries to determine
    // if the response is chunked or not. This program does not handle
    // chunked encoded transfer.
    server_response = HTTP_Response::parse(response_header.c_str(),
                                    response_header.length());

    // The response is illegal.
    if(server_response == NULL)
    {
        cerr << "Unable to parse the response header." << endl;
        // clean up if there's something wrong
        delete server_response;
        /*if(proxy_url != NULL){
            delete proxy_url;
        }*/
        delete server_url;
        exit(1);
    }

    // output the response header
    cout << endl << "Response header received" << endl;
    cout << "=========================================================="
         << endl;
    server_response->print(print_buffer);
    cout << print_buffer.substr(0, print_buffer.length() - 4) << endl;
    cout << "=========================================================="
         << endl;

    // Modify the response from the server to show this worked
    //server_response->set_header_field("Server", "zuzownic");



    /***GET REST OF THE MESSAGE BODY AND STORE IT***/
    // Open a local copy in which to store the file.

    //out = Open_local_copy(server_url);

    // check
    /*if(!out)
    {
        cout << "Error opening local copy for writing." << endl;
        // clean up if failed
        if(proxy_addr != NULL)
        {
            delete proxy_url;
        }
        delete server_url;
        exit(1);
    }*/

    //cout << endl << "Downloading rest of the file ... " << endl;

    int bytes_written = 0, bytes_left;
    int total_data;

    if(server_response->is_chunked() == false)
    {
        // none-chunked encoding transfer does not split the data into
        // chunks. The header specifies a content_length. The client knows
        // exactly how many data it is expecting. The client keeps receiving
        // the response until it gets the amount.

        cout << "Default encoding transfer" << endl;
        cout << "Content-length: " << server_response->get_content_len() << endl;
        //bytes_left = server_response->get_content_len();


        //do
        //{
            // If we got a piece of the file in our buffer for the headers,
            // have that piece written out to the file, so we don't lose it.

            //fwrite(response_body.c_str(), 1, response_body.length(), out);

            // Try sending directly to the client and not writing to a file.
            //bool send_response = return_response();
            /*try
            {
                server_response->send(*client_sock);
                //server_response->send(server_sock);
            }
            catch(string msg)
            {
                cerr << msg << endl;
                exit(1);
            }*/

          /*  bytes_written += response_body.length();
            bytes_left -= response_body.length();

          //  cout << "bytes written:" <<  bytes_written << endl;
          //  cout << "data gotten:" <<  response_body.length() << endl;

          //  response_body.clear();
            //try
            //{
                // Keeps receiving until it gets the amount it expects.
                //server_response->receive_data(*client_sock, response_body,
                //                       bytes_left);
                  //server_response->receive_data(server_sock, response_body,
                  //                       bytes_left);
                  //std::string response_data = server_response->get_content();
            //}
            catch(string msg)
            {
                // something bad happend
                cout << msg << endl;
                // clean up
                delete server_response;
                delete server_url;
                //if(proxy_addr != NULL)
                //{
                //    delete proxy_url;
                //}
                //fclose(out);
                //client_sock.Close();
                client_sock->Close();
                server_sock.Close();
                exit(1);
            }
        } while (bytes_left > 0);*/


        //server_response = HTTP_Response::create_standard_response(server_response->get_content_len(), 200, "OK", "HTTP/1.1");

        // Try sending directly to the client and not writing to a file.
        bool send_response = return_response();

	      // write the response to the client
	      int write_response = client_sock->write_string(response_body);

    }
    else
    {
      std::cout << "This is a test. It did not hit the if statement" << std::endl;
    }






    return true;
}

/*
 * Purpose: Return the response from the server to the client
 *          Also modify the server field.
 * Receive: none
 * Return:  a boolean indicating if returning the request was succesful or not
 *          (always true for now)
*/
bool Proxy_Worker::return_response() {
    // TODO: return the response to the client. However, we want to modify the
    //       header field "Server" to something else (anything). You can use
    //       the function HTTP_Response::set_header_field to do this.

    // Modify the response from the server to show this worked
    server_response->set_header_field("Server", "zuzownic");

    try
    {
        // Output the Response
        cout << endl;
        cout << "Response sent..." << endl;
        cout << "=========================================================="
             << endl;
        string print_buffer;
        server_response->print(print_buffer);
        cout << print_buffer.substr(0, print_buffer.length() - 4) << endl;
        cout << "=========================================================="
             << endl;

	      server_response->send_no_error(*client_sock);
        //server_response->send(*client_sock);
        //server_response->send(server_sock);
    }
    catch(string msg)
    {
        cerr << msg << endl;
        exit(1);
    }

    return true;
}

/*
 * Purpose: Create a response "locally" and return it to a client
 *          For error situations like 403, 404, and 500 .. etc
 * Receive: the error code
 * Return:  a boolean indicating if returning the request was succesful
 *          or not (always true for now)
*/
bool Proxy_Worker::proxy_response(int status_code) {
    string buffer;
    HTTP_Response proxy_res(status_code);
    stringstream ss;
    int content_length = int(proxy_res.content.length());
    ss << content_length;

    proxy_res.set_header_field("Content-Length", ss.str());
    cout << endl << "Returning " << status_code << " to client ..." << endl;
    cout << "=========================================================="
         << endl;
    buffer.clear();
    proxy_res.print(buffer);
    cout << buffer.substr(0, buffer.length() - 4) << endl;
    cout << "=========================================================="
         << endl;
    proxy_res.send_no_error(*client_sock);
    return true;
}

/*
 * Purpose: Create a "subliminal message" response "locally" and return
 *          it to a client.
 * Receive: string url: the original url string.
 *          int duration: the duration of the subliminal message
 * Return:  a boolean indicating if returning the request was succesful
 *          or not (always true for now)
*/
bool Proxy_Worker::subliminal_response(const string url, int duration) {
    string buffer;
    // create a new HTTP_Response
    HTTP_Response proxy_res(200);
    // Randomly choose a image
    int fignumber = rand() % 4;
    stringstream ss;
    ss << fignumber;
    string fignumber_str = ss.str();
    ss.str("");
    ss << duration;
    string duration_str = ss.str();

    // create a webpage containing the image and automatically redirects to
    // original url in "duration" seconds
    char p = 156;
    proxy_res.content = "<html><head><meta http-equiv=\"refresh\" content=\"" + duration_str + ";url=" + url + "CSE422\" /></head><body><center><font size=72>GO GREEN! GO WHITE!</font><br><img src=\"http://www.cse.msu.edu/~cse422b/fs14/lab2/" + fignumber_str + ".jpg\" width=1000px><br>Redirecting...</center></body></html>";
    ss.str("");
    int content_length = int(proxy_res.content.length());
    ss << content_length;

    proxy_res.set_header_field("Content-Length", ss.str());
    cout << endl << "Returning subliminal to client ..." << endl;
    cout << "=========================================================="
         << endl;
    buffer.clear();
    proxy_res.print(buffer);
    cout << buffer.substr(0, buffer.length() - 4) << endl;
    cout << "=========================================================="
         << endl;
    proxy_res.send_no_error(*client_sock);
    return true;
}

/*
 * Purplse: Extract the chunk size from a string
 * Receive: the string
 * Return:  the chunk size in int
 *          Note that the chunk size in hex is removed from the string.
 */
// You can either remove the hex chunk size or leave it in the data string.
// Both is fine.
int Proxy_Worker::get_chunk_size(string &data)
{
    int chunk_len;          // The value we want to obtain
    int chunk_len_str_end;  // The var to hold the end of chunk length string
    std::stringstream ss;   // For hex to in conversion

    chunk_len_str_end = data.find("\r\n"); // Find the first CRLF
    string chunk_len_str = data.substr(0, chunk_len_str_end);
    // take the chunk length string out

    // convert the chunk length string hex to int
    ss << std::hex << chunk_len_str;
    ss >> chunk_len;

    // reorganize the data
    // remove the chunk length string and the CRLF
    data = data.substr(chunk_len_str_end + 2,
                       data.length() - chunk_len_str_end - 2);


    //cout << "chunk_len_str: " << chunk_len_str << endl;
    //cout << "chunk_len:     " << chunk_len << endl;
    return chunk_len;
}
