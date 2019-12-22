#include <iostream>

#include <filesystem>
#include <glob.h>

#include <uuid/uuid.h>
#include <cstdio>
#include <stdlib.h> 
#include <cstdlib>

#include "gumbo.h"
#include <fstream>
#include <ios>
#include <signal.h>

#include <vector>

class Link{
    public:
        Link(std::string Href, std::string Title);
        const std::string get_href() const;
        const std::string get_title() const;
    protected:
        std::string href;
        std::string title;
};

class Page{
    public:
        Page(std::string page_name, GumboOutput *output, std::string filename, std::string pages_folder);
        ~Page();

        Page* get_sub_page(Link link);

        std::string name;
        std::vector <Link> links;
    protected:
        void get_links();
        GumboOutput *output;
        std::string fileName;

        std::string pages_folder;
        std::vector <Page*> pages;
    private:
        bool get_links_recursive(GumboNode *node);
};

class Finder{
    public:
        Finder(std::string search_for, int max_n);
        ~Finder();
        Page *get_next_file();
        bool find_hitler_recursive(int n, Page* page, std::string *path);
        void write_result(std::string result);

        void begin();

        std::string file_name;
    protected:
        bool hasRun;
        int MAX;
        std::string goal_page;
        std::string random_url;
        std::string pages_folder;
        std::string results_file;

        Page* current_page;

};

std::string gen_uuid();
const std::string find_title(const GumboNode* root);
