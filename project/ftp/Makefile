#This is makefile.

CC      = gcc
CXX     = g++

DEBUG   =

CFLAGS  = $(DEBUG) -Wall
LDFLAGS =  -lpthread
OBJS    = ftp_manager.o main.o server.o

ftpserver: $(OBJS)
	$(CXX) $(CFLAGS) -o $@ -g $(OBJS) $(LDFLAGS)
%.o:%.cpp
	$(CXX) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS)
	rm -f ftpserver
