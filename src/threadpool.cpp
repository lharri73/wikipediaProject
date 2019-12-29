#include "thread_pool.hpp"
#include "wikipedia.hpp"

using namespace std;

ThreadPool::ThreadPool(size_t threads, string goal_page, int max_n, volatile sig_atomic_t &gSignalStatus){
    Finder *finder;
    for(size_t i = 0; i<threads; i++){
        finder = new Finder(goal_page, max_n);
        threadVec.push_back(new Thread(finder));
    }
    while(gSignalStatus !=2){
        for(size_t i = 0; i < threads; i++){
            if(!threadVec[i]->isRunning()){
                cerr << "restarting thread " << i << '\n';
                delete threadVec[i];
                if(!threadVec[i]->isSigint()){
                    finder = new Finder(goal_page, max_n);
                    threadVec[i] = new Thread(finder);
                }
            }
        }
    }
}

void ThreadPool::add_thread(Finder* finder){
    threadVec.push_back(new Thread(finder));
}

ThreadPool::~ThreadPool(){
    for(size_t i = 0; i < threadVec.size(); i++){
        delete threadVec[i];
    }
}

//-----------------------------------------------------------------------------
bool Thread::isRunning() const{
    return running;
}

bool Thread::isSigint() const{
    return sigint;
}

Thread::Thread(Finder* input_finder){
    running = true;
    sigint = false;
    finder = input_finder;
    multiThread = new thread(multithread_start, input_finder, &running, &sigint);
}

Thread::~Thread(){
    multiThread->join();
    delete multiThread;
    delete finder;
}

void multithread_start(Finder* finder, bool* running, bool* sigint){
    *running = true;
    finder->begin();
    *running = false;
    if(finder->sigInt)
        *sigint = true;
}
