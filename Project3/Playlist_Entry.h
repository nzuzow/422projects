// Playlist_Entry - Class representing the entry for a specific segment in an
// extended M3U playlist.  It should be considered internal to the Playlist
// class's implementation.

#ifndef _PLAYLIST_ENTRY_H_
#define _PLAYLIST_ENTRY_H_

#include <string>


class Playlist_Entry
{
public:
	Playlist_Entry(const std::string& uri = "", unsigned duration = 0);
	~Playlist_Entry();

	const std::string& Get_uri() const;
	unsigned Get_duration() const;

	void Set_uri(const std::string& uri);
	void Set_duration(unsigned duration);

private:
	std::string uri;
	unsigned duration;
};

#endif //ndef _PLAYLIST_ENTRY_H_
