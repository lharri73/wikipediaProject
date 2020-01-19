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
    }
    args Args = parse_args(argc, argv);
	if(Args.debug){
		Finder *finder = new Finder(Args);
		finder->begin();
		delete finder;
	}
    if(Args.singleThread){
        Finder *finder = new Finder(Args);
		while(true){
			finder->begin();
		}
        delete finder;
    }else{
        cleanup();
        unsigned concurentThreadsSupported = std::thread::hardware_concurrency();

        size_t numThreads = concurentThreadsSupported == 0 ? 1 : concurentThreadsSupported; // will return 0 if unable to detect
        cout << "Working with " << numThreads << " thread(s)\n";

        ThreadPool(numThreads, Args, gSignalStatus);
        
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

args parse_args(int argc, char** argv){
    (void)argc;
    (void)argv;
    args ret;
    // 0 progNmae;
    if(argc < 3){
        printf("usage: %s 'goal_page_name' max_depth\n", argv[0]);
    }

    for(int i = 1; i < argc; i++){
        if(i == 1){
            ret.goal_page = string(argv[i]);
            continue;
        }else if(i == 2){
            ret.max_n = atoi(argv[i]);
            continue;
        }

        if(argv[i] == string("--single"))
            ret.singleThread = true;

		if(argv[i] == string("--debug"))
			ret.singleThread = true;

        if(argv[i] == string("--sql_ip")){
            if(argc == i){
                cout << "must specify ip following '--sql_ip'\n";
                exit(-1);
            }
            ret.mysql_ip = string(argv[++i]);
            ret.useSql = true;
            continue;
        }

        if(argv[i] == string("--sql_user")){
            if(argc == i){
                cout << "must specify user following '--sql_user'\n";
                exit(-1);
            }
            ret.mysql_user = string(argv[++i]);
            ret.useSql = true;
            continue;
        }

        if(argv[i] == string("--sql_pass")){
            if(argc == i){
                cout << "must specify pass following '--sql_pass'\n";
                exit(-1);
            }
            ret.mysql_pass = string(argv[++i]);
            ret.useSql = true;
            continue;
        }

        if(argv[i] == string("--ram")){
            if(argc == i){
                cout << "must specify ram in gb following '--ram'\n";
                exit(-1);
            }
            ret.ramAmount = atof(argv[++i]);
            continue;
        }
    }

    //------------

    if(ret.ramAmount == 0.0){
        cout << "+----------------------------------------------------+\n";
        cout << "| WARNING: Did not specify RAM amount...assuming 8Gb |\n";
        cout << "|         (specify with --ram #inGb)                 |\n";
        cout << "+----------------------------------------------------+\n";
        ret.ramAmount = 8.0;
    }
    return ret;
}

args::args(){
    goal_page = "";
    max_n = 0;
    ramAmount = 0;
    singleThread = false;
    useSql = true;
    mysql_ip = "127.0.0.1";
    mysql_user = "root";
    mysql_pass = "";
    resultsFile = "";
	debug = false;
}
