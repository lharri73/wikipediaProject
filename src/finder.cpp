#include "wikipedia.hpp"
using namespace std;

Finder::Finder(string search_for, int max_n){
    MAX = max_n-1;
    goal_page = search_for;
    pages_folder = "pages";
    results_file = "results/results.csv";
    random_url = "https://en.wikipedia.org/wiki/Special:Random";
}
Finder::~Finder(){
    delete current_page;
}
Page* Finder::get_next_file(){
    delete current_page;

    string root_page = pages_folder + "/" + gen_uuid() + ".webpage";
                                                              // wget prints a lot of garbage
    string command="wget -O " + root_page + " " +random_url + " >/dev/null 2>&1";
    system((const char*)command.c_str());
    // const char* filename = argv[1];

    ifstream in(root_page, ios::in | ios::binary);
    if (!in) {
        std::cout << "File " << root_page << " not found!\n";
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
    Page *cur_page = new Page(title, output, root_page, pages_folder);
    printf("Root: %s...\n",cur_page->name.c_str());
    current_page = cur_page;
    return cur_page;
}
bool Finder::find_hitler_recursive(int n, Page *page, string *path){
    if(n > MAX) return false;

    // base case
    if(page->name == goal_page){
        printf("FOUND %s", goal_page.c_str());
        *path = goal_page + *path;
        return true;
    }

    for(size_t i = 0; i < page->links.size(); i++){
        if(page->links[i].get_title() == goal_page){
            printf("FOUND %s", goal_page.c_str());
            *path = page->links[i].get_title() + *path;
            return true;
        }
    }
   // don't do this on the last iteration
   Page* nextPage;
   if(n != MAX){
       for(size_t i = 0; i < page->links.size(); i++){
           nextPage = page->get_sub_page(page->links[i]);
           if(nextPage->name == "") continue;
           if(find_hitler_recursive(++n, nextPage, path)){
               *path = page->links[i].get_title() + *path;
               return true;
           }
       }
   }
    return false;
}
void Finder::write_result(string result){
    (void) result;
}

void Finder::begin(){
    string *path = new string("");
    bool result;
    result = find_hitler_recursive(MAX, get_next_file(), path);
    printf("result: %d\npath: '%s'\n", result,path->c_str());
}
