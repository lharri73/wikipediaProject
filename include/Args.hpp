#pragma once

#include <string>
#include <iostream>

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

args parse_args(int argc, char** argv);
