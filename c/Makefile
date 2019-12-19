CFLAGS ?= -std=c++98 -g -Wall -Wextra -Iinclude

# -------------------------
# This means that when you just type "make", you make all of the executables

ALL = bin/wikipedia \

all: $(ALL)

# -------------------------
# "make clean" deletes the object files and binaries

clean:
	rm -f obj/* bin/*
# -------------------------
# Object files

obj/wiki_link.o: src/wiki_link.cpp include/wikipedia.hpp
	g++ $(CFLAGS) -c -o obj/wiki_link.o src/wiki_link.cpp

obj/wiki_page.o: src/wiki_page.cpp include/wikipedia.hpp
	g++ $(CFLAGS) -c -o obj/wiki_page.o src/wiki_page.cpp

obj/wikipedia.o: src/wikipedia.cpp include/wikipedia.hpp
	g++ $(CFLAGS) -c -o obj/wikipedia.o src/wikipedia.cpp

# -------------------------
# Executables

bin/wikipedia: obj/wiki_link.o obj/wiki_page.o obj/wikipedia.o
	g++ $(CFLAGS) -o bin/wikipedia obj/wiki_link.o obj/wiki_page.o obj/wikipedia.o
