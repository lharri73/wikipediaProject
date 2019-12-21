#include "wikipedia.hpp"
using namespace std;

void cleanup();

int main(int argc, char** argv){

    if(argc == 2 && string(argv[1])=="clean"){
        cleanup();
        exit(0);
    }else if(argc < 3){
        printf("usage: %s 'goal_page_name' max_depth\n", argv[0]);
        exit(1);
    }

    Finder finder = Finder(string(argv[1]), stoi(argv[2]));
    finder.get_next_file();

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
