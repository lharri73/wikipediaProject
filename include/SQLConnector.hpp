#pragma once

#include <string>
#include "mysql_connection.h"
#include <vector>
#include <mutex>

class SQLConnector{
    public:
        // sql::ResultSet *res;
        SQLConnector(std::string ip, std::string user, std::string pass, std::string goalPage);
        ~SQLConnector();
        void write(std::string *result);
        bool query_table(std::string &name, int max_depth, std::vector<std::string> *retVec);
    protected:
        std::string goal_page;
        sql::Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;
        void *res;

        void write_negative(std::string &name);
        void write_positive(std::string &first, std::string &second, std::string &third, std::string &fourth);
        std::vector<std::string> find_existing(const std::vector<std::string> &vec, const std::string &name, int n);
};
void escape_special(std::string &s);
