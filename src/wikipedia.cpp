#include "wikipedia.hpp"
using namespace std;

void cleanup();
void sigint_cleanup(int signal);

int main(int argc, char** argv){
    signal(SIGINT, sigint_cleanup);
    if(argc == 2 && string(argv[1])=="clean"){
        cleanup();
        exit(0);
    }else if(argc < 3){
        printf("usage: %s 'goal_page_name' max_depth\n", argv[0]);
        exit(1);
    }

    Finder finder = Finder(string(argv[1]), atoi(argv[2]), "results/results.csv");
    finder.begin();

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

string gen_uuid(){
        uuid_t binuuid;
    /*
     * Generate a UUID. We're not done yet, though,
     * for the UUID generated is in binary format 
     * (hence the variable name). We must 'unparse' 
     * binuuid to get a usable 36-character string.
     */
    uuid_generate_random(binuuid);

    /*
     * uuid_unparse() doesn't allocate memory for itself, so do that with
     * malloc(). 37 is the length of a UUID (36 characters), plus '\0'.
     */
    char *uuid = (char*)malloc(37);

    /*
     * Produces a UUID string at uuid consisting of letters
     * whose case depends on the system's locale.
     */
    uuid_unparse(binuuid, uuid);
    return uuid;
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

void sigint_cleanup(int signal){
    (void) signal;
    cleanup();
    exit(1);
}
