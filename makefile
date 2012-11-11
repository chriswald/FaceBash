CC=g++
CFLAGS=-Wall -I/usr/local/include -I/usr/include -L/usr/local/lib -ljson -lcurlpp -lstdc++ -lcurl -lreadline -lncurses
LDFLAGS=
DEBUG=-g
SOURCES=src/*.cpp src/*.h
EXECUTABLE=facebash
EXEPATH=bin/
EXE=

all:
	$(CC) $(SOURCES) $(CFLAGS) -o $(EXEPATH)$(EXECUTABLE)$(EXE)

debug:
	$(CC) $(SOURCES) $(CFLAGS) $(DEBUG) -o $(EXEPATH)$(EXECUTABLE)$(EXE)