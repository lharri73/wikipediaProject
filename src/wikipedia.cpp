#include "wikipedia.hpp"
#include "thread_pool.hpp"

using namespace std;



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

        ThreadPool(numThreads, Args);
        
    }

    return 0;
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



