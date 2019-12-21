#include <iostream>

#include <filesystem>
#include <glob.h>

#include <uuid/uuid.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <cstdlib>

#include "gumbo.h"
#include <fstream>
#include <ios>
class Page{
    public:
        Page(std::string page_name);
        std::string name;
};

class Link{

};

class Finder{
    public:
        Finder(std::string search_for, int max_n);
        ~Finder();
        Page get_next_file();
        bool find_hitler_recursive(int n, Page page, std::string *path);
        void write_result(std::string result);

        void begin();

        std::string file_name;
    protected:
        int MAX;
        std::string goal_page;
        std::string random_url;
        std::string pages_folder;
        std::string results_file;

};

std::string gen_uuid();
