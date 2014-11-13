#include "HTTP_Request.h"

using namespace std;


HTTP_Request::HTTP_Request(const string& method, const string& uri,
	const string& version)
	: method(method),
	uri(uri),
	version(version)
{
}


HTTP_Request::~HTTP_Request()
{
	// Nothing to do...
}


HTTP_Request* HTTP_Request::Parse(const char* data, unsigned length)
{
	// Separate the opening line (for the request) from the rest.
	HTTP_Request* request = new HTTP_Request();
	const char* first_header = request->Find_next_line(data, length);
	if (first_header == NULL)
	{
		// Ouch, not even a complete first line...
		delete request;
		return NULL;
	}
	size_t first_line_length = static_cast<size_t>(first_header - data);

	// Figure out that opening request line.  Look for the spaces that
	// separate the method, URI, and version.  Set as appropriate.
	string request_line(data, first_line_length - 2);

	size_t uri_pos = request_line.find(" ");
	size_t version_pos = string::npos;
	if (uri_pos != string::npos)
	{
		request->Set_method(request_line.substr(0, uri_pos));
		version_pos = request_line.find(" ", uri_pos + 1);
	}
	if (version_pos != string::npos)
	{
		request->Set_uri(request_line.substr(uri_pos + 1,
			version_pos - uri_pos - 1));
		request->Set_version(request_line.substr(version_pos + 1));
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
	bool headers_okay = request->Parse_fields(first_header,
		length - first_line_length);
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


HTTP_Request* HTTP_Request::Create_GET_request(const string& uri,
	const string& version)
{
	HTTP_Request* request = new HTTP_Request("GET", uri, version);

        // Ask the server to either send the file data uncompressed, or not
        // bother sending it at all.  I don't think anyone plans on tying
        // gzip into this lab.
        request->Set_header_field("Accept-Encoding", "identity;q=1.0, *;q=0");

	// Make a futile attempt to stop servers from sending chunked
	// responses, even though the HTTP 1.1 spec says they can do it anyway.
	request->Set_header_field("TE", "identity;q=1.0, chunked;q=0, *;q=0");

	return request;
}


const string& HTTP_Request::Get_method() const
{
	return method;
}


const string& HTTP_Request::Get_uri() const
{
	return uri;
}


const string& HTTP_Request::Get_version() const
{
	return version;
}


void HTTP_Request::Get_host(string& out_host) const
{
	if (!Get_header_value("Host", out_host))
	{
		out_host = "";
	}
}


void HTTP_Request::Print(string& output_string) const
{
	// Throw in our one request line.
	output_string = method;
	output_string += ' ';
	output_string += uri;
	output_string += ' ';
	output_string += version;

	output_string += line_ending;

	// Now have all the headers thrown in on top of that.
	HTTP_Message::Print(output_string);
}


void HTTP_Request::Print(char* output_buffer, unsigned buffer_length) const
{
	// Similar model, except with a character buffer.
	Copy_if_room(output_buffer, method.c_str(), buffer_length);
	Copy_if_room(output_buffer, " ", buffer_length);
	Copy_if_room(output_buffer, uri.c_str(), buffer_length);
	Copy_if_room(output_buffer, " ", buffer_length);
	Copy_if_room(output_buffer, version.c_str(), buffer_length);

	Copy_if_room(output_buffer, line_ending.c_str(), buffer_length);

	HTTP_Message::Print(output_buffer, buffer_length);
}


void HTTP_Request::Set_method(const string& method)
{
	this->method = method;
}


void HTTP_Request::Set_uri(const string& uri)
{
	this->uri = uri;
}


void HTTP_Request::Set_version(const string& version)
{
	this->version = version;
}


void HTTP_Request::Set_host(const string& host)
{
	Set_header_field("Host", host);
}
