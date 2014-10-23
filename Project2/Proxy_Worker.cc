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

    // TODO: get request from client

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
    // TODO: check if the request is valid.

    cout << "Done. The request is valid." << endl;

    // Check if the request contains our tag by checking the header field
    // SUBLIMINAL_POPPED. (You can use HTTP_Request::get_header_value() to
    // get the valud of the field. Please refer to HTTP_Request for more 
    // information.
    
    // TODO: obtain the value of the header SUBLIMINAL_POPPED
 
    // Check if this request contains the field SUBLIMINAL_POPPOP and if the 
    // request is requesting an html file (using is_html() function)
    if(0/*TODO: check the above condition*/) {
        // If this request does not contain the field SUBLIMINAL_POPPED, the 
        // proxy does not forward this request to the serer. Instead, the proxy
        // returns a subliminal message response to the client.
        subliminal_response(client_request->get_url(), 1);
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
    // TODO:
    // Get the request from the client (HTTP_Request::receive)
    // Chck if the request is received correctly
    //
    // Obtain the server_url from the request (HTTP_Request::get_host 
    // and HTTP_Request::get_path). url = host + path
    // parse the url using URL::parse
    //
}

/*
 * Purpose: Check if the request just gotten is valid
 * Receive: none
 * Return:  the request is valid or not.
 */
bool Proxy_Worker::check_request()
{
    // TODO:
    // 1. Make sure we're pointing to a server URL
    //    Respond a 404 Not Found if the server is invalid
    //    (That is server_url == NULL)
    // 2. Filter out any "host" with the keyword "facebook"
    //    Note that we are filtering out "host" with "facebook".
    //    "path" with facebook is allowed.
    //    Respond a 403 forbidden for host with facebook.
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
