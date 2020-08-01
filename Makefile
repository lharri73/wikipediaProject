CFLAGS ?= -std=c++11 -g -rdynamic -Wall -Wextra -Wpedantic -Iinclude
LFLAGS := -lgumbo -lmysqlcppconn -pthread

# ------------------------
# "make binaries" generates all required binaries

all: bin/wikipedia \


setup: 
	sudo apt install libtool automake libmysqlcppconn-dev 
	git submodule init 
	git submodule update 
	cd lib/gumbo && ./autogen.sh && ./configure && make && sudo make install

# -------------------------
# "make clean" deletes the object files and binaries

clean:
	rm -f obj/* bin/* pages/*
# -------------------------
# Object files

obj/wiki_link.o: src/wiki_link.cpp include/wikipedia.hpp include/Link.hpp
	g++ $(CFLAGS) -c -o obj/wiki_link.o src/wiki_link.cpp

obj/wiki_page.o: src/wiki_page.cpp include/wikipedia.hpp include/Page.hpp
	g++ $(CFLAGS) -c -o obj/wiki_page.o src/wiki_page.cpp

obj/wikipedia.o: src/wikipedia.cpp include/wikipedia.hpp include/Args.hpp
	g++ $(CFLAGS) -c -o obj/wikipedia.o src/wikipedia.cpp

obj/finder.o: src/finder.cpp include/wikipedia.hpp include/Finder.hpp
	g++ $(CFLAGS) -c -o obj/finder.o src/finder.cpp

obj/uuid.o: src/uuid.cpp include/wikipedia.hpp include/UUID.hpp
	g++ $(CFLAGS) -c -o obj/uuid.o src/uuid.cpp

# obj/sql.o: src/sql.cpp include/wikipedia.hpp
# 	g++ $(CFLAGS) -c -o obj/sql.o src/sql.cpp

obj/threadpool.o: src/threadpool.cpp include/thread_pool.hpp
	g++ $(CFLAGS) -c -o obj/threadpool.o src/threadpool.cpp

obj/arg_handler.o: src/arg_handler.cpp include/Args.hpp
	g++ $(CFLAGS) -c -o obj/arg_handler.o src/arg_handler.cpp

obj/signal_handler.o: src/signal_handler.cpp include/Signals.hpp
	g++ $(CFLAGS) -c -o obj/signal_handler.o src/signal_handler.cpp
# -------------------------
# Executables

bin/wikipedia: obj/wiki_link.o obj/wiki_page.o obj/wikipedia.o obj/finder.o obj/uuid.o obj/threadpool.o obj/signal_handler.o obj/arg_handler.o
	g++ $(CFLAGS) -o bin/wikipedia obj/wiki_link.o obj/wiki_page.o obj/wikipedia.o obj/finder.o obj/uuid.o obj/threadpool.o obj/signal_handler.o obj/arg_handler.o $(LFLAGS)
