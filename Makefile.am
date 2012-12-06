AUTOMAKE_OPTIONS = foreign

CFLAGS=-Wall -I/usr/local/include -I/usr/include -DVERSION=\"$(VERSION)\" -DPROG="\"$(PACKAGE)\""
LDFLAGS=-L/usr/local/lib -ljson -lcurlpp -lstdc++ -lcurl -lreadline -lncurses

bin_PROGRAMS = facebash
facebash_SOURCES = src/ArgParse.cpp src/ArgParse.h src/Comment.cpp src/Comment.h src/facebash.cpp src/Journal.cpp src/Journal.h src/LoginField.cpp src/LoginField.h src/NetUtils.cpp src/NetUtils.h src/NewsStory.cpp src/NewsStory.h src/Utils.cpp src/Utils.h src/utf8.h src/utf8/checked.h src/utf8/unchecked.h src/utf8/core.h