#include "HTTP_Message.h"
#include <algorithm>
#include <cstring>

using namespace std;


HTTP_Message::HTTP_Message()
{
	// Nothing to do...
}


HTTP_Message::~HTTP_Message()
{
	// Nothing to do here, either...
}


unsigned HTTP_Message::Get_num_header_fields() const
{
	return headers.size();
}


void HTTP_Message::Get_header_set(vector<HTTP_Header_Field>& out_set) const
{
	out_set.clear();
	for (map<string, string>::const_iterator it = headers.begin();
		it != headers.end(); it++)
	{
		HTTP_Header_Field header = {it->first, it->second};
		out_set.push_back(header);
	}
}


bool HTTP_Message::Get_header_value(const string& name, string& out_value)
	const
{
	map<string, string>::const_iterator it = headers.find(name);
	if (it != headers.end())
	{
		out_value = it->second;
		return true;
	}
	else
	{
		return false;
	}
}


void HTTP_Message::Set_header_field(const HTTP_Header_Field& field)
{
	Set_header_field(field.name, field.value);
}


void HTTP_Message::Set_header_field(const string& name, const string& value)
{
	headers[name] = value;
}


bool HTTP_Message::Parse_fields(const char* data, unsigned length)
{
	// Keep parsing fields until we run up against the end of the data or
	// we reach the end-of-lines marking the end of the headers.
	const char* data_end = data + length;
	bool found_eoh = false;
	while (data < data_end)
	{
		// Figure out where this header line ends.  Check if it's a
		// blank line (signifying the end of the headers), and make
		// sure it has an ending at all (if it doesn't, we haven't read
		// the complete header yet).
		const char* line_end = Find_next_line(data, length);
		if (line_end == (data + line_ending.length()))
		{
			found_eoh = true;
			break;
		}
		else if (line_end == NULL)
		{
			break;
		}
		// We won't be working with the EOL characters, so skip 'em.
		line_end -= line_ending.length();

		// Figure out where the break between the header name and
		// value appears.
		const char* delim_pos = data;
		for (; (delim_pos < line_end) &&
			(*delim_pos != header_delimiter); delim_pos++)
		{
		}
		// If it doesn't, we've got a bad header.
		if (delim_pos >= line_end)
			break;

		// Grab out the name & value.  Trim any crud off the value
		// that we can.
		string name, value;
		name = string(data, static_cast<size_t>(delim_pos - data));
		value = string(delim_pos + 1,
			static_cast<size_t>(line_end - delim_pos - 1));

		size_t ltrim_pos = value.find_first_not_of(" \t\r\n");
		size_t rtrim_pos = value.find_last_not_of(" \t\r\n");
		if (ltrim_pos != string::npos)
		{
			value = string(value, ltrim_pos, rtrim_pos);
		}
		else
		{
			value = "";
		}

		Set_header_field(name, value);

		// Jump to the next line, for the next header.
		data = line_end + line_ending.length();
	}

	return found_eoh;
}


void HTTP_Message::Print(string& output_string) const
{
	// Append the contents of our headers one-by-one.
	for (map<string, string>::const_iterator it = headers.begin();
		it != headers.end(); it++)
	{
		output_string += it->first;
		output_string += header_delimiter;
		output_string += " ";
		output_string += it->second;
		output_string += line_ending;
	}

	// Toss in a final line ending to signify the headers' end.
	output_string += line_ending;
}


void HTTP_Message::Print(char* output_buffer, unsigned buffer_length) const
{
	const char delim_string[] = {header_delimiter, ' ', '\0'};

	for (map<string, string>::const_iterator it = headers.begin();
		it != headers.end(); it++)
	{
		Copy_if_room(output_buffer, it->first.c_str(), buffer_length);
		Copy_if_room(output_buffer, delim_string, buffer_length);
		Copy_if_room(output_buffer, it->second.c_str(),
			buffer_length);
		Copy_if_room(output_buffer, line_ending.c_str(), buffer_length);
	}

	Copy_if_room(output_buffer, line_ending.c_str(), buffer_length);
}


void HTTP_Message::Copy_if_room(char*& output_buffer, const char* data_string,
	unsigned& remaining_length) const
{
	// Quit now if there's nothing at all that we can do.
	if (remaining_length == 0)
		return;

	// Figure out how much data we've got to copy, given the remaining
	// space.
	unsigned data_length = strlen(data_string);
	if (data_length > remaining_length)
		data_length = remaining_length;

	// Copy exactly that much.  Advance the buffer pointer accordingly.
	memcpy(output_buffer, data_string, data_length);
	remaining_length -= data_length;
	output_buffer += data_length;

	// Be nice and null-terminate what we've written so far.
	*output_buffer = '\0';
}


const char* HTTP_Message::Find_next_line(const char* data, unsigned length)
	const
{
	// Go character-by-character through the data until we either get past
	// the end or we get past a line-ending string.  Note that in the latter
	// case, we intentionally move a character past the line ending, so
	// the returned pointer will point to the *next* line.
	const char* data_end = data + length;
	unsigned end_chars_found = 0;
	while ((data < data_end) && (end_chars_found < line_ending.length()))
	{
		if (*data == line_ending[end_chars_found])
			end_chars_found++;
		else
			end_chars_found = 0;

		data++;
	}

	// If we found the line end, great.  If not, boo.
	if (end_chars_found >= line_ending.length())
		return data;
	else
		return NULL;
}
