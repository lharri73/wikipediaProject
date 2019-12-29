# Wikipedia Mapper
This tool was started as a project that I planned to finish in a couple of 
hours. Needles to say, it ended up taking me a lot longer to finish simply 
because I kept seeing potential to make it better and better. 



## Building From Source

This project was created using the c++98 standard, but with the addition of multithreading, the libraries added in c++11 made this significantly easier. 

NOTE that This project requires the use of [Google's Gumbo parser](https://github.com/google/gumbo-parser). 

1. Install dependencies for the gumbo parser

```bash
    sudo apt install libtool automake
```
1. Install mysql header files

```bash
    sudo apt install libmysqlcppconn-dev
```
1. Clone and set up this repository. The gumbo parser is added to this 
repository as a submodule. 

```bash
    git clone https://github.com/lharri73/wikipediaProject
    cd wikipediaProject
    git submodule init
    git submodule update
```
1. Install the Gumbo parser. Detailed instructions can be found on [the 
Gumbo repository](https://github.com/google/gumbo-parser), but I'll leave the sparknotes version here

```bash
    cd lib/gumbo
    ./autogen.sh
    ./configure
    make
    sudo make install
```
1. Build the wikipedia project. 
```bash
    cd ../../ ## back to the wikipediaProject directory (or wherever you cloned the repo to)
    make
```

## Usage

```bash
    ./bin/wikipedia "goal_page_name" max_search_depth
```

### Flags
- `--single` : Will force the program to only use one thread
- `--sql_ip ###.###.###.###` : Specify the mysql server ip address to use. default: localhost
- `--sql_user ####` : Specify the mysql user to use. default: root
- `sql_pass ###` : Specify the mysql password. default: ""
- `ram #.#` : Specify how much ram to use in Gb. default: 8.0Gb
  - NOTE: this is not a hard maximum, this is a target value. When it reaches 75% of this value, it will not spawn anymore threads until it drops below 75% of this value. 
