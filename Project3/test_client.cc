// Modified from CSE422 FS09
// test_client - Demonstrates how a local video file can be played back with
// the Video_Player class.

#include "Video_Player.h"
#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    // See what we're playing.
    if (argc != 2) {
        cout << "Usage:  " << argv[0] << " filename" << endl;
        return 1;
    }

    // Try to open up the video file for reading.
    ifstream video_in(argv[1]);

    if (!video_in.good()) {
        cout << "Error opening " << argv[1] << endl;
        return 2;
    }

    // Make a video player instance to play the file back.
    Video_Player* player = Video_Player::Create();
    if (player == NULL) {
        cout << "Error initializing video player." << endl;
        return 3;
    }
    cout << "Player created." << endl;

    // Tell the video player to get ready for playback...
    player->Start();
    cout << "Start playback." << endl;

    // and then feed in the entire file, piece by piece.
    char data_buffer[65536];
    while (video_in.good()) {
        size_t bytes_read = video_in.readsome(data_buffer,
                                              sizeof(data_buffer));
        if (bytes_read == 0)
            break;

        player->Stream(data_buffer, bytes_read);
    }

    // The video is played back in a separate thread, so we don't know
    // how long it will be until it ends.  Wait, therefore, until the user
    // closes the playback window, and the video is guaranteed to no
    // longer be running.
    player->Wait_for_close();

    // Clean up.
    delete player;
    return 0;
}
