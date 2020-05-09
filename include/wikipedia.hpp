#pragma once
#include <iostream>
#include <vector>
#include <map>

#include <experimental/filesystem>
#include <glob.h>

#include <sstream>
#include <random>
#include <cstdio>
#include <stdlib.h> 
#include <cstdlib>

#include "gumbo.h"
#include <fstream>
#include <ios>
#include <signal.h>


#include <unistd.h>
#include <mutex>

#include <execinfo.h>

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

class Link{
    public:
        Link(std::string Href, std::string Title);
        const std::string get_href() const;
        const std::string get_title() const;
    protected:
        std::string href;
        std::string title;
};

class args{
    public:
        args();
        std::string goal_page;
        int max_n;
        double ramAmount;
        bool singleThread;
		bool debug;

        bool useSql;
        std::string mysql_ip;
        std::string mysql_user;
        std::string mysql_pass;

        std::string resultsFile;

};

class Page{
    public:
        Page(std::string page_name, GumboOutput *output, std::string filename);
        ~Page();

        Page* get_sub_page(Link &link);

        std::string name;
        std::vector <Link> links;
    protected:
        void get_links();
        GumboOutput *output;
        std::string fileName;

    private:
        bool get_links_recursive(GumboNode *node);
};

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
        sql::ResultSet  *res;

        void write_negative(std::string &name);
        void write_positive(std::string &first, std::string &second, std::string &third, std::string &fourth);
        std::vector<std::string> find_existing(const std::vector<std::string> &vec, const std::string &name, int n);
};

class Finder{
    public:
        Finder(args Args);
        ~Finder();
        Page *get_next_file();
        bool find_hitler_recursive(int n, Page* page, std::string path[]);
        void write_result(std::string* result);

        void begin();

        std::string file_name;
        bool sigInt;
        SQLConnector *sql_connection;
    protected:
        void sigint(int signal);
        bool hasRun;
        int MAX;
        std::string goal_page;
        std::string random_url;
        std::string pages_folder;

        Page* current_page;
};

class uuid{
    public:
        uuid();
        std::string uuid_string() const;
    private:
        std::string uuidStr;
        unsigned int random_char();
        std::string generate_hex(const unsigned int len);
};

const std::string find_title(const GumboNode* root);
void handler(int sig);

void mem_usage(double &vm_usage, double& resident_set);

args parse_args(int argc, char** argv);
void escape_special(std::string &s);
