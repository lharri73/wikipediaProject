#include <iostream>
#include <vector>

#include <filesystem>
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


#include <thread>
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

        std::vector <Page*> pages;
    private:
        bool get_links_recursive(GumboNode *node);
};

class SQLConnector{
    public:
        // sql::ResultSet *res;
        SQLConnector();
        ~SQLConnector();
        void write(std::string *result);
    protected:
        sql::Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;

        void write_negative(std::string &name);
        void write_positive(std::string &first, std::string &second, std::string &third, std::string &fourth);
};

class Finder{
    public:
        Finder(std::string search_for, int max_n, std::string resultsFile);
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
        std::string results_file;

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
