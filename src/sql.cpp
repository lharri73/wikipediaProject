#include "wikipedia.hpp"

using namespace std;

SQLConnector::SQLConnector(){
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
    stmt->execute("INSERT INTO negative(name) VALUES ('" + name + "')");

}
