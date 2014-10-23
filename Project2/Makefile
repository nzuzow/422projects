# CSE 422 Lab 2 makefile
# Modified from CSE422 FS12

# Options to set when compiling/linking the project.
CXXFLAGS=-g
LDFLAGS=-pthread

# The name of the executable to generate.
TARGET=client proxy


# The objects that should be compiled from the project source files (expected
# to correspond to actual source files, e.g. URL.o will come from URL.cc).
#
# You will want to add the name of your driver object to this list.
client_OBJS=HTTP_Message.o \
	HTTP_Request.o \
	HTTP_Response.o \
	TCP_Socket.o\
	URL.o \
	client.o \

proxy_OBJS=HTTP_Message.o \
	HTTP_Request.o \
	HTTP_Response.o \
	TCP_Socket.o\
	URL.o \
	Proxy_Worker.o \
	proxy.o


# Have everything built automatically based on the above settings.
all: $(TARGET)

.cc.o:
	g++ -o $@ $(CXXFLAGS) -c $< -DBUFFER_SIZE=40960

client: $(client_OBJS)
	g++ -o $@ $^ $(LDFLAGS)

proxy: $(proxy_OBJS)
	g++ -o $@ $^ $(LDFLAGS)

clean:
	$(RM) $(TARGET) $(proxy_OBJS) $(client_OBJS)
	$(RM) -rf Download


# Dependencies follow (i.e. which source files and headers a given object is
# built from).
TCP_Socket.o: TCP_Socket.h TCP_Socket.cc
URL.o: URL.cc URL.h
HTTP_Message.o: HTTP_Message.cc HTTP_Message.h
HTTP_Request.o: HTTP_Request.cc HTTP_Request.h HTTP_Message.h URL.o TCP_Socket.o
HTTP_Response.o: HTTP_Response.cc HTTP_Response.h HTTP_Message.h URL.o TCP_Socket.o
client.o: client.cc client.h HTTP_Request.o HTTP_Response.o
Proxy_Worker.o: Proxy_Worker.cc Proxy_Worker.h HTTP_Request.o HTTP_Response.o URL.o
proxy.o: proxy.cc Proxy_Worker.o
