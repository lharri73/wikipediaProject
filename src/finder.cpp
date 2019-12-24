#include "wikipedia.hpp"
using namespace std;
mutex g_pages_mutex;
Finder::Finder(string search_for, int max_n, string resultsFile){
    
    MAX = max_n-1;
    goal_page = search_for;
    pages_folder = "pages";
    results_file = resultsFile;
    random_url = "https://en.wikipedia.org/wiki/Special:Random";
    // random_url = "https://en.wikipedia.org/wiki/United_States";
    // random_url = "https://en.wikipedia.org/wiki/World_war";
    
    hasRun = false;

    sigInt = false;
}

Finder::Finder(){
    sigInt = false;
}

Finder::~Finder(){
    delete current_page;
}
Page* Finder::get_next_file(){
    if(hasRun && !sigInt)  delete current_page;
    hasRun = true;

    uuid thisUUID;

    string root_page = "pages/" + thisUUID.uuid_string() + ".webpage";
                                                              // wget prints a lot of garbage
    string command="curl -L -o " + root_page + "  \"" +random_url + "\" >/dev/null 2>&1";
    int system_result = system((const char*)command.c_str());

    if(system_result != 0){
        throw (string) "sigint detected";
    }

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
    Page *cur_page = new Page(title, output, root_page);
    printf("Root: %s...\n",cur_page->name.c_str());
    current_page = cur_page;
    return cur_page;
}
bool Finder::find_hitler_recursive(int n, Page *page, string path[]){
    if(sigInt) return false;
    if(n > MAX) return false;

    // base case
    if(page->name == goal_page){
        path[n+1] = goal_page;
        return true;
    }

    for(size_t i = 0; i < page->links.size(); i++){
        if(page->links[i].get_title() == goal_page){
            path[n+1] = page->links[i].get_title();
            return true;
        }
    }
   // don't do this on the last iteration
   Page* nextPage;
   if(n != MAX){
       for(size_t i = 0; i < page->links.size() && !sigInt; i++){
            try{
                nextPage = page->get_sub_page(page->links[i]);
            }catch(string s){
                sigInt = true;
                return false;
            }

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

    lock_guard<mutex> guard(g_pages_mutex);

    if(!fout){
        cerr << "BAD RESULTS FILE\n " << results_file;
        exit(2);
    }

    for(int i = 0; i < 4; i++){
        fout << '"' << result[i] << '"';
        if(i == 3){
            fout << '\n';
        }else{
            fout << ',';
        }
    }
    fout.close();
}

void Finder::begin(){
    string path[4] = {"","","",""};
    bool result;
    try{
        get_next_file();
    }catch(string s){
        cerr << "detected sigint\n";
        return;
    }
    result = find_hitler_recursive(0, current_page, path);

    if(result){
        printf("FOUND: ['%s', '%s', '%s', '%s']\n", current_page->name.c_str(), path[1].c_str(), path[2].c_str(), path[3].c_str());
        path[0] = current_page->name.c_str();
        write_result(path);
    }else{
        path[0] = "NOT POSSIBLE";
        path[1] = current_page->name;
        if(!sigInt)
            write_result(path);
    }
}
void Finder::sigint(int signal){
    cout << "caught signal " << signal << '\n';
}
