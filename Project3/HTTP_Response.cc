#include "HTTP_Response.h"
#include <cstdlib>
#include <ctime>
#include <sstream>

using namespace std;


HTTP_Response::HTTP_Response(unsigned code, const string& reason,
	const string& version)
	: version(version),
	code(code),
	reason(reason)
{
}


HTTP_Response::~HTTP_Response()
{
	// Nothing to do...
}


HTTP_Response* HTTP_Response::Parse(const char* data, unsigned length)
{
        // Separate the opening line (for the response) from the rest.
        HTTP_Response* response = new HTTP_Response();
        const char* first_header = response->Find_next_line(data, length);
        if (first_header == NULL)
        {
                // Ouch, not even a complete first line...
                delete response;
                return NULL;
        }
        size_t first_line_length = static_cast<size_t>(first_header - data);
	string response_line(data, first_line_length - 2);

	// Parse the pieces of the response.
	size_t code_pos = response_line.find(" ");
	size_t reason_pos = string::npos;
	if (code_pos != string::npos)
	{
		response->Set_version(response_line.substr(0, code_pos));
		reason_pos = response_line.find(" ", code_pos + 1);
	}
	if (reason_pos != string::npos)
	{
		// Note that codes fall within a pretty strict range (which
		// happens to exclude atoi's default 0 return value).
		int code = atoi(response_line.substr(code_pos + 1,
			reason_pos - code_pos - 1).c_str());
		if ((code < 100) || (code >= 600))
		{
			delete response;
			return NULL;
		}

		response->Set_code(static_cast<unsigned>(code));
		response->Set_reason(response_line.substr(reason_pos + 1));
	}
	else
	{
		// Missing fields = bad response.
		delete response;
		return NULL;
	}

	// Have the header lines parsed now; response line is okay.
	bool headers_okay = response->Parse_fields(first_header,
		length - first_line_length);
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


HTTP_Response* HTTP_Response::Create_standard_response(unsigned content_length,
	unsigned code, const string& reason, const string& version)
{
	HTTP_Response* response = new HTTP_Response(code, reason, version);

	// Assume we're not bothering with chunked/gzipped data.
	response->Set_header_field("Content-Encoding", "identity");
	response->Set_header_field("Transfer-Encoding", "identity");

	// Also assume that we don't want to have to keep track of connections.
	response->Set_header_field("Connection", "close");

	// HTTP requires responses to include the data of construction.
	// Therefore, let's set that.
	char time_buffer[128];
	time_t response_time = time(NULL);
	strftime(time_buffer, sizeof(time_buffer) / sizeof(char),
		"%a, %d %b %Y %H:%M:%S %Z", gmtime(&response_time));
	response->Set_header_field("Date", time_buffer);

	// Finally, we know how long the body's going to be, so set that, too.
	ostringstream length_string;
	length_string << content_length;
	response->Set_header_field("Content-Length", length_string.str());

	return response;
}


const string& HTTP_Response::Get_version() const
{
	return version;
}


unsigned HTTP_Response::Get_code() const
{
	return code;
}


const string& HTTP_Response::Get_reason() const
{
	return reason;
}


void HTTP_Response::Print(string& output_string) const
{
	// Have the sstream library format the response line for us, since we
	// need to turn the status code back into a string somehow.
	ostringstream response_line;
	response_line << version << " " << code << " " << reason;

	// Take that and toss on the ending to get the first line...
	output_string = response_line.str();
	output_string += line_ending;
	
	// ...and then add the associated headers.
	HTTP_Message::Print(output_string);
}


void HTTP_Response::Print(char* output_buffer, unsigned buffer_length) const
{
	// Similar business, though we have to be more choosy with how we
	// apply sstream.
	ostringstream code_str;
	code_str << code;

	Copy_if_room(output_buffer, version.c_str(), buffer_length);
	Copy_if_room(output_buffer, " ", buffer_length);
	Copy_if_room(output_buffer, code_str.str().c_str(), buffer_length);
	Copy_if_room(output_buffer, " ", buffer_length);
	Copy_if_room(output_buffer, reason.c_str(), buffer_length);

	Copy_if_room(output_buffer, line_ending.c_str(), buffer_length);

	HTTP_Message::Print(output_buffer, buffer_length);
}


void HTTP_Response::Set_version(const string& version)
{
	this->version = version;
}


void HTTP_Response::Set_code(const unsigned code)
{
	this->code = code;
}


void HTTP_Response::Set_reason(const string& reason)
{
	this->reason = reason;
}
