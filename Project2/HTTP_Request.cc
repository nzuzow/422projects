// Modified from CSE422 FS09
// Modified from CSE422 FS12
// Modified from CSE422 FS13

#include "HTTP_Request.h"

using namespace std;

HTTP_Request::HTTP_Request(const string& method, const string& url,
    const string& version)
    : method(method),
    url(url),
    version(version)
{
}

HTTP_Request::~HTTP_Request()
{
    method.clear();
    url.clear();
    version.clear();
}

HTTP_Request *HTTP_Request::receive(TCP_Socket &sock)
{
    HTTP_Request *request;
    string incoming_str;
    string tmp;
    int zero_count = 0;

    sock.read_line(incoming_str);

    while(tmp != "\r\n")
    {
        incoming_str += tmp;
        tmp.clear();
        if(sock.read_line(tmp) == 0)
        {
            zero_count++;
            if(zero_count >= 1000)
            {
                break;
            }
        }
    }
    incoming_str.append("\r\n");
    //cout << incoming_str << endl;

    request = HTTP_Request::parse(incoming_str.c_str(), incoming_str.length());
    return request;
}

HTTP_Request* HTTP_Request::parse(const char* data, unsigned length)
{
    // Separate the opening line (for the request) from the rest.
    HTTP_Request* request = new HTTP_Request();
    const char* first_header = request->find_next_line(data, length);
    if (first_header == NULL)
    {
        // Ouch, not even a complete first line...
        
        delete request;
        return NULL;
    }

    size_t first_line_length = static_cast<size_t>(first_header - data);

    // Figure out that opening request line.  Look for the spaces that
    // separate the method, URL, and version.  Set as appropriate.
    string request_line(data, first_line_length - 2);

    size_t url_pos = request_line.find(" ");
    size_t version_pos = string::npos;
    if (url_pos != string::npos)
    {
        request->set_method(request_line.substr(0, url_pos));
        version_pos = request_line.find(" ", url_pos + 1);
    }

    string path;

    if (version_pos != string::npos)
    {
        path = request_line.substr(url_pos + 1, version_pos - url_pos - 1);
        // We are not sure if the path field here is the whole URL or just the path.
        // URL = http://host/path
        // For example: http://www.cse.msu.edu/~cse422
        //              host: www.cse.msu.edu
        //              path: ~cse422
        // We will parse it later when we get the host.

        int pos = path.find("CSE422");
      
        if(pos != string::npos && path.substr(pos, 6).compare("CSE422") == 0) {
            request->set_header_field("SUBLIMINAL_POPPED", "True");
            path = path.substr(0, pos);
        }
        request->set_version(request_line.substr(version_pos + 1));
    }
    else
    {
        // If we couldn't get those three fields out of it, it's a bad
        // request, and we should stop trying to handle it.
        delete request;
        return NULL;
    }

    // Go on and handle the remaining header lines in the request.  If
    // they're good, we're good.  If not...
    bool headers_okay = request->parse_fields(first_header,
        length - first_line_length);

    string host;
    request->get_host(host);
    // Get rid of the extra "http://" in path field.
    int pos = path.find("http://");
    if(pos != string::npos) {
        path.replace(pos, 7, "");
    }
    // Get rid of the extra host field in path field
    pos = path.find(host);
    if(pos != string::npos) {
        path.replace(pos, host.length(), "");
    }
    request->set_path(path);

    if (headers_okay)
    {
        return request;
    }
    else
    {
        delete request;
        return NULL;
    }
}


HTTP_Request* HTTP_Request::create_GET_request(const string& url,
    const string& version)
{
    HTTP_Request* request = new HTTP_Request("GET", url, version);

    // Ask the server to either send the file data uncompressed, or not
    // bother sending it at all.  I don't think anyone plans on tying
    // gzip into this lab.

    return request;
}

void HTTP_Request::send(TCP_Socket &sock)
{
    string outgoing_buffer;
    print(outgoing_buffer);
    sock.write_string(outgoing_buffer);
}

const string& HTTP_Request::get_method() const
{
    return method;
}


const string& HTTP_Request::get_path() const
{
    return url;
}

const string HTTP_Request::get_url() const
{
    string url_string = "http://";
    string server_host, server_path;
    get_host(server_host);
    server_path = get_path();
    url_string += server_host;
    url_string += server_path;
    return url_string;
}


const string& HTTP_Request::get_version() const
{
    return version;
}


void HTTP_Request::get_host(string& out_host) const
{
    if (!get_header_value("Host", out_host))
    {
        out_host = "";
    }
}


void HTTP_Request::print(string& output_string) const
{
    output_string.clear();
    // Throw in our one request line.
    output_string = method;
    output_string += ' ';
    output_string += url;
    output_string += ' ';
    output_string += version;

    output_string += line_ending;

    // Now have all the headers thrown in on top of that.
    HTTP_Message::print(output_string);
}


void HTTP_Request::print(char* output_buffer, unsigned buffer_length) const
{
    // Similar model, except with a character buffer.
    copy_if_room(output_buffer, method.c_str(), buffer_length);
    copy_if_room(output_buffer, " ", buffer_length);
    copy_if_room(output_buffer, url.c_str(), buffer_length);
    copy_if_room(output_buffer, " ", buffer_length);
    copy_if_room(output_buffer, version.c_str(), buffer_length);

    copy_if_room(output_buffer, line_ending.c_str(), buffer_length);

    HTTP_Message::print(output_buffer, buffer_length);
}


void HTTP_Request::set_method(const string& method)
{
    this->method = method;
}


void HTTP_Request::set_path(const string& url)
{
    this->url = url;
}


void HTTP_Request::set_version(const string& version)
{
    this->version = version;
}


void HTTP_Request::set_host(const string& host)
{
    set_header_field("Host", host);
}
