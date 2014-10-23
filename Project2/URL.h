// Modified from CSE422 FS09
// Modified from CSE422 FS12
// Modified from CSE422 FS14


// URL - Represents most aspects of a uniform resource identifier (URL).  Can
// be used to parse existing URL strings into their component parts, and to form
// new URL strings piece-by-piece from those components.
//
// Expects URLs to be formatted in the following manner (note that most fields
// are optional, depending on which other fields are also present):
//
// protocol://host:port/path?query#fragment

#ifndef _URL_H_
#define _URL_H_

#include <iostream>
#include <string>


class URL
{
public:
    URL();
    ~URL();

    // Creates a new URL based on the contents of the given string.
    // A few things to note about how the parsing is done:
    //  + The protocol *must* be specified.  "http://example.org" will
    //    parse. http:// will be the default protocl if not given
    //  + If no port number is given in the URL, the returned URL object
    //    will have the port clearly indicates as being undefined.
    //  + If no path is given in the URL, it will be set to a forward slash
    //    i("/"), to avoid having a blank string there.
    //
    //
    // Name:    parse
    // Purpose: As described above
    // Recevie: url_string - The URL string to parse.
    // Return:  A URL object with its components taken from the given
    //          string. If the given string is not formatted like a proper 
    //          URL and cannot be parsed, a NULL pointer will be returned 
    //          instead.
    static URL* parse(const std::string& url_string);


    // Purpose: Looks up the protocol given in the URL.
    // Recevie: None
    // Return:  The URL's protocol.
    const std::string& get_protocol() const;

    // Purpose: Looks up the target host of the URL.
    // Receive: None
    // Return:  The URL's host.
    const std::string& get_host() const;

    // Purpose: Checks if the URL refers to a specific port, or none at all.
    // Receive: None
    // Return:  true if the URL has a defined port, false if not.
    bool is_port_defined() const;

    // Purpose: Looks up the port number to which the URL refers.
    // Receive: None
    // Return:  The URL's port.  If is_port_defined() returns false, this
    //          value is meaningless.
    unsigned get_port() const;

    // Purpose: Looks up the path of the resource to which the URL refers.
    // Receive: None
    // Return:  The URL's path.
    const std::string& get_path() const;

    // Purpose: Looks up the query part of the URL (which may be used to identify
    //          a resource in a non-hierarchical manner, unlike the path).
    // Receive: None
    // Return:  The URL's query.
    const std::string& get_query() const;

    // Purpose: Looks up the fragment of the primary resource to which the URL
    //          specifically refers (e.g. an anchor in a web page).
    // Receive: None
    // Return:  The URL's fragment.
    const std::string& get_fragment() const;


    // Purpose: Has the URL printed to the given output stream, in standard format.
    // Receive: out - The output stream to which to print the URL.
    // Return:  None
    void Print(std::ostream& out);

    // Purpose: Has the URL printed into the given string, in standard format.
    // Receive: target - Will be set to a string representation of this URL.
    // Return:  None
    void Print(std::string& target);


    // Purpose: Sets the URL protocol to the given string.
    // Receive: protocol - The protocol to set.
    // Return:  None
    void set_protocol(const std::string& protocol);

    // Purpose: Sets the URL's host to the given string.
    // Receive: host - The host to set.
    // Return:  None
    void set_host(const std::string& host);

    // Purpose: Throws out the port of the URL, making it undefined.
    // Receive: None
    // Return:  None
    void Clear_port();

    // Purpose: Sets a specific port number for the URL.
    // Receive: port - The port to set.
    // Return:  None
    void set_port(unsigned port);

    // Sets the path of the resource to which the URL refers.
    // Receive: None
    //
    // path - The path to set.
    void set_path(const std::string& path);

    // Sets the query string for the URL.
    // Receive: None
    //
    // query - The query to set.  Can be used to effectively delete the
    //   URL's query by passing in a blank string.
    void set_query(const std::string& query);

    // Sets the fragment for the URL.
    // Receive: None
    //
    // fragment - The fragment to set.  Will effectively delete the URL's
    //   fragment if you pass in a blank string.
    void set_fragment(const std::string& fragment);


private:
    size_t read_protocol(const std::string& url_string, size_t offset = 0);
    size_t read_host_port(const std::string& url_string, size_t offset);
    size_t read_path_details(const std::string& url_string, size_t offset);

    std::string protocol;
    std::string host;
    unsigned port;
    std::string path;
    std::string query;
    std::string fragment;
};

#endif //ndef _URL_H_
