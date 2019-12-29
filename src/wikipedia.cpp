#include "wikipedia.hpp"
#include "thread_pool.hpp"

using namespace std;

void cleanup();

volatile sig_atomic_t gSignalStatus = 0;
void signal_handler(int signal){
    gSignalStatus = signal;
}

int main(int argc, char** argv){
    signal(SIGINT, signal_handler);
    signal(SIGSEGV, handler);
    if(argc == 2 && string(argv[1])=="clean"){
        cleanup();
        exit(0);
    }else if(argc < 3){
        printf("usage: %s 'goal_page_name' max_depth\n", argv[0]);
        exit(1);
    }
    if(argc >=4 && string(argv[3]) == "single"){
        Finder *finder = new Finder(string(argv[1]), atoi(argv[2]));
        finder->begin();
        delete finder;
    }else{
        cleanup();
        unsigned concurentThreadsSupported = std::thread::hardware_concurrency();

        size_t numThreads = concurentThreadsSupported == 0 ? 1 : concurentThreadsSupported; // will return 0 if unable to detect
        cout << "Working with " << numThreads << " thread(s)\n";

        ThreadPool(numThreads, string(argv[1]), atoi(argv[2]), gSignalStatus);
        
        cout <<"\n----------------------------\n\tdetected sigint\n----------------------------\n";
    }

    return 0;
}

void cleanup(){
    glob_t glob_result;
    glob("pages/*",GLOB_TILDE,NULL,&glob_result);
    for(unsigned int i=0; i<glob_result.gl_pathc; ++i){
        if(string(glob_result.gl_pathv[i]) == "pages/.keep") continue;
        remove(glob_result.gl_pathv[i]);
    }
}

const string find_title(const GumboNode* root) {

    const GumboVector* root_children = &root->v.element.children;
    GumboNode* head = NULL;
    for (size_t i = 0; i < root_children->length; i++) {
        GumboNode* child = (GumboNode*) root_children->data[i];
        if (child->type == GUMBO_NODE_ELEMENT &&
            child->v.element.tag == GUMBO_TAG_HEAD) {
            head = child;
            break;
        }
    }

    GumboVector* head_children = &head->v.element.children;
    for (size_t i = 0; i < head_children->length; i++) {
        GumboNode* child = (GumboNode*) head_children->data[i];
        if (child->type == GUMBO_NODE_ELEMENT &&
            child->v.element.tag == GUMBO_TAG_TITLE) {
            GumboNode* title_text = (GumboNode*) child->v.element.children.data[0];
            string result = title_text->v.text.text;
            size_t trailed_pos = result.find(" - Wikipedia");
            if(trailed_pos == string::npos){
                // this happens when we receive "400 bad request" or simply find a link to a non-wikipedia page somehow
                return "BAD TITLE BECAUSE NPOS";
            }
            result.resize(trailed_pos);
            return result;
        }
    }
    return "<no title found>";
}

void handler(int sig){
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
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
