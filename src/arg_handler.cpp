#include "Args.hpp"

using namespace std;
args parse_args(int argc, char** argv){
    (void)argc;
    (void)argv;
    args ret;
    // 0 progNmae;
    if(argc < 3){
        printf("usage: %s 'goal_page_name' max_depth\n", argv[0]);
    }

    for(int i = 1; i < argc; i++){
        if(i == 1){
            ret.goal_page = string(argv[i]);
            continue;
        }else if(i == 2){
            ret.max_n = atoi(argv[i]);
            continue;
        }

        if(argv[i] == string("--single"))
            ret.singleThread = true;

		if(argv[i] == string("--debug"))
			ret.singleThread = true;

        if(argv[i] == string("--sql_ip")){
            if(argc == i){
                cout << "must specify ip following '--sql_ip'\n";
                exit(-1);
            }
            ret.mysql_ip = string(argv[++i]);
            ret.useSql = true;
            continue;
        }

        if(argv[i] == string("--sql_user")){
            if(argc == i){
                cout << "must specify user following '--sql_user'\n";
                exit(-1);
            }
            ret.mysql_user = string(argv[++i]);
            ret.useSql = true;
            continue;
        }

        if(argv[i] == string("--sql_pass")){
            if(argc == i){
                cout << "must specify pass following '--sql_pass'\n";
                exit(-1);
            }
            ret.mysql_pass = string(argv[++i]);
            ret.useSql = true;
            continue;
        }

        if(argv[i] == string("--ram")){
            if(argc == i){
                cout << "must specify ram in gb following '--ram'\n";
                exit(-1);
            }
            ret.ramAmount = atof(argv[++i]);
            continue;
        }
    }

    //------------

    if(ret.ramAmount == 0.0){
        cout << "+----------------------------------------------------+\n";
        cout << "| WARNING: Did not specify RAM amount...assuming 8Gb |\n";
        cout << "|         (specify with --ram #inGb)                 |\n";
        cout << "+----------------------------------------------------+\n";
        ret.ramAmount = 8.0;
    }
    return ret;
}

args::args(){
    goal_page = "";
    max_n = 0;
    ramAmount = 0;
    singleThread = false;
    useSql = true;
    mysql_ip = "127.0.0.1";
    mysql_user = "root";
    mysql_pass = "";
    resultsFile = "";
	debug = false;
}
