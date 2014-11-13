// URI - Represents most aspects of a uniform resource identifier (URI).  Can
// be used to parse existing URI strings into their component parts, and to form
// new URI strings piece-by-piece from those components.
//
// Expects URIs to be formatted in the following manner (note that most fields
// are optional, depending on which other fields are also present):
//
// protocol://host:port/path?query#fragment

#ifndef _URI_H_
#define _URI_H_

#include <iostream>
#include <string>


class URI
{
public:
	URI();
	~URI();

	// Creates a new URI based on the contents of the given string.
	// A few things to note about how the parsing is done:
	//  + The protocol *must* be specified.  "http://example.org" will
	//      parse.  "example.org" will not.
	//  + If no port number is given in the URI, the returned URI object
	//      will have the port clearly indicates as being undefined.
	//  + If no path is given in the URI, it will be set to a forward slash
	//      ("/"), to avoid having a blank string there.
	//
	// uri_string - The URI string to parse.
	//
	// Returns - A URI object with its components taken from the given
	//   string.  If the given string is not formatted like a proper URI
	//   and cannot be parsed, a NULL pointer will be returned instead.
	static URI* Parse(const std::string& uri_string);


	// Looks up the protocol given in the URI.
	//
	// Returns - The URI's protocol.
	const std::string& Get_protocol() const;

	// Looks up the target host of the URI.
	//
	// Returns - The URI's host.
	const std::string& Get_host() const;

	// Checks if the URI refers to a specific port, or none at all.
	//
	// Returns - true if the URI has a defined port, false if not.
	bool Is_port_defined() const;

	// Looks up the port number to which the URI refers.
	//
	// Returns - The URI's port.  If Is_port_defined() returns false, this
	//   value is meaningless.
	unsigned Get_port() const;

	// Looks up the path of the resource to which the URI refers.
	//
	// Returns - The URI's path.
	const std::string& Get_path() const;

	// Looks up the query part of the URI (which may be used to identify
	// a resource in a non-hierarchical manner, unlike the path).
	//
	// Returns - The URI's query.
	const std::string& Get_query() const;

	// Looks up the fragment of the primary resource to which the URI
	// specifically refers (e.g. an anchor in a web page).
	//
	// Returns - The URI's fragment.
	const std::string& Get_fragment() const;


	// Has the URI printed to the given output stream, in standard format.
	//
	// out - The output stream to which to print the URI.
	void Print(std::ostream& out);

	// Has the URI printed into the given string, in standard format.
	//
	// target - Will be set to a string representation of this URI.
	void Print(std::string& target);


	// Sets the URI protocol to the given string.
	//
	// protocol - The protocol to set.
	void Set_protocol(const std::string& protocol);

	// Sets the URI's host to the given string.
	//
	// host - The host to set.
	void Set_host(const std::string& host);

	// Throws out the port of the URI, making it undefined.
	void Clear_port();

	// Sets a specific port number for the URI.
	//
	// port - The port to set.
	void Set_port(unsigned port);

	// Sets the path of the resource to which the URI refers.
	//
	// path - The path to set.
	void Set_path(const std::string& path);

	// Sets the query string for the URI.
	//
	// query - The query to set.  Can be used to effectively delete the
	//   URI's query by passing in a blank string.
	void Set_query(const std::string& query);

	// Sets the fragment for the URI.
	//
	// fragment - The fragment to set.  Will effectively delete the URI's
	//   fragment if you pass in a blank string.
	void Set_fragment(const std::string& fragment);


private:
	size_t Read_protocol(const std::string& uri_string, size_t offset = 0);
	size_t Read_host_port(const std::string& uri_string, size_t offset);
	size_t Read_path_details(const std::string& uri_string, size_t offset);


	std::string protocol;
	std::string host;
	unsigned port;
	std::string path;
	std::string query;
	std::string fragment;
};

#endif //ndef _URI_H_
