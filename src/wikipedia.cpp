#include <iostream>
#include <filesystem>
#include <glob.h>
#include<stdio.h> 
#include<stdlib.h> 
using namespace std;

void cleanup();

int main(int argc, char ** argv){
    if(argc == 2 && string(argv[1])=="clean"){
        cleanup();
        exit(0);
    }else if(argc < 3){
        printf("usage: %s 'goal_page_name' max_depth\n", argv[0]);
        exit(1);
    }
    cout << "here\n";
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
