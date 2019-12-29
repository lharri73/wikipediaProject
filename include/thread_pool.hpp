#pragma once

#include "wikipedia.hpp"
#include <thread>
#include <condition_variable>

class Thread{
    public:
        Thread(Finder* finder);
        ~Thread();
        bool isRunning() const;
        bool isSigint() const;
    private:
        bool sigint;
        bool running;
        Finder* finder;
        std::thread* multiThread;
};

class ThreadPool {
    public:
        ThreadPool(size_t threads, args Args, volatile sig_atomic_t &gSignalStatus);
        void add_thread(Finder* finder);
        ~ThreadPool();
    private:
        double ramAmount;
        std::vector<Thread*> threadVec;
};

void multithread_start(Finder* finder, bool* running, bool* sigint);
