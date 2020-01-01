# Wikipedia Game Mapper

The Wikipedia Game Mapper is a tool that crawls through a random 
wikipedia page (and all pages that it links to) until it can find 
the goal page that is provided as input. The mapper performs a 
breadth-first search, only going n levels deep, serving as a brute force approach to solving [The Wiki Game](https://en.wikipedia.org/wiki/Wikipedia:Wiki_Game).

This started as a project that I planned to finish in a couple of 
hours. Needles to say, it ended up taking me a lot longer to finish simply 
because I kept seeing potential to make it better and better. 

## Visualization

This generated 56,000 paths from the random article page to the 
Adolf Hitler page with over 82,000 UNIQUE nodes (which is an interesting 
observation in itself). The javascript branch of this project contains
the code used to visualize this set of data in the browser, and is 
adapted from [allnpmviz3d](https://github.com/anvaka/allnpmviz3d)
with a couple of modifications, namely color. 

In this visualization, the color represents the click level, so
the red sphere is the final page (Adolf Hitler), the yellow color 
is the third page, the green is the second page, and the blue is the random page. 

This interactive visualization can be found at [https://lharri73.github.io/wikipediaProject/](https://lharri73.github.io/wikipediaProject/).

## Building From Source

This project was created using the C++98 standard, but with the addition of multithreading, the libraries added in C++11 made this significantly easier. 

NOTE: This project requires the use of [Google's Gumbo parser](https://github.com/google/gumbo-parser). 

1. Install dependencies for the gumbo parser

```bash
    sudo apt install libtool automake
```
2. Install mysql header files

```bash
    sudo apt install libmysqlcppconn-dev
```
3. Clone and set up this repository. The gumbo parser is added to this 
repository as a submodule. 

```bash
    git clone https://github.com/lharri73/wikipediaProject.git
    cd wikipediaProject
    git submodule init
    git submodule update
```
4. Install the Gumbo parser. Detailed instructions can be found on [the 
Gumbo repository](https://github.com/google/gumbo-parser), but I'll leave the spark notes version here

```bash
    cd lib/gumbo
    ./autogen.sh
    ./configure
    make
    sudo make install
```
5. Build the wikipedia project. 
```bash
    cd ../../ ## back to the wikipediaProject directory (or wherever you cloned the repo to)
    make
```

## Usage

```bash
    ./bin/wikipedia "goal_page_name" max_search_depth
```

### Flags
| Flag               | Description                           | Default   |
|--------------------|---------------------------------------|-----------|
| `--single`         | Disable multithreading                | False     |
| `--sql_ip #.#.#.#` | Set the ip of the MySQL server        | 127.0.0.1 |
| `--sql_user XXX`   | Set the user of the MySQL server      | root      |
| `--sql_pass XXX`   | Set the password of the MySQL server  | ""        |
| `--ram #.#`        | Set the amount of ram to use (in GB)  | 8.0       |

- NOTE: `--ram` Does not set a hard limit. The program will not create threads until memory usage is below 50% of this number.

## License 

GPLv2
