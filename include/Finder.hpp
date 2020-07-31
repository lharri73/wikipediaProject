#pragma once

#include <string>

#include "SQLConnector.hpp"
#include "Args.hpp"
#include "Page.hpp"

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
void handler(int sig);
