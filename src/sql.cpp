#include "wikipedia.hpp"

using namespace std;

void escape_special(string &s);

mutex get_driver_mutex;

SQLConnector::SQLConnector(){
    lock_guard<mutex> guard(get_driver_mutex);
    driver =  get_driver_instance();
    con = driver->connect("tcp://34.68.183.57:3306", "root", "exwmaCrxx8zvPCCl");
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
    stmt->execute("INSERT INTO negative(name) VALUES ('" + name + "')");
}

void SQLConnector::write_positive(string &first, string &second, string &third, string &fourth){
	
    lock_guard<mutex> guard(get_driver_mutex);
    escape_special(first);
    escape_special(second);
    escape_special(third);
    escape_special(fourth);
    char command[255*4+30];
    sprintf(command, "INSERT INTO positive(first, second, third, fourth) VALUES ('%s', '%s', '%s', '%s')", first.c_str(), second.c_str(), third.c_str(), fourth.c_str());
    cout << command << '\n';
    stmt->execute(string(command));
}

void escape_special(string &s){
    for(size_t i = 0; i < s.size(); i++){
        if(s[i] == '\'' || s[i] == '\"' || s[i] == '\?' || s[i] == '\\'){
            s.insert(i, "\\");
            i++;
        }
    }
}
