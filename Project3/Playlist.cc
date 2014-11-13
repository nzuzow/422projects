#include "Playlist.h"
#include <cstdlib>
#include <sstream>

using namespace std;


namespace
{
	const string GARBAGE_URI = "";

	const string PLAYLIST_HEADER = "#EXTM3U";
	const string SEGMENT_TAG = "#EXTINF:";
	const string END_TAG = "#EXT-X-ENDLIST";
}


Playlist::Playlist()
{
}

Playlist::~Playlist()
{
}


Playlist* Playlist::Parse(const char* data, unsigned length)
{
	// Make sure there's a proper header in the given data.  If not, don't
	// even try.
	if (!Verify_header(data, length))
		return NULL;

	// Read in playlist information until we can't get any more.  Pool it
	// all together.
	Playlist* playlist = new Playlist;
	bool possibly_more = true;
	while (possibly_more)
	{
		possibly_more = Read_next_segment(data, length, playlist);
	}

	return playlist;
}

Playlist* Playlist::Parse(const string& data)
{
	return Parse(data.c_str(), data.length());
}


unsigned Playlist::Get_num_segments() const
{
	return segments.size();
}

unsigned Playlist::Get_segment_duration(unsigned segment) const
{
	if (segment < Get_num_segments())
		return segments[segment].Get_duration();
	else
		return 0;
}

const string& Playlist::Get_segment_uri(unsigned segment) const
{
	if (segment < Get_num_segments())
		return segments[segment].Get_uri();
	else
		return GARBAGE_URI; 
}


bool Playlist::Verify_header(const char*& data, unsigned& length)
{
	string header_line;
	Read_up_to(data, length, '\n', header_line);

	return ((header_line == PLAYLIST_HEADER) && (length != 0));
}


bool Playlist::Read_next_segment(const char*& data, unsigned& length,
	Playlist* out_playlist)
{
	// Read lines out of the buffer repeatedly.  Keep going until we hit
	// the end of the playlist or we finally read a segment.
	string line;
	bool end_of_list = false;
	bool found_segment = false;
	bool expecting_uri = false;
	while (!(end_of_list || found_segment) && (length > 0))
	{
		// Pull in the next line.
		Read_up_to(data, length, '\n', line);

		// If the line has the end-of-playlist tag on it, don't bother
		// reading anything else.
		if (line.substr(0, END_TAG.length()) == END_TAG)
		{
			end_of_list = true;
		}

		// If the line starts with the segment indicator tag, read it,
		// the duration that follows it, and the URI on the next line.
		// If it looks like something fishy's happening, bail.
		if (line.substr(0, SEGMENT_TAG.length()) == SEGMENT_TAG)
		{
			// The duration should be between the tag and a comma.
			// Make sure the comma's there.
			line.erase(0, SEGMENT_TAG.length());
			size_t comma_pos = line.find(',');
			if (comma_pos != string::npos)
			{
				// Grab the duration & URI.  Add them as the
				// next entry in the playlist.  Make sure the
				// URI's kosher first, though.
				unsigned duration = atoi(line.c_str());

				Read_up_to(data, length, '\n', line);
				if ((line.length() > 0) && (line[0] != '#'))
				{
					Playlist_Entry entry(line, duration);
					out_playlist->segments.push_back(entry);
					found_segment = true;
				}
				else if (line.substr(0, END_TAG.length()) ==
					END_TAG)
				{
					// Toss in this check why not.
					end_of_list = true;
				}
			}
		}

		// Since we don't support any other tags...  don't do anything
		// if the line starts with anything else.
	}

	// Double-check that the loop didn't exit because we hit the end of
	// the file.
	return !end_of_list && (length > 0);
}


void Playlist::Read_up_to(const char*& data, unsigned& length,
	char delimiter, string& output)
{
	// Throw out whatever was in the string before (we'll replace it).
	output.clear();

	// If we're already done... hey, we're done!
	if (length == 0)
		return;

	// Read up until we reach the delimiter, or we hit the end of the data,
	// whichever comes first.  Add all characters other than the delimiter
	// to the output string.  Be sure to read at least one character every
	// time so that it's possible to advance beyond a sequence of several
	// delimiters in a row.
	const char* data_end = data + length;
	char test;
	do
	{
		test = *data;
		data++;
		length--;

		if (test != delimiter)
			output += test;
	} while ((test != delimiter) && (data < data_end));
}
