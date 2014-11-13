// HTTP_Message - Base class for HTTP requests and responses.  Defines the
// methods for accessing the various headers on a request/response.  Also
// defines some internal things that are shared by the request/response classes.

#ifndef _HTTP_MESSAGE_H_
#define _HTTP_MESSAGE_H_

#include <map>
#include <string>
#include <vector>

namespace
{
	// The exact string of characters used to represent HTTP line endings.
	const std::string line_ending = "\r\n";

	// The character used to separate the name of a header from its value.
	const char header_delimiter = ':';
}


// Structure used to associate header names and values, whenever necessary.
struct HTTP_Header_Field
{
	std::string name;
	std::string value;
};


class HTTP_Message
{
public:
	virtual ~HTTP_Message();

	// Indicates how many headers the message has.
	//
	// Returns - The number of headers stored in the message.
	unsigned Get_num_header_fields() const;

	// Copies all of the message's headers into the given vector.  Use
	// this if you need to iterate through the headers.  If you know the
	// name of the header you want, Get_header_value() is far more useful.
	//
	// out_set - Will be set to a collection of HTTP_Header_Fields
	//   representing all of the headers stored in the message
	void Get_header_set(std::vector<HTTP_Header_Field>& out_set) const;

	// Retrieves the value of the header with the given name.
	//
	// name - The name of the header to look up.
	// out_value - Will be set to that header's value, if it is found.
	//   If no header with that name is found, value will be undefined.
	//
	// Returns - true if the requested header was found in the message
	//   (in which case, out_value is valid); false if it was not found.
	bool Get_header_value(const std::string& name,
		std::string& out_value) const;


	// Updates the message to have the given header field.  Overwrites the
	// old value of the specified header if the message already had it.
	//
	// field - The name/value of the header to set.
	void Set_header_field(const HTTP_Header_Field& field);

	// Updates the given header field in the message.  If the header is not
	// already present, it will be added to the message.  If the header
	// *is* already present, its previous value will be overwritten.
	//
	// name - The name of the header to set.
	// value - The new value to set.
	void Set_header_field(const std::string& name,
		const std::string& value);


protected:
	HTTP_Message();

	bool Parse_fields(const char* data, unsigned length);
	virtual void Print(std::string& output_string) const;
	virtual void Print(char* output_buffer, unsigned buffer_length) const;

	virtual void Copy_if_room(char*& output_buffer, const char* data_string,
		unsigned& remaining_length) const;

	const char* Find_next_line(const char* data, unsigned length) const;

private:
	std::map<std::string, std::string> headers;
};

#endif //ndef _HTTP_MESSAGE_H_
