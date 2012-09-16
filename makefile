CC=g++
CFLAGS=-Wall -I/usr/local/include -I/usr/include -L/usr/local/lib -lcurlpp -lstdc++ -lcurl
LDFLAGS=
SOURCES= src/curlpp.cpp src/LoginField.cpp src/LoginField.h
EXECUTABLE=FaceBash
EXEPATH=bin/
EXE=

all:
	$(CC) $(SOURCES) $(CFLAGS) -o $(EXEPATH)$(EXECUTABLE)$(EXE)