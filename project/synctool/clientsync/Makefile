#This is makefile.

CC      = gcc
CXX     = g++

DEBUG   =

CFLAGS  = $(DEBUG) -fshort-wchar -Wall -I /workspace/software/libcurl/include
LDFLAGS = -L/workspace/software/libcurl/lib  -lcurl -lpthread
OBJS    = Curl_parser.o Device_Control.o SyncControl.o Markup.o public.o log.o \
          Clientsync.o UniConversion.o lock.o

synctool: $(OBJS)
	$(CXX) $(CFLAGS) -o $@ -g $(OBJS) $(LDFLAGS)
	cp -f $@ ../bin
%.o:%.cpp
	$(CXX) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS)
	rm -f synctool

cleanvs:
	rm -rf log
	rm -rf Debug
	rm -rf Release
	rm -f  *.user
