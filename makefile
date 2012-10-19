CC=g++
CFLAGS=-Wall -I/usr/local/include -I/usr/include -L/usr/local/lib -ljson_linux-gcc-4.6_libmt -lcurlpp -lstdc++ -lcurl -lreadline -lncurses
LDFLAGS=
SOURCES=src/*.cpp src/*.h
EXECUTABLE=facebash
EXEPATH=bin/
EXE=

all:
	$(CC) $(SOURCES) $(CFLAGS) -o $(EXEPATH)$(EXECUTABLE)$(EXE)