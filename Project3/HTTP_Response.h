// HTTP_Response - Class representing an HTTP response message.  May be used
// both to parse existing HTTP response into a comprehensible object, and to
// construct new responses from scratch and print them out to a text string.
// Makes no attempt to handle the body of the response -- only the response code
// and the headers will be captured.
//
// If you're planning on servicing GET and HEAD requests only, you can use
// the Create_standard_response() method to have a lot of headers automatically
// set up for you.  The HTTP specification mandates these headers, and some
// clients may expect them.
//
// Also see the HTTP_Message class for methods that can be used to query and
// set the response headers.

#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include "HTTP_Message.h"
#include <string>


class HTTP_Response : public HTTP_Message
{
public:
	// Constructs a new HTTP_Response.  Note that nothing is done to check
	// the validity of the arguments -- make sure you trust your input
	// and/or yourself.
	//
	// code - The code representing the response status (e.g. 200, 403).
	// reason - A one-line textual description of the response.
	// version - The HTTP version used to transmit the response.
	HTTP_Response(unsigned code = 0, const std::string& reason = "",
		const std::string& version = "HTTP/1.1");

	virtual ~HTTP_Response();


	// Constructs an HTTP_Response object corresponding to the actual
	// response text in the given buffer.  Use this if you've received a
	// response and want to know what it says.
	//
	// data - The text buffer in which the response is stored.
	// length - The length of the response data, in bytes.
	//
	// Returns - An HTTP_Response parsed from the given response text.
	//   If parsing fails, a NULL pointer will be returned instead.
	static HTTP_Response* Parse(const char* data, unsigned length);

	// Constructs a new HTTP_Response with some mandatory header fields
	// convenienty set for you (unlike the constructor, which sets no
	// fields for you at all).  Assumes that you will be sending back
	// some kind of message body, and that the message body will be sent
	// verbatim (i.e. not compressed or chunked in any way).  Also assumes
	// that the connection will be closed immediately after the send ends.
	//
	// content_length - The length of the message body that will be
	//   sent following this response.
	// code - The code representing the response status (e.g. 500).
	// reason - A short description of the response code's meaning.
	// version - The HTTP version used to transmit the response.
	//
	// Returns - An HTTP_Response created for the given input, containing
	//   all of the mandatory headers.
	static HTTP_Response* Create_standard_response(unsigned content_length,
		unsigned code = 0,
		const std::string& reason = "",
		const std::string& version = "HTTP/1.1");


	// Looks up the version of the HTTP response (e.g. HTTP/1.1).
	//
	// Returns - The response's HTTP version.
	const std::string& Get_version() const;

	// Looks up the status code of the HTTP response (e.g. 404, 500).
	//
	// Returns - The response's status code.
	unsigned Get_code() const;

	// Looks up the description of the response (e.g. "OK").
	//
	// Returns - The response's associated reason string.
	const std::string& Get_reason() const;


	// Prints the response object to a text string, suitable for sending
	// to an HTTP client.  Includes the terminating blank line and all
	// response headers.
	//
	// output_string - Will be set to the response text.
	void Print(std::string& output_string) const;

	// Prints the response object to a text string, suitable for sending
	// to an HTTP client.  Includes the terminating blank line and all
	// response headers.
	//
	// output_buffer - The text buffer into which the response should be
	//   printed.  Will be null-terminated.
	// buffer_length - The number of characters available for writing in
	//   the buffer.  Printing stops after this many characters have been
	//   written.
	void Print(char* output_buffer, unsigned buffer_length) const;


	// Sets the HTTP version of the response (e.g. HTTP/1.1).
	//
	// version -  The version to set.
	void Set_version(const std::string& version);

	// Sets the status code to indicate in the response.
	//
	// code - The HTTP status code to set.
	void Set_code(const unsigned code);

	// Sets the reason for the given status code being sent.
	//
	// reason - The text string to set as the response's description.
	void Set_reason(const std::string& reason);


private:
	std::string version;
	unsigned code;
	std::string reason;
};

#endif //ndef _HTTP_RESPONSE_H_
