#include "thread_pool.hpp"
#include "wikipedia.hpp"

using namespace std;

ThreadPool::ThreadPool(size_t threads, args Args, volatile sig_atomic_t &gSignalStatus){
    Finder *finder;
    ramAmount = Args.ramAmount;
    for(size_t i = 0; i<threads; i++){
        finder = new Finder(Args);
        threadVec.push_back(new Thread(finder));
    }

    double vRam, ret_usage;
    Thread* oldThread;
    int j = 0;
    while(gSignalStatus !=2){
	    if(++j >= 10000) {
		    cout << threadVec.size() << '\n';
		    j = 0;
		}
        mem_usage(vRam, ret_usage);
       	if(vRam < .75 * ramAmount && threadVec.size() < threads){
	    cout << "starting new thread\n";
            finder = new Finder(Args);
            threadVec.push_back(new Thread(finder));
        }
        
        for(size_t i = 0; i < threadVec.size(); i++){
            if(!threadVec[i]->isRunning()){
                cout << "stopping thread " << i << '\n';
                oldThread = threadVec[i];
                // if(!threadVec[i]->isSigint()){
                //     finder = new Finder(Args);
                //     threadVec[i] = new Thread(finder);
                // }
                if(threadVec[i]->isSigint()){
		    gSignalStatus = 2;
                }
                threadVec.erase(threadVec.begin() + i);
                delete oldThread;
		i--;
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

void mem_usage(double& vm_usage, double& resident_set) {
   vm_usage = 0.0;
   resident_set = 0.0;
   ifstream stat_stream("/proc/self/stat",ios_base::in); //get info from procdirectory
   //create some variables to get info
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue, starttime;
   unsigned long vsize;
   long rss;
   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
   >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
   >> utime >> stime >> cutime >> cstime >> priority >> nice
   >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest
   stat_stream.close();
   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // for x86-64 is configured to use 2MB pages
   vm_usage = vsize /1073741824.0;
   resident_set = rss * page_size_kb;
}
