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
