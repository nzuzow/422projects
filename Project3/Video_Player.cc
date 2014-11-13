#include "Video_Player.h"
#include <cstring>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <unistd.h>

bool Video_Player::initialized = false;

namespace
{

enum PIPE_HALF {PIPE_OUT = 0, PIPE_IN = 1};

void handle_new_decoder_pad(GstElement* decoder, GstPad* new_pad,
	gboolean ignored, gpointer video_hook_ptr)
{
	GstElement* video_hook = (GstElement*)video_hook_ptr;

	// Examine the new output pad created by the decoder, and make sure it's
	// video-related.
	GstCaps* new_pad_caps = gst_pad_get_caps(new_pad);
	GstStructure* caps_struct = gst_caps_get_structure(new_pad_caps, 0);
	gboolean is_video = g_str_has_prefix(
		gst_structure_get_name(caps_struct), "video");
	gst_caps_unref(new_pad_caps);

	if (!is_video)
		return;

	// Try actually connect the new decoder pad.
	GstPad* sink_pad = gst_element_get_static_pad(video_hook, "sink");
	if (sink_pad == NULL)
	{
		g_printerr("Video sink is bad; could not set up output.\n");
		return;
	}

	GstPadLinkReturn result = gst_pad_link(new_pad, sink_pad);
	if (result != 0)
		g_printerr("Error linking video output (code %d).\n", result);
}

}


Video_Player::Video_Player()
	: pipeline(NULL),
	bus(NULL)
{
	memset(pipe_halves, 0, sizeof(pipe_halves));
	memset(queues, 0, sizeof(queues));
}


Video_Player::~Video_Player()
{
	if (bus != NULL)
		gst_object_unref(bus);

	if (pipeline != NULL)
	{
		// Make sure anything playback-related is stopped before we
		// start throwing out data.
		gst_element_set_state(pipeline, GST_STATE_NULL);
		gst_object_unref(pipeline);
	}

	if (pipe_halves[PIPE_OUT] != 0)
		close(pipe_halves[PIPE_OUT]);
	if (pipe_halves[PIPE_IN] != 0)
		close(pipe_halves[PIPE_IN]);
}


void Video_Player::Initialize()
{
	if (!initialized)
	{
		// Nobody's going to want to pass in Gstreamer options.
		// Pretend we didn't get any.
		static const char* EXE_NAME_SRC = "Video Player";
		int fake_strlen = strlen(EXE_NAME_SRC) + 1;

		char* fake_exe_name = new char[fake_strlen];
		strncpy(fake_exe_name, EXE_NAME_SRC, fake_strlen);
		char** fake_argv = new char*[2];
		fake_argv[0] = fake_exe_name;
		fake_argv[1] = NULL;

		int fake_argc = 1;

		gst_init(&fake_argc, &fake_argv);
		initialized = true;

		delete [] fake_exe_name;
		delete [] fake_argv;
	}
}


Video_Player* Video_Player::Create()
{
	// Make sure we're initialized before we try to do anything.
	Initialize();

	// Make a new video player.  Set up all of its stuff.  Make sure it
	// works.
	Video_Player* player = new Video_Player;

	if (!player->Create_pipe())
	{
		delete player;
		return NULL;
	}

	if (!player->Create_pipeline())
	{
		delete player;
		return NULL;
	}

	return player;
}


void Video_Player::Start()
{
	gst_element_set_state(pipeline, GST_STATE_PLAYING);
}

void Video_Player::Stop()
{
	gst_element_set_state(pipeline, GST_STATE_READY);
}


void Video_Player::Stream(const char* data, size_t length)
{
	write(pipe_halves[PIPE_IN], data, length);
}

bool Video_Player::Check_status()
{
	bool still_looking = true;
	bool okay = true;
	while (still_looking && okay)
	{
		GstMessage* msg = gst_bus_pop(bus);
		if (msg != NULL)
		{
			if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR)
				okay = false;
			gst_message_unref(msg);
		}
		else
		{
			still_looking = false;
		}
	}

	return okay;
}

void Video_Player::Wait_for_close()
{
	while (Check_status())
	{
		usleep(100000);
	}
}


bool Video_Player::Create_pipeline()
{
	// Create elements for each part of the pipeline.  The goal is this:
	//
	// input source -> buffer -> decoder
	// decoder >> buffer -> video format cleanup -> video output
	//
	// Note that the >> links above will need to be set up later, once the
	// decoder has determined that video content is present.
	pipeline = gst_pipeline_new("pipeline");
	GstElement* source = gst_element_factory_make("fdsrc", "source");
	GstElement* main_queue = gst_element_factory_make("queue",
		"main_queue");
	GstElement* decoder = gst_element_factory_make("decodebin", "decoder");
	GstElement* video_queue = gst_element_factory_make("queue",
		"video_queue");
	GstElement* video_color_fix = gst_element_factory_make(
		"ffmpegcolorspace", "video_color_fix");
	GstElement* video_scale_fix = gst_element_factory_make("videoscale",
		"video_scale_fix");
	GstElement* video_sink = gst_element_factory_make("ximagesink",
		"video_sink");

	// If we were unable to create any of the above elements, give up.
	// Clean up anything that won't be freed in the constructor.
	if (!(pipeline && source && main_queue && decoder && video_queue &&
		video_color_fix && video_scale_fix && video_sink))
	{
		gst_object_unref(source);
		gst_object_unref(main_queue);
		gst_object_unref(decoder);
		gst_object_unref(video_queue);
		gst_object_unref(video_color_fix);
		gst_object_unref(video_scale_fix);
		gst_object_unref(video_sink);
		return false;
	}

	// Set up the input source to read from the input pipe.
	g_object_set(G_OBJECT(source), "fd", pipe_halves[PIPE_OUT], NULL);

	// Set up the main buffer to hold as much data as the client feels like
	// shoving in.
	g_object_set(G_OBJECT(main_queue), "max-size-bytes", 0, NULL);
	g_object_set(G_OBJECT(main_queue), "max-size-time", 0ll, NULL);
	g_object_set(G_OBJECT(main_queue), "max-size-buffers", 0, NULL);

	// Set things up so that when the decoder finds video content, we can
	// have it linked into the pipeline automatically.
	g_signal_connect(decoder, "new-decoded-pad",
		(GCallback)(handle_new_decoder_pad), video_queue);

	// Assemble the non-decoder-dependent parts of the pipeline.
	gst_bin_add_many(GST_BIN(pipeline), source, main_queue, decoder,
		video_queue, video_color_fix, video_scale_fix, video_sink,
		NULL);
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));

	gboolean linked = gst_element_link(source, main_queue);
	linked = linked && gst_element_link(main_queue, decoder);
	linked = linked && gst_element_link(video_queue, video_color_fix);
	linked = linked && gst_element_link(video_color_fix, video_scale_fix);
	linked = linked && gst_element_link(video_scale_fix, video_sink);

	return linked;
}


bool Video_Player::Create_pipe()
{
	return (pipe(pipe_halves) == 0);
}
