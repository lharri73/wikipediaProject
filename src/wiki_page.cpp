#include "wikipedia.hpp"
using namespace std;
Page::Page(string page_name, GumboOutput* Output, string Filename){
    name = page_name;
    output = Output;
    fileName = Filename;
    get_links();
}

Page::~Page(){
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    remove(fileName.c_str());
}

void Page::get_links(){
    get_links_recursive(output->root);
}

bool Page::get_links_recursive(GumboNode *node){
    if (node->type != GUMBO_NODE_ELEMENT) {
        // continue looking
        return true;
    }

    // stop looking at class=navbox-list

    GumboAttribute* href;
    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
            if(href->value == string("/wiki/Help:Category")){

                // stop looking
                return false;
            }
            string result = href->value;
            if(result.find("Wikipedia:") == string::npos && \
               result.find("Special:") == string::npos && \
               result.find("Help:") == string::npos && \
               result.find("Template:") == string::npos && \
               result.find("Template talk:") == string::npos && \
               result.find("User:") == string::npos && \
               result.find("File:") == string::npos && \
               result.find("/wiki/") == 0){
                GumboAttribute* title = gumbo_get_attribute(&node->v.element.attributes, "title");
                if(title){
                    links.push_back(Link(href->value, title->value));
                }
            }
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; i++) {
        if(!get_links_recursive(/*static_cast<GumboNode*>(*/(GumboNode*)children->data[i])){
            return false;
        }
    }
    return true;
}

Page* Page::get_sub_page(Link &link){
    uuid thisUUID;
    string root_page = "pages/" + thisUUID.uuid_string() + ".webpage";
                                                        // wget/curl prints a lot of garbage
    string command="curl -L -o " + root_page + " \"" + link.get_href() + "\" >/dev/null 2>&1";
    int system_result = system(command.c_str());
    
    if(system_result != 0){
        throw (string) "sigint detected";
    }

    ifstream in(root_page.c_str(), ios::in | ios::binary);
    if (!in) {
        std::cerr << "get_sub_page: File " << root_page << " not found!\n";
        exit(-1);
    }

    string contents;
    in.seekg(0, ios::end);
    contents.resize(in.tellg());
    in.seekg(0, ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    GumboOutput* output = gumbo_parse(contents.c_str());

    const string title = find_title(output->root);
    Page *cur_page = new Page(title, output, root_page);
    return cur_page;
}
