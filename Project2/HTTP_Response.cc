// Modified from CSE422 FS09
// Modified from CSE422 FS12
// Modified from CSE422 FS13
// Modified from CSE422 FS13
//
#include "HTTP_Response.h"

using namespace std;

HTTP_Response::HTTP_Response(unsigned code, const string& status_desc,
    const string& version, const string &data)
{
    set_status_code(code);

    set_version("HTTP/1.1");
    set_header_field("Content-Type", "text/html");
    set_header_field("Server", "MSU/CSE422/FS13");
    set_header_field("Connection", "close");

    content = data;
    stringstream out;
    out << content.size();
    //set_header_field("Content-Length", out.str().c_str());
    set_header_field("Date", build_time().c_str());
}


HTTP_Response::~HTTP_Response() {
    version.clear();
    status_desc.clear();
}

/*
 * Purpose: Parse the response from server to construct a HTTP_Response
 *          Object. Check if the response is formatted correctly.
 * Receive: data: the received data piece
 *          length: the length of the data
 *          NOTE: People parse the response differently. The way they
 *                slice the header varies as well. In this implementation
 *                The header MUST END WITH \r\n\r\n.
 * Return:  a pointer to an HTTP_Response object, if this data is good.
 *          NULL otherwise
 */
// Examines the HTTP response header in the buffer: data. Make sure the
// header is good.
//
// If the request succeeded, the "Content-Length" indicates the length
// of the response body. According to that value, we know how many 
// bytes we need to recevie.
//
// If the request failed, or if the response is not correctly formatted
// return a NULL pointer and release all resource.
HTTP_Response *HTTP_Response::parse(const char* data, unsigned length)
{
    HTTP_Response *response = new HTTP_Response();

    // Separate the opening line (for the response) from the rest.
    const char* first_header = response->find_next_line(data, length);

    if (first_header == NULL)
    {
        // Not even a complete first line...
        delete response;
        return NULL;
    }
    size_t first_line_length = static_cast<size_t>(first_header - data);
    string response_line(data, first_line_length - 2);

    // parse the pieces of the response.
    size_t status_code_pos = response_line.find(" ");
    size_t status_desc_pos = string::npos;
    if (status_code_pos != string::npos)
    {
        response->set_version(response_line.substr(0, status_code_pos));
        status_desc_pos = response_line.find(" ", status_code_pos + 1);
    }
    if (status_desc_pos != string::npos)
    {
        // Note that codes fall within a pretty strict range (which
        // happens to exclude atoi's default 0 return value).
        int status_code = atoi(response_line.substr(status_code_pos + 1,
            status_desc_pos - status_code_pos - 1).c_str());
        if ((status_code < 100) || (status_code >= 600))
        {
            delete response;
            return NULL;
        }

        response->set_status_code(static_cast<unsigned>(status_code));
        response->set_status_desc(response_line.substr(status_desc_pos + 1));
    }
    else
    {
        // Missing fields = bad response.
        delete response;
        return NULL;
    }

    // Have the header lines parsed now; response line is okay.
    // Handled in HTTP_Message.cc
    bool headers_okay = response->parse_fields(first_header, length - first_line_length);
     
    string tmp;
    response->get_header_value("Transfer-Encoding", tmp);
    if(tmp.find("chunked") != string::npos) {
        response->chunked = true;
    }
    else{
        response->chunked = false;
        tmp.clear();
        response->get_header_value("Content-Length", tmp);
        istringstream s(tmp);
        s >> response->content_len;
    }


    if (headers_okay)
    {
        return response;
    }
    else
    {
        delete response;
        return NULL;
    }
}

HTTP_Response* HTTP_Response::create_standard_response(unsigned content_length,
    unsigned code, const string& status_desc, const string& version)
{
    HTTP_Response* response = new HTTP_Response(code, status_desc, version);

    // Assume we're not bothering with chunked/gzipped data.
    response->set_header_field("Content-Encoding", "identity");
    response->set_header_field("Transfer-Encoding", "identity");

    // Also assume that we don't want to have to keep track of connections.
    response->set_header_field("Connection", "close");

    // HTTP requires responses to include the data of construction.
    // Therefore, let's set that.
    char time_buffer[128];
    time_t response_time = time(NULL);
    strftime(time_buffer, sizeof(time_buffer) / sizeof(char),
        "%a, %d %b %Y %H:%M:%S %Z", gmtime(&response_time));
    response->set_header_field("Date", time_buffer);

    // Finally, we know how long the body's going to be, so set that, too.
    ostringstream length_string;
    length_string << content_length;
    response->set_header_field("Content-Length", length_string.str());

    return response;
}

void HTTP_Response::receive_header(TCP_Socket &sock, string &response_header, string &response_body)
{
    sock.read_header(response_header, response_body);
}

int HTTP_Response::receive_data(TCP_Socket &sock, string &response_data, int bytes_left)
{
    if(bytes_left > BUFFER_SIZE)
    {
        return sock.read_data(response_data, BUFFER_SIZE);
    }
    else
    {
        return sock.read_data(response_data, bytes_left);
    }
}

int HTTP_Response::receive_line(TCP_Socket &sock, string &response_data)
{
    return sock.read_line(response_data);
}


const int HTTP_Response::get_content_length() const
{
    int len = 0;
    string len_str;
    if(get_header_value("Content-Length", len_str) == true)
    {
        istringstream conv(len_str);
        conv >> len;
        return len;
    }
    return -1;
}


const string& HTTP_Response::get_version() const
{
    return version;
}


unsigned HTTP_Response::get_status_code() const
{
    return status_code;
}


const string& HTTP_Response::get_status_desc() const
{
    return status_desc;
}

const bool HTTP_Response::is_chunked() const
{
    return chunked;
}

const int HTTP_Response::get_content_len() const
{
    return content_len;
}

const string &HTTP_Response::get_content() const
{
    return content;
}

void HTTP_Response::print(string& output_string) const
{
    output_string.clear();
    // Have the sstream library format the response line for us, since we
    // need to turn the status code back into a string somehow.
    ostringstream response_line;
    response_line << version << " " << status_code << " " << status_desc;

    // Take that and toss on the ending to get the first line...
    output_string = response_line.str();
    output_string += line_ending;
    
    // ...and then add the associated headers.
    HTTP_Message::print(output_string);
}


void HTTP_Response::print(char* output_buffer, unsigned buffer_length) const
{
    // Similar business, though we have to be more choosy with how we
    // apply sstream.
    ostringstream code_str;
    code_str << status_code;

    copy_if_room(output_buffer, version.c_str(), buffer_length);
    copy_if_room(output_buffer, " ", buffer_length);
    copy_if_room(output_buffer, code_str.str().c_str(), buffer_length);
    copy_if_room(output_buffer, " ", buffer_length);
    copy_if_room(output_buffer, status_desc.c_str(), buffer_length);

    copy_if_room(output_buffer, line_ending.c_str(), buffer_length);

    HTTP_Message::print(output_buffer, buffer_length);
}


void HTTP_Response::set_version(const string& version)
{
    this->version = version;
}


void HTTP_Response::set_status_code(const unsigned code)
{
    this->status_code = code;
    build_status();
}


void HTTP_Response::set_status_desc(const string& status_desc)
{
    this->status_desc = status_desc;
}


void HTTP_Response::send(TCP_Socket &sock)
{
    string outgoing_buffer;
    print(outgoing_buffer);
    outgoing_buffer.append(content);
    cout << sock.write_string(outgoing_buffer) << " bytes sent" << endl;
}

void HTTP_Response::send_no_error(TCP_Socket &sock)
{
    send(sock);
}

/*
 * Purpose: Private function that builds a data header field that matches
 *          the spec of HTTP
 * Receive: none
 * Retuen:  none
 */
string HTTP_Response::build_time()
{
    // format a time
    time_t t;
    struct tm *ts;
    char result[38];
    static char wday_name[7][4] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    static char mon_name[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                                   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    // get the time 
    t = time(NULL);
    ts = gmtime(&t);

    //format the time string according to the spec, e.g. Sun, 06 Nov 1994 08:49:37 GMT)
    snprintf(result, 30, "%.3s, %.2d %.3s %d %.2d:%.2d:%.2d GMT",
             wday_name[ts->tm_wday], ts->tm_mday, mon_name[ts->tm_mon],
             1900+ts->tm_year, ts->tm_hour, ts->tm_min, ts->tm_sec);
    return string(result);
}

void HTTP_Response::build_status()
{
    switch(status_code) {
        case 200:
            status_desc = "OK";
            break;
        case 400:
            status_desc = "Bad request";
            break;
        case 403:
            status_desc = "Forbidden";
            break;
        case 404:
            status_desc = "Not Found";
            break;
        case 500:
            status_desc = "Internal server error";
            break;
        case 501:
            status_desc = "Not implemented";
            break;
        case 503:
            status_desc = "Service unavailable";
            break;
        default:
            status_desc = "Code not implemented/recognized";
            break;
    }
}
