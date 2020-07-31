#pragma once

#include <string>
#include <Link.hpp>
#include <set>
#include <fstream>
#include "gumbo.h"

class Page{
    public:
        Page(std::string page_name, GumboOutput *output, std::string filename);
        ~Page();

        Page* get_sub_page(Link &link);

        std::string name;
        std::set <Link> links;
    protected:
        void get_links();
        GumboOutput *output;
        std::string fileName;

    private:
        bool get_links_recursive(GumboNode *node);
};
const std::string find_title(const GumboNode* root);
