#include "URI.h"
#include <cstdlib>
#include <sstream>

using namespace std;


namespace
{
	// Used to signal when the port number is not known.  There is an
	// excellent chance that this will never be a valid port for anything.
	const unsigned UNDEFINED_PORT = 0xffffffff;
}


// We want to assume that the port hasn't been set until we know otherwise.
// We also want to make sure that there's some kind of path, since HTTP
// requires a path.  The root path is the accepted default there.
URI::URI()
	: port(UNDEFINED_PORT),
	path("/")
{
}


URI::~URI()
{
	// Nothing to do...
}


URI* URI::Parse(const string& uri_string)
{
	URI* new_uri = new URI();

	size_t offset = new_uri->Read_protocol(uri_string);
	if (offset == string::npos)
	{
		delete new_uri;
		return NULL;
	}

	offset = new_uri->Read_host_port(uri_string, offset);
	if (offset < uri_string.length())
	{
		new_uri->Read_path_details(uri_string, offset);

		// If the client somehow input a URI with an empty path,
		// quietly save them from themselves.
		if (new_uri->path.length() == 0)
			new_uri->path = "/";
	}

	return new_uri;
}


const string& URI::Get_protocol() const
{
	return protocol;
}


const string& URI::Get_host() const
{
	return host;
}


bool URI::Is_port_defined() const
{
	return (port != UNDEFINED_PORT);
}


unsigned URI::Get_port() const
{
	return port;
}


const string& URI::Get_path() const
{
	return path;
}


const string& URI::Get_query() const
{
	return query;
}


const string& URI::Get_fragment() const
{
	return fragment;
}


void URI::Print(ostream& out)
{
	// Say the URI is http://www.example.org:8080/example.php?example#ex
	// Each piece follows.  Note that we should avoid printing optional
	// parts of the URI that have associated formatting characters, if
	// they aren't actually defined.

	// http://
	out << protocol << "://";

	// www.example.org
	out << host;

	// :8080 (if given)
	if (Is_port_defined())
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


void URI::Print(string& target)
{
	// Much easier than duplicating the code.
	ostringstream target_out;
	Print(target_out);
	target = target_out.str();
}


size_t URI::Read_protocol(const string& uri_string, size_t offset)
{
	size_t protocol_end = uri_string.find("://", offset);
	if (protocol_end == string::npos)
		return string::npos;

	protocol = uri_string.substr(offset, protocol_end - offset);

	return protocol_end + 3;
}


size_t URI::Read_host_port(const string& uri_string, size_t offset)
{
	size_t part_end = uri_string.find_first_of("/#?", offset);
	if (part_end == string::npos)
		part_end = uri_string.length();

	size_t port_offset = uri_string.find(":", offset);
	if ((port_offset == string::npos) || (port_offset > part_end))
		port_offset = part_end;

	host = uri_string.substr(offset, port_offset - offset);
	if (port_offset < part_end)
	{
		port = atoi(uri_string.substr(port_offset + 1,
			part_end - port_offset - 1).c_str());
	}

	return part_end;
}


size_t URI::Read_path_details(const string& uri_string, size_t offset)
{
	size_t unparsed_end = uri_string.length();

	// Once you hit the beginning of the fragment, that's the end of the
	// URI.  Since it's nice to know where our limits are, let's check for
	// that first.
	size_t fragment_offset = uri_string.find("#", offset);
	if (fragment_offset != string::npos)
	{
		fragment = uri_string.substr(fragment_offset + 1);
		unparsed_end = fragment_offset;
	}

	size_t query_offset = uri_string.find("?", offset);
	if ((query_offset != string::npos) && (query_offset < unparsed_end))
	{
		query = uri_string.substr(query_offset + 1,
			unparsed_end - query_offset - 1);
		unparsed_end = query_offset;
	}

	path = uri_string.substr(offset, unparsed_end - offset);

	return uri_string.length();
}


void URI::Set_protocol(const string& protocol)
{
	this->protocol = protocol;
}

void URI::Set_host(const string& host)
{
	this->host = host;
}

void URI::Clear_port()
{
	port = UNDEFINED_PORT;
}

void URI::Set_port(unsigned port)
{
	this->port = port;
}

void URI::Set_query(const string& query)
{
	this->query = query;
}

void URI::Set_fragment(const string& fragment)
{
	this->fragment = fragment;
}
