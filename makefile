CC=g++
CFLAGS=-Wall -I/usr/local/include -I/usr/include -L/usr/local/lib -lcurlpp -lstdc++ -lcurl
LDFLAGS=
SOURCES=curlpp.cpp
EXECUTABLE=FaceBash
EXE=

all:
	$(CC) $(SOURCES) $(CFLAGS) -o $(EXECUTABLE)$(EXE)