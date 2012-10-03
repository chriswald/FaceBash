CC=g++
CFLAGS=-Wall -I/usr/local/include -I/usr/include -L/usr/local/lib -ljson_linux-gcc-4.6_libmt -lcurlpp -lstdc++ -lcurl
LDFLAGS=
#SOURCES=src/facebash.cpp src/LoginField.cpp src/LoginField.h
SOURCES=src/*.cpp src/*.h
EXECUTABLE=FaceBash
EXEPATH=bin/
EXE=

all:
	$(CC) $(SOURCES) $(CFLAGS) -o $(EXEPATH)$(EXECUTABLE)$(EXE)