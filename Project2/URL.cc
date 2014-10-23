// Modified from CSE422 FS09
// Modified from CSE422 FS12
// Modified from CSE422 FS14
//

#include "URL.h"
#include <cstdlib>
#include <sstream>

using namespace std;


namespace
{
    // Used to signal when the port number is not known.  There is an
    // excellent chance that this will never be a valid port for anything.
    const unsigned UNDEFINED_PORT = 0xffffffff;
}

/*********************************
 * Name:    URL
 * Purpose: constructor of URL class objects
 * Reecive: none
 * Return:  none
 *********************************/
// We want to assume that the port hasn't been set until we know otherwise.
// We also want to make sure that there's some kind of path, since HTTP
// requires a path.  The root path is the accepted default there.
URL::URL()
    : port(UNDEFINED_PORT),
    path("/")
{
}

/*********************************
 * Name:    ~URL
 * Purpose: destructor of URL class objects
 * Receive: none
 * Return:  none
 *********************************/
URL::~URL()
{
    // Nothing to do...
}

/*********************************
    listen_sock.get_port(port);
    listen_sock.get_port(port);
 * Name:    parse
 * Purpose: parse the url string and create an URL object
 * Receive: url_string: the URL string to be parsed
 * Return:  pointer to the URL object
 *********************************/
URL* URL::parse(const string& url_string)
{

    URL* new_url = new URL();

    // Obtain the protocol from url_string
    size_t offset = new_url->read_protocol(url_string);
    if (offset == string::npos)
    {
        delete new_url;
        return NULL;
    }

    // Obtain the port from url_string
    offset = new_url->read_host_port(url_string, offset);

    // if the offset has not yet read the end of the url string,
    // get the path
    if (offset < url_string.length())
    {
        // Obtain the path
        new_url->read_path_details(url_string, offset);

        // If the client somehow input a URL with an empty path,
        // quietly save them from themselves.
        if (new_url->path.length() == 0)
        {
            new_url->path = "/";
        }
    }

    return new_url;
}

const string& URL::get_protocol() const
{
    return protocol;
}

const string& URL::get_host() const
{
    return host;
}

bool URL::is_port_defined() const
{
    return (port != UNDEFINED_PORT);
}

unsigned URL::get_port() const
{
    return port;
}

const string& URL::get_path() const
{
    return path;
}

const string& URL::get_query() const
{
    return query;
}

const string& URL::get_fragment() const
{
    return fragment;
}

void URL::Print(ostream& out)
{
    // Say the URL is http://www.example.org:8080/example.php?example#ex
    // Each piece follows.  Note that we should avoid printing optional
    // parts of the URL that have associated formatting characters, if
    // they aren't actually defined.

    // http://
    out << protocol << "://";

    // www.example.org
    out << host;

    // :8080 (if given)
    if (is_port_defined())
        out << ":" << port;

    // /example.php
    out << path;

    // ?example(if given)
    if (query.length() > 0)
        out << "?" << query;

    // #ex (if given)
    if (fragment.length() > 0)
        out << "#" << fragment;
}

void URL::Print(string& target)
{
    // Much easier than duplicating the code.
    ostringstream target_out;
    Print(target_out);
    target = target_out.str();
}

/*********************************
 * Name:    read_protocol
 * Purpose: read the protocol from the URL string
 * Receive: url_string: the URL string to be parsed 
 *          offset:     the offset, the position to start parsing
 *                      default = 0
 * Return:  the offset indicates that the part before this offset
 *          has been parsed
 *********************************/
size_t URL::read_protocol(const string& url_string, size_t offset)
{
    size_t protocol_end = url_string.find("://", offset);

    if (protocol_end == string::npos) // If protocol is not specified
    {                                 // assume it is http
        protocol = "http";
        return 0;
    }
    else{
        protocol = url_string.substr(offset, protocol_end - offset);
        return protocol_end + 3;
    }
}

/*********************************
 * Name:    read_host_port
 * Purpose: read the port from the URL string, if specified
 * Receive: url_string: the URL string to be parsed
 *          offset:     the offset, the position to start parsing
 * Return:  the offset indicates that the part before this offset
 *          has been parsed
 *********************************/
size_t URL::read_host_port(const string& url_string, size_t offset)
{
    size_t part_end = url_string.find_first_of("/#?", offset);
    if (part_end == string::npos)
        part_end = url_string.length();

    size_t port_offset = url_string.find(":", offset);
    if ((port_offset == string::npos) || (port_offset > part_end))
        port_offset = part_end;

    host = url_string.substr(offset, port_offset - offset);
    if (port_offset < part_end)
    {
        port = atoi(url_string.substr(port_offset + 1,
            part_end - port_offset - 1).c_str());
    }

    return part_end;
}

/*********************************
 * Name:    read_host_port
 * Purpose: read the path from the URL string, if specified
 * Receive: url_string: the URL string to be parsed
 *          offset:     the offset, the position to start parsing
 * Return:  the offset indicates that the part before this offset
 *          has been parsed
 *********************************/
size_t URL::read_path_details(const string& url_string, size_t offset)
{
    size_t unparsed_end = url_string.length();

    // Once you hit the beginning of the fragment, that's the end of the
    // URL.  Since it's nice to know where our limits are, let's check for
    // that first.
    size_t fragment_offset = url_string.find("#", offset);
    if (fragment_offset != string::npos)
    {
        fragment = url_string.substr(fragment_offset + 1);
        unparsed_end = fragment_offset;
    }

    size_t query_offset = url_string.find("?", offset);
    if ((query_offset != string::npos) && (query_offset < unparsed_end))
    {
        query = url_string.substr(query_offset + 1,
            unparsed_end - query_offset - 1);
        unparsed_end = query_offset;
    }

    path = url_string.substr(offset, unparsed_end - offset);

    return url_string.length();
}


void URL::set_protocol(const string& protocol)
{
    this->protocol = protocol;
}

void URL::set_host(const string& host)
{
    this->host = host;
}

void URL::Clear_port()
{
    port = UNDEFINED_PORT;
}

void URL::set_port(unsigned port)
{
    this->port = port;
}

void URL::set_query(const string& query)
{
    this->query = query;
}

void URL::set_fragment(const string& fragment)
{
    this->fragment = fragment;
}
