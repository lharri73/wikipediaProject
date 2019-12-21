#include "wikipedia.hpp"
using namespace std;
Page::Page(string page_name, GumboOutput* Output, string Filename, string pageFolder){
    name = page_name;
    output = Output;
    fileName = Filename;
    pages_folder = pageFolder;
    get_links();
}

Page::~Page(){
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    for(size_t i = 0; i < pages.size(); i++){
        delete pages[i];
    }
    remove(fileName.c_str());
}

void Page::get_links(){
    get_links_recursive(output->root);
    // for(size_t i = 0; i < links.size(); i++){
    //     cout << links[i].get_href() << '\n';
    // }
}

bool Page::get_links_recursive(GumboNode *node){
    if (node->type != GUMBO_NODE_ELEMENT) {
        // continue looking
        return true;
    }

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
               result.find("Templage:") == string::npos && \
               result.find("File:") == string::npos && \
               result.find("/wiki/") == 0){
                   GumboAttribute* title = gumbo_get_attribute(&node->v.element.attributes, "title");
                   links.push_back(Link(href->value, title->value));
            }
    }

  GumboVector* children = &node->v.element.children;
  for (unsigned int i = 0; i < children->length; ++i) {
    if(!get_links_recursive(static_cast<GumboNode*>(children->data[i]))){
        return false;
    }
  }
  return true;
}

Page* Page::get_sub_page(Link link){
    string root_page = pages_folder + "/" + gen_uuid() + ".webpage";
                                                              // wget prints a lot of garbage
    string command="wget -O " + root_page + " " + link.get_href() + " >/dev/null 2>&1";
    system((const char*)command.c_str());
    // const char* filename = argv[1];

    ifstream in(root_page, ios::in | ios::binary);
    if (!in) {
        std::cout << "File " << root_page << " not found!\n";
        exit(EXIT_FAILURE);
    }

    string contents;
    in.seekg(0, ios::end);
    contents.resize(in.tellg());
    in.seekg(0, ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    GumboOutput* output = gumbo_parse(contents.c_str());

    const string title = find_title(output->root);
    Page *cur_page = new Page(title, output, root_page, pages_folder);
    pages.push_back(cur_page);
    return cur_page;
}
