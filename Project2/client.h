// Modified from CSE422 FS09
// Modified from CSE422 FS12
// Modified from CSE422 FS14
//

#include "HTTP_Request.h"
#include "HTTP_Response.h"
#include "URL.h"
#include <climits>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <netdb.h>
#include <string>
#include <sstream>
#include <sys/stat.h>

using namespace std;


/*********************************
 * Purpose: prints a brief usage string describing how to use the application,
 *          in case the user passes in something that just doesn't work.
 * Receive: exe_name - the name of the executable
 *          out - the ostream
 * Return:  none
 *********************************/
void help_message(const char* exe_name, ostream& out)
{
    out << "Usage: " << exe_name << " [options]" << endl;
    out << "The following options are available:" << endl;
    out << "    -s host URL" << endl;
    out << "    -p proxy URL" << endl;
    out << "    -h display help message" << endl;
    out << endl;
    out << "Example: " << exe_name 
        << "-s http://www.some_server.com/ -p 100.200.50.150:8080"  << endl;
}

/*********************************
 * Purpose: parse the parameters
 * Receive: argv and argc
 * Return:  return by call by reference
 *          target_URL: the URL we are going to download
 *          proxy_addr: the address of the proxy
 *********************************/
int parse_args(int argc, char *argv[],
               char **url_addr, char **proxy_addr)
{
    for(int i = 1; i < argc; i++)
    {
        if((!strncmp(argv[i], "-s", 2)) ||
           (!strncmp(argv[i], "-S", 2)))
        {
            *url_addr = argv[++i];
        }
        else if((!strncmp(argv[i], "-p", 2)) ||
                (!strncmp(argv[i], "-P", 2)))
        {
            *proxy_addr = argv[++i];
        }
        else if((!strncmp(argv[i], "-h", 2)) ||
                (!strncmp(argv[i], "-H", 2)))
        {
            help_message(argv[0], cout);
            exit(1);
        }
        else
        {
            cerr << "Invalid parameter: argv[i]" << endl;
            help_message(argv[0], cout);
            exit(1);
        }
    }
}

/*********************************
 * Name:    Open_local_copy
 * Purpose: Open a file pointer to store the data in ./Download
 * Receive: url - the url for the object
 * Return:  The file pointer
 *********************************/
// Opens a local copy of the file referenced by the given request URL, for
// writing.  Ignores any directories in the URL path, instead opening the file
// in the current directory.  Makes up a filename if none is given.
//
// Returns a pointer to the open file, or a NULL pointer if the open fails.
FILE* Open_local_copy(const URL* url)
{
    FILE* outfile = NULL;

    struct stat sb; // For checking if ./Download exists

    if(stat("./Download", &sb) == -1) // if ./Download does not exist
    {
         mkdir("./Download", 0700);   // create it
    }

    const string& full_path = url->get_path();
    size_t filename_pos = full_path.rfind('/');
    // find the last '/', the substring after it should be the filename

    if ((filename_pos != string::npos) && // if found a '/'
        ((filename_pos + 1) < full_path.length())) // or / is not the end of
                                                   // the string
    {
        string fn = string("Download/") + full_path.substr(filename_pos + 1);
        outfile =
            fopen(fn.c_str(),"wb");
    }
    else
    {
        outfile = fopen("Download/index.html", "wb");
    }

    return outfile;
}


/*********************************
 * Purpose: for a given data, extract the chunk_len
 * Receive: the data as string
 * Return:  the extracted chunk_len
 *          note that the chunk_len in hex is removed from data string.
 *********************************/
// For the client, it needs to remove the chunk_len from the data string
// because the client is storing the data as a file. The chunk length
// is no longer needed anymore. However, for proxies, they need to keep the 
// chunk length, so that the forwarded response body can be decoded/received
// by the clients.
int get_chunk_size(string &data)
{
    int chunk_len;          // The value we want to obtain
    int chunk_len_str_end;  // The var to hold the end of chunk length string
    std::stringstream ss;   // For hex to in conversion

    chunk_len_str_end = data.find("\r\n"); // Find the first CLRF
    string chunk_len_str = data.substr(0, chunk_len_str_end);
    // take the chunk length string out

    // convert the chunk length string hex to int
    ss << std::hex << chunk_len_str;
    ss >> chunk_len;

    // reorganize the data
    // remove the chunk length string and the CLRF
    data = data.substr(chunk_len_str_end + 2, data.length() - chunk_len_str_end - 2);

    //cout << "chunk_len_str: " << chunk_len_str << endl;
    //cout << "chunk_len:     " << chunk_len << endl;  
    
    return chunk_len;
}
