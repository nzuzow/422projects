CPPFLAGS=-I/user/cse422b/fs14/include/gstreamer-0.10 \
	-I/user/cse422b/fs14/include/glib-2.0 \
	-I/user/cse422b/fs14/lib/glib-2.0/include \
	-I/user/cse422b/fs14/include/libxml2
CXXFLAGS=$(CPPFLAGS) -g
LDFLAGS=-L/user/cse422b/fs14/lib -lgstreamer-0.10 -lgstapp-0.10  -lglib-2.0 -lgobject-2.0\
	-Wl,-rpath,/user/cse422b/fs14/lib

CLIENT=stream_client
CLIENT_OBJS=Video_Player.o \
	stream_client.o \
	Playlist_Entry.o \
	Playlist.o \
	HTTP_Message.o \
	HTTP_Request.o \
	HTTP_Response.o \
	TCP_Socket.o \
	URI.o

TEST_CLIENT=test_client
TEST_CLIENT_OBJS=Video_Player.o \
	test_client.o


all: $(CLIENT) $(TEST_CLIENT)

%.o : %.cc
	g++ -c $< $(CXXFLAGS) -o $@


$(CLIENT): $(CLIENT_OBJS)
	g++ -o $@ $^ $(LDFLAGS)

$(TEST_CLIENT): $(TEST_CLIENT_OBJS)
	g++ -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(CLIENT) $(CLIENT_OBJS) $(TEST_CLIENT) $(TEST_CLIENT_OBJS)
