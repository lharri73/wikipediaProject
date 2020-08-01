#include "wikipedia.hpp"
using namespace std;
Finder::Finder(args Args){
    //sql_connection = new SQLConnector(Args.mysql_ip, Args.mysql_user, Args.mysql_pass, Args.goal_page);
    MAX = Args.max_n-1;
    goal_page = Args.goal_page;
    pages_folder = "pages";
    random_url = "https://en.wikipedia.org/wiki/Special:Random";

    // values used for testing
    //random_url = "https://en.wikipedia.org/wiki/United_States";
    //random_url = "https://en.wikipedia.org/wiki/World_war";
    
    hasRun = false;
    sigInt = false;
}

Finder::~Finder(){
    //delete sql_connection;
    if(hasRun)
        delete current_page;
}
Page* Finder::get_next_file(){
    if(hasRun && !sigInt)  delete current_page;
    hasRun = true;

    uuid thisUUID;

    string root_page = "pages/" + thisUUID.uuid_string() + ".webpage";
                                                              // wget prints a lot of garbage
    string command="curl -L -o " + root_page + "  \"" +random_url + "\" >/dev/null 2>&1";
    int system_result = system(command.c_str());

    if(system_result != 0){
        throw (string) "sigint detected";
    }

    ifstream in(root_page.c_str(), ios::in | ios::binary);
    if (!in) {
        cerr << "get_next_file: File " << root_page << " not found!\n";
        exit(-1);
    }

    // read the contents of the entire file
    string contents;
    in.seekg(0, ios::end);
    contents.resize(in.tellg());
    in.seekg(0, ios::beg);
    in.read(&contents[0], contents.size());
    in.close();

    GumboOutput* output = gumbo_parse(contents.c_str());

    const string title = find_title(output->root);
    if(title == "BAD TITLE BECAUSE NPOS"){
        hasRun = false;
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
    set<Link>::iterator linkIt;
    for(linkIt = page->links.begin(); linkIt != page->links.end(); linkIt++){
        if(linkIt->get_title() == goal_page){
            path[n+1] = linkIt->get_title();
            return true;
        }
    }
	vector<string> *retVec = new vector<string>;
    // if(sql_connection->query_table(page->name, n, retVec)){
	// 	for(int i = 0;n < 3; n++){
	// 		path[n] = retVec->at(i++);
	// 		if(i == (int)retVec->size()) break;
	// 	}
	// 	path[n+1] = goal_page;
    //     cout << "found in sql table\n";
    //     for(size_t i = 0; i < retVec->size(); i++){
    //         cout << retVec->at(i) << " ";
    //     }
    //     cout << '\n';
	// 	return true;
    // }

	delete retVec;
    // query the SQL table to see if we've already found a path for
    // this page. If so, return that path. 

   // don't do this on the last iteration
   Page* nextPage;
   if(n != MAX){
       for(linkIt = page->links.begin(); linkIt != page->links.end() && !sigInt; linkIt++){
            try{
                Link tmp = *linkIt;
                nextPage = page->get_sub_page(tmp);
            }catch(string s){
                sigInt = true;
                delete nextPage;
                return false;
            }

            if(nextPage->name == ""){
                delete nextPage;
                continue;
            }
            
            if(find_hitler_recursive(n+1, nextPage, path)){
                path[n+1] = linkIt->get_title();
                delete nextPage;
                return true;
           }else{
               delete nextPage;
           }
       }
   }
    return false;
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
        // sql_connection->write(path);
    }else{
        path[0] = "NOT POSSIBLE";
        path[1] = current_page->name;
        if(!sigInt){
            //sql_connection->write(path);
        }
    }
    delete [] path;
}

void Finder::sigint(int signal){
    cout << "caught signal " << signal << '\n';
}
