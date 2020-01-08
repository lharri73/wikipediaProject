#include "wikipedia.hpp"

using namespace std;

mutex get_driver_mutex;

SQLConnector::SQLConnector(string ip, string user, string pass){
    lock_guard<mutex> guard(get_driver_mutex);
    driver =  get_driver_instance();
    con = driver->connect("tcp://" + ip + ":3306", user, pass);
    stmt = con->createStatement();
    stmt->execute("USE results");
    stmt->execute("CREATE TABLE IF NOT EXISTS positive("
                  "id INT AUTO_INCREMENT PRIMARY KEY, "
                  "first VARCHAR(255), "
                  "second VARCHAR(255), "
                  "third VARCHAR(255), "
                  "fourth VARCHAR(255))");
    stmt->execute("CREATE TABLE IF NOT EXISTS negative("
                  "id INT AUTO_INCREMENT PRIMARY KEY, "
                  "name VARCHAR(255))");
    cout << "connection created\n";

}

SQLConnector::~SQLConnector(){
    delete con;
    delete stmt;
    delete res;
}

void SQLConnector::write(string *result){
    if(result[0] == "NOT POSSIBLE"){
        write_negative(result[1]);
    }else{
        write_positive(result[0], result[1], result[2], result[3]);
    }
}

void SQLConnector::write_negative(string &name){
    lock_guard<mutex> guard(get_driver_mutex);
    escape_special(name);
    // stmt->execute("INSERT INTO negative(name) VALUES ('" + name + "')");
}

void SQLConnector::write_positive(string &first, string &second, string &third, string &fourth){
	
    lock_guard<mutex> guard(get_driver_mutex);
    escape_special(first);
    escape_special(second);
    escape_special(third);
    escape_special(fourth);
    char command[255*4+30];
    sprintf(command, "INSERT INTO positive(first, second, third, fourth) VALUES ('%s', '%s', '%s', '%s')", first.c_str(), second.c_str(), third.c_str(), fourth.c_str());
    // stmt->execute(string(command));
}

bool SQLConnector::query_table(string name, int n){
    
    lock_guard<mutex> guard(get_driver_mutex);
    escape_special(name);
    char command[255*3+30];
    sprintf(command, "SELECT * FROM positive WHERE first = '%s' OR second = '%s' OR third = '%s'", name.c_str(), name.c_str(), name.c_str());
    res = stmt->executeQuery(string(command));

    vector< vector<string>> vec;
    if(res->rowsCount() != 0) {
        while(res->next()){
            vector<string> tmp;
            tmp.push_back(res->getString("first"));
            tmp.push_back(res->getString("second"));
            tmp.push_back(res->getString("third"));
            tmp.push_back(res->getString("fourth"));
            vec.push_back(tmp);
        }
    }

    for(size_t i = 0; i < vec.size(); i++){
        find_existing(vec[i], name, n);
    }
    return false;
    
}

vector<string> SQLConnector::find_existing(const vector<string> &vec, const std::string &name, int n){
    int dist_to_goal, name_index;
    if(vec.size() < 3)
        throw (string)"Bad vector was provided to SQLConnector::find_existing\n";
    for(size_t i = 0; i < vec.size(); i++){
        // First determine the size of the vector
        int path_size;
        if(vec[3] == "" && vec[2] == ""){
            path_size = 1;
        }else if(vec[3] == ""){
            path_size = 2;
        }else{
            path_size = 3;
        }

        // Now we determine the distance to the goal
        switch(path_size){
            case 1:
				// must be [name, goal_page]
                dist_to_goal = 1;
				name_index = 0;
                break;
            case 2:
                if(vec[1] == name){
					// ["garbage", name, goal_page]
                    dist_to_goal = 1;
					name_index = 1;
                }else{
					// [name, "garbage", goal_page]
                    dist_to_goal = 2;
					name_index = 0;
                }
                break;
            case 3:
                if(vec[0] == name){
					// [name, "garbage", "garbage", goal_page]
					dist_to_goal = 3;
					name_index = 0;
				}else if(vec[1] == name){
					// ["garbage", name, "garbage", goal_page]
					dist_to_goal = 2;
					name_index = 1;
				}else{
					// ["garbage", "garbage", name, goal_page]
					dist_to_goal = 1;
					name_index = 2;
				}
				break;
			// no default
        }
    }
	if(dist_to_goal >= 3-n){
		cout << "here\n";
	}

}

void escape_special(string &s){
    for(size_t i = 0; i < s.size(); i++){
        if(s[i] == '\'' || s[i] == '\"' || s[i] == '\?' || s[i] == '\\'){
            s.insert(i, "\\");
            i++;
        }
    }
}
