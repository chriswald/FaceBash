AUTOMAKE_OPTIONS = foreign

CPPFLAGS=-Wall -I/usr/local/include -I/usr/include -DPKGPYEXECDIR=\"$(pyexecdir)\" -DPKGVERSION=\"$(VERSION)\" -DPROG="\"$(PACKAGE)\""
LDFLAGS=-L/usr/local/lib -ljson -lcurlpp -lstdc++ -lcurl -lreadline -lncurses

bin_PROGRAMS = facebash
facebash_SOURCES = ArgParse.cpp ArgParse.h Comment.cpp Comment.h config.h facebash.cpp Journal.cpp Journal.h LoginField.cpp LoginField.h NetUtils.cpp NetUtils.h NewsStory.cpp NewsStory.h Utils.cpp Utils.h utf8.h utf8/checked.h utf8/unchecked.h utf8/core.h