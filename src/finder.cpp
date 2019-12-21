#include "wikipedia.hpp"
using namespace std;

static const string find_title(const GumboNode* root);

Finder::Finder(string search_for, int max_n){
    MAX = max_n-1;
    goal_page = search_for;
    pages_folder = "pages";
    results_file = "results/results.csv";
    random_url = "https://en.wikipedia.org/wiki/Special:Random";
}
Finder::~Finder(){
    //TODO: does this need to delete pages?
}
Page Finder::get_next_file(){
    string root_page = pages_folder + "/" + gen_uuid() + ".webpage";
                                                              // wget prints a lot of garbage
    string command="wget -O " + root_page + " " +random_url + " >/dev/null 2>&1";
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
    cout << "output " <<  title << '\n';
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    return Page("");
}
bool Finder::find_hitler_recursive(int n, Page page, string *path){
    (void) n;
    (void) page;
    (void) path;
    return false;
}
void Finder::write_result(string result){
    (void) result;
}

static const string find_title(const GumboNode* root) {

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
