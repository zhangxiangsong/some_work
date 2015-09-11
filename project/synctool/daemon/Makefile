#This is makefile.

CC      = gcc
CXX     = g++

DEBUG   = -ggdb

CFLAGS  = $(DEBUG) -Wall
LDFLAGS =

OBJS1    = server_begin.o
OBJS2    = server_end.o 
OBJS3    = sync_deamon.o

TARGET1 = server_begin
TARGET2 = server_end
TARGET3 = sync_daemon

all:$(TARGET1)  $(TARGET2)  $(TARGET3)

$(TARGET1): $(OBJS1)
	$(CXX) $(CFLAGS) -o $@ $(OBJS1) $(LDFLAGS)
	cp -f $@ ../bin

$(TARGET2): $(OBJS2)
	$(CXX) $(CFLAGS) -o $@ $(OBJS2) $(LDFLAGS)
	cp -f $@ ../bin

$(TARGET3): $(OBJS3)
	$(CXX) $(CFLAGS) -o $@ $(OBJS3) $(LDFLAGS)
	cp -f $@ ../bin


%.o:%.c
	$(CXX) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS1)
	rm -f $(OBJS2)
	rm -f $(OBJS3)
	rm -f $(TARGET1)
	rm -f $(TARGET2)
	rm -f $(TARGET3)
