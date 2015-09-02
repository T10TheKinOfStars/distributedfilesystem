#!/bin/sh

OPT=-O2
#TEST=-DTEST
DEBUG=-g -DDEBUG 
THRIFT_PATH=/usr/local


CFLAGS=-std=c++11 -Wall -Wno-format -fPIC $(OPT) $(DEBUG) $(TEST)

CC=g++
LIBS = -lthrift
INCLUDE_PATH = -I $(THRIFT_PATH)/include/ -I $(THRIFT_PATH)/include/thrift/
LIB_PATH = -L$(THRIFT_PATH)/lib/

OBJS=FileStore.o chord_types.o chord_constants.o fileworker.o sha256.o DHTController.o network.o
OBJSERVER=FileStore_server.o
OBJJRCLIENT=FileStore_jrclient.o
OBJIOCLIENT=FileStore_IOclient.o


all: server jrclient ioclient

server: $(OBJS) $(OBJSERVER)
	$(CC) -o server.exec $^ $(CFLAGS) $(LIBS) $(LIB_PATH) $(DEBUG)
	
jrclient: $(OBJS) $(OBJJRCLIENT)
	$(CC) -o jrclient.exec $^ $(CFLAGS) $(LIBS) $(LIB_PATH) $(DEBUG)
	
ioclient: $(OBJS) $(OBJIOCLIENT)
	$(CC) -o ioclient.exec $^ $(CFLAGS) $(LIBS) $(LIB_PATH) $(DEBUG)

%.o: %.cpp
	$(CC) $(INCLUDE_PATH) -c $(CFLAGS) -o $@ $^ $(DEBUG)

clean:
	rm -f *.o *.exec

rebuild: clean all



