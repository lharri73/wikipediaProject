CFLAGS ?= -std=c++11 -g -rdynamic -Wall -Wextra -Wpedantic -Iinclude -lstdc++fs
LFLAGS := -lgumbo -lmysqlcppconn -pthread

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

obj/wikipedia.o: src/wikipedia.cpp include/wikipedia.hpp include/thread_pool.hpp
	g++ $(CFLAGS) -c -o obj/wikipedia.o src/wikipedia.cpp

obj/finder.o: src/finder.cpp include/wikipedia.hpp
	g++ $(CFLAGS) -c -o obj/finder.o src/finder.cpp

obj/uuid.o: src/uuid.cpp include/wikipedia.hpp
	g++ $(CFLAGS) -c -o obj/uuid.o src/uuid.cpp

obj/sql.o: src/sql.cpp include/wikipedia.hpp
	g++ $(CFLAGS) -c -o obj/sql.o src/sql.cpp

obj/threadpool.o: src/threadpool.cpp include/thread_pool.hpp include/wikipedia.hpp
	g++ $(CFLAGS) -c -o obj/threadpool.o src/threadpool.cpp
# -------------------------
# Executables

bin/wikipedia: obj/wiki_link.o obj/wiki_page.o obj/wikipedia.o obj/finder.o obj/uuid.o obj/sql.o obj/threadpool.o include/thread_pool.hpp include/wikipedia.hpp
	g++ $(CFLAGS) -o bin/wikipedia obj/wiki_link.o obj/wiki_page.o obj/wikipedia.o obj/finder.o obj/uuid.o obj/sql.o obj/threadpool.o $(LFLAGS)
