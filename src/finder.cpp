#include "wikipedia.hpp"
using namespace std;

Finder::Finder(string search_for, int max_n, string resultsFile){
    MAX = max_n-1;
    goal_page = search_for;
    pages_folder = "pages";
    results_file = resultsFile;
    random_url = "https://en.wikipedia.org/wiki/Special:Random";
    // random_url = "https://en.wikipedia.org/wiki/United_States";
    // random_url = "https://en.wikipedia.org/wiki/World_war";
    hasRun = false;
}
Finder::~Finder(){
    delete current_page;
}
Page* Finder::get_next_file(){
    if(hasRun)  delete current_page;
    hasRun = true;

    string root_page = pages_folder + "/" + gen_uuid() + ".webpage";
                                                              // wget prints a lot of garbage
    string command="wget -O " + root_page + " '" +random_url + "' >/dev/null 2>&1";
    system((const char*)command.c_str());
    // const char* filename = argv[1];

    ifstream in(root_page.c_str(), ios::in | ios::binary);
    if (!in) {
        std::cerr << "get_next_file: File " << root_page << " not found!\n";
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
        printf("FOUND %s\n", goal_page.c_str());
        path[n+1] = goal_page;
        return true;
    }

    for(size_t i = 0; i < page->links.size(); i++){
        if(page->links[i].get_title() == goal_page){
            printf("FOUND %s\n", goal_page.c_str());
            path[n+1] = page->links[i].get_title();
            return true;
        }
    }
   // don't do this on the last iteration
   Page* nextPage;
   if(n != MAX){
       for(size_t i = 0; i < page->links.size(); i++){
           nextPage = page->get_sub_page(page->links[i]);
           if(nextPage->name == "") continue;
           if(find_hitler_recursive(n+1, nextPage, path)){
               path[n+1] = page->links[i].get_title();
               return true;
           }
       }
   }
    return false;
}
void Finder::write_result(string* result){
    ofstream fout;
    fout.open(results_file.c_str(), ios_base::app);
    for(int i = 0; i < 4; i++){
        if (result[i].find(',' != string::npos)){
            fout << '"' << result[i] << '"';
        }else{
            fout << result[i];
        }
        if(i == 3){
            fout << '\n';
        }else{
            fout << ',';
        }
    }
}

void Finder::begin(){
    string path[4];
    bool result;
    Page *page = get_next_file();
    result = find_hitler_recursive(0, page, path);
    if(result){
        printf("path: ['%s', '%s', '%s', '%s']\n", page->name.c_str(), path[1].c_str(), path[2].c_str(), path[3].c_str());
        write_result(path);
    }else{
        path[0] = "NOT POSSIBLE";
        path[1] = page->name;
        write_result(path);
    }

}
