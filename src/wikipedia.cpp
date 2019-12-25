#include "wikipedia.hpp"
using namespace std;

void cleanup();
void multithread_start(string goa_page, int max_depth);

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
        multithread_start(string(argv[1]), atoi(argv[2]));
    }else{
        vector<thread> threads;
        unsigned concurentThreadsSupported = std::thread::hardware_concurrency();

        size_t numThreads = concurentThreadsSupported == 0 ? 1 : concurentThreadsSupported; // will return 0 if unable to detect
	cout << numThreads << '\n';
        for(size_t i =0; i < numThreads; i++){
            threads.push_back(thread(multithread_start, string(argv[1]), atoi(argv[2])));
        }

        while(gSignalStatus != 2){
            sleep(1);
        }

        for(size_t i = 0; i < threads.size(); i++){
            threads[i].join();
        }
    }

    return 0;
}

void multithread_start(string goal_page, int max_depth){
    Finder *finder = new Finder(goal_page, max_depth, "results/results.csv");
    while(gSignalStatus != 2){
        try{
            finder->begin();
            if(finder->sigInt){
                break;
            }
        }catch(length_error){
            cerr << "caught length_error\n";
        }
    }
    delete finder;
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
