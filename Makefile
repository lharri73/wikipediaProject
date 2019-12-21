CFLAGS ?= -std=c++98 -g -Wall -Wextra -Iinclude
LFLAGS := $(shell pkg-config --libs gumbo)

# ------------------------
# "make binaries" generates all required binaries

all: bin/wikipedia \

# -------------------------
# "make clean" deletes the object files and binaries

clean:
	rm -f obj/* bin/* pages/*
# -------------------------
# Object files

obj/wiki_link.o: src/wiki_link.cpp include/wikipedia.hpp
	g++ $(CFLAGS) -c -o obj/wiki_link.o src/wiki_link.cpp

obj/wiki_page.o: src/wiki_page.cpp include/wikipedia.hpp
	g++ $(CFLAGS) -c -o obj/wiki_page.o src/wiki_page.cpp

obj/wikipedia.o: src/wikipedia.cpp include/wikipedia.hpp
	g++ $(CFLAGS) -c -o obj/wikipedia.o src/wikipedia.cpp

obj/finder.o: src/finder.cpp include/wikipedia.hpp
	g++ $(CFLAGS) -c -o obj/finder.o src/finder.cpp
# -------------------------
# Executables

bin/wikipedia: obj/wiki_link.o obj/wiki_page.o obj/wikipedia.o obj/finder.o
	g++ $(CFLAGS) $(LFLAGS) -o bin/wikipedia obj/wiki_link.o obj/wiki_page.o obj/wikipedia.o obj/finder.o
