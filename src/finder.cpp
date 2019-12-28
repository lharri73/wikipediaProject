#include "wikipedia.hpp"
using namespace std;
mutex g_pages_mutex;
Finder::Finder(string search_for, int max_n, string resultsFile){
    sql_connection = new SQLConnector();
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
    delete sql_connection;
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
        cerr << "get_next_file: File " << root_page << " not found!\n";
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
    if(title == "BAD TITLE BECAUSE NPOS"){
        return get_next_file();
    }
    Page *cur_page = new Page(title, output, root_page);
    printf("Root : %s...\n",cur_page->name.c_str());
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
    cerr << "Don't use this function\n";
    sql_connection->write(result);
}

void Finder::begin(){
    string * path = new string[4];
    for(size_t i = 0; i < 3; i++) path[i]="";
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
        sql_connection->write(path);
    }else{
        path[0] = "NOT POSSIBLE";
        path[1] = current_page->name;
        if(!sigInt){
            sql_connection->write(path);
	}
    }
    delete [] path;
}
void Finder::sigint(int signal){
    cout << "caught signal " << signal << '\n';
}
