// Playlist - Represents an extended M3U playlist, as described in Apple's HTTP
// Live Streaming specification.  By design, this leaves out many of the
// features of the full spec, and instead provides a convenient way to parse
// such playlist files and see which media files they reference.
//
// Conceptually, a playlist is formed from a sequence of segments.  To play
// back the playlist, each segment should be downloaded and streamed in order,
// starting with segment 0.

#ifndef _PLAYLIST_H_
#define _PLAYLIST_H_

#include "Playlist_Entry.h"
#include <string>
#include <vector>


class Playlist
{
public:
	~Playlist();

	// Parses the extended M3U playlist file stored in the given data
	// buffer.
	//
	// data - The buffer in which the playlist file is stored.
	// length - The length of the given buffer.
	//
	// Returns - A new Playlist that represents the buffer's contents.
	// Will return NULL if the buffered playlist cannot be parsed.
	static Playlist* Parse(const char* data, unsigned length);

	// Parses the extended M3U playlist file stored in the given string.
	//
	// data - The string in which the playlist file is stored.
	//
	// Returns - A new Playlist that represents the string's contents.
	// Will return NULL if the buffered playlist cannot be parsed.
	static Playlist* Parse(const std::string& data);


	// Gets the length of the current playlist.
	//
	// Returns - The number of segment files in the playlist.
	unsigned Get_num_segments() const;

	// Gets the length of the segment at the given index in the playlist.
	//
	// Returns - The approximate length of the given segment, in seconds.
	// The exact length of the segment may differ slightly.
	unsigned Get_segment_duration(unsigned segment) const;

	// Gets the URI of the segment at the given index in the playlist.
	//
	// Returns - The URI of the media file for the requested segment.
	const std::string& Get_segment_uri(unsigned segment) const;


protected:
	// Internal stuff.
	Playlist();

private:
	std::vector<Playlist_Entry> segments;

	static bool Verify_header(const char*& data, unsigned& length);
	static bool Read_next_segment(const char*& data, unsigned& length,
		Playlist* out_playlist);

	static void Read_up_to(const char*& data, unsigned& length,
		char delimiter, std::string& output);
};

#endif //ndef _PLAYLIST_H_
