#include "Playlist_Entry.h"

using namespace std;


Playlist_Entry::Playlist_Entry(const string& uri, unsigned duration)
	: uri(uri),
	duration(duration)
{
}


Playlist_Entry::~Playlist_Entry()
{
}


const string& Playlist_Entry::Get_uri() const
{
	return uri;
}

unsigned Playlist_Entry::Get_duration() const
{
	return duration;
}


void Playlist_Entry::Set_uri(const string& uri)
{
	this->uri = uri;
}

void Playlist_Entry::Set_duration(unsigned duration)
{
	this->duration = duration;
}
