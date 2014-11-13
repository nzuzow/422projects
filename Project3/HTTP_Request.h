// HTTP_Request - Class representing an HTTP request message.  May be used both
// to parse an existing HTTP request into a comprehensible object, and to
// construct new requests from scratch and print them out to a text string.
// Makes no attempt to handle the body of the request -- only the request line
// and the headers will be captured.
//
// If all you want to do is download a file, call Create_GET_request() with
// the path of the file that you want to download, and then call Set_host()
// on the returned object with the hostname of the server from which you'll
// be downloading.  You should then be able to Print() the request out to a
// character buffer to get something that the server will accept.
//
// Also see the HTTP_Message class for methods that can be used to query and
// set the request's headers.

#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include "HTTP_Message.h"
#include <string>


class HTTP_Request : public HTTP_Message
{
public:
	// Constructs a new HTTP_Request.  Note that nothing is done to check
	// the validity of the arguments -- make sure you trust your input.
	//
	// method - The action being requested (e.g. GET, POST, etc).
	// uri - The URI of the resource to which the request applies.
	//   In most cases, this will typically just be the path of the
	//   resource on the server (e.g. /somedir/something.txt).
	// version - The HTTP version of the client making the request.
	//   Will default to HTTP 1.1 (which ought to be what you support).
	HTTP_Request(const std::string& method = "",
		const std::string& uri = "",
		const std::string& version = "HTTP/1.1");

	virtual ~HTTP_Request();


	// Constructs an HTTP_Request object corresponding to the actual
	// request text in the given buffer.  Use this if you've received a
	// request and want to know what it's asking.
	//
	// data - The text buffer in which the request is stored.
	// length - The length of the request data, in bytes.
	//
	// Returns - An HTTP_Request parsed from the request text.  If parsing
	//   fails, a NULL pointer will be returned instead.
	static HTTP_Request* Parse(const char* data, unsigned length);

	// Constructs a new HTTP GET request, with a header or two set to
	// make it more likely that the server will return an easy-to-handle
	// result.
	//
	// uri - The URI of the resource to get.
	// version - The HTTP version to associate with the request.
	//
	// Returns - A new HTTP_Request object for the GET request.
	static HTTP_Request* Create_GET_request(const std::string& uri ="",
		const std::string& version = "HTTP/1.1");


	// Looks up the method of the request (e.g. GET, PUT, DELETE).
	//
	// Returns - The request method.
	const std::string& Get_method() const;

	// Looks up the URI targeted by the request (e.g. /stuff.txt).
	//
	// Returns - The request's URI.
	const std::string& Get_uri() const;

	// Looks up the HTTP version of the requesting client (e.g. HTTP/1.1).
	//
	// Returns - The request's HTTP version.
	const std::string& Get_version() const;

	// Looks up the host for which the request is intended, from the
	// request's Host header.
	//
	// out_host - Will be set to the request's target host.  If the host
	//   has not yet been entered, it will be set to a blank string.
	void Get_host(std::string& out_host) const;


	// Prints the request object to a text string, suitable for transmission
	// to an HTTP server.  Includes the terminating blank line and all
	// request headers.
	//
	// output_string - Will be set to the request text.
	void Print(std::string& output_string) const;

	// Prints the request object to a text string, suitable for transmission
	// to an HTTP server.  Includes the terminating blank line and all
	// request headers.
	//
	// output_buffer - The text buffer into which the request should be
	//   printed.  Will be null-terminated.
	// buffer_length - The number of characters available for writing in
	//   the buffer.  Printing stops after this many characters have been
	//   written.
	void Print(char* output_buffer, unsigned buffer_length) const;


	// Sets the method of the HTTP request (e.g. GET, PUT, DELETE).
	//
	// method - The method to set for the request.
	void Set_method(const std::string& method);

	// Sets the URI that the request should target (e.g. /stuff.txt).
	//
	// uri - The URI to set for the request.
	void Set_uri(const std::string& uri);

	// Sets the HTTP version supported by the request's client.
	//
	// version - The HTTP version the request should indicate.
	void Set_version(const std::string& version);

	// Sets the host for which the request is intended, into the request's
	// Host header.
	//
	// host - The host to set for the request.
	void Set_host(const std::string& host);


private:
	std::string method;
	std::string uri;
	std::string version;
};

#endif //ndef _HTTP_REQUEST_H_
