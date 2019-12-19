## Wikipedia, A Wealth Of Knowledge
As a student, Wikipedia happens to be one of the most underrated freely available tools. 
It's often the top result for any Google search query, and for good reason: it provides a 
convenient, concise summary of a selected topic as well as a comprehensive analysis of the
topic. Coincidentally, the page always includes an excessive amount of links, but specifically they're
to any pages or concepts mentioned (not even necessarily relevant). As a result, there are a few interesting
[games that can be played](https://en.wikipedia.org/wiki/Wikipedia:Wikipedia_games) on Wikipedia, 
but the one that fascinated me was what is known as the [Wiki Game](https://en.wikipedia.org/wiki/Wikipedia:Wiki_Game).

### The Wiki Game
The motivation for this project came about while I was working on a report and a friend 
mentioned a game you can play on Wikipedia. He explained the game as such:
you start from Wikipedia's [Random Article Link](https://en.wikipedia.org/wiki/Special:Random), and
by only clicking on the links within the body of the article (and subsequent articles), 
you can reach the [Adolf Hitler](https://en.wikipedia.org/wiki/Adolf_Hitler) page within 3 clicks.
At first glance, it's true! (If you're deliberate about the links you select).

After being inundated with writing for the last few hours, I decided to take a break and do 
some research into this subject; I am, after all, an inquisitive computer science student with very little
free time and an ambition to work on pointless projects that will serve no purpose for me later in
life, but hey. Here we are. 

I, interestingly, found no data on this so-called game, so I decided to study it myself (because why not!).
I call it:

## The Wikipedia Network Mapper
This project focuses on the idea that one can reach a particularly common 
wikipedia page from any other wikipedia page. I started this project with
the idea of using recursion, but I ended up implementing a hybrid between
a [Breadth-First Search](https://en.wikipedia.org/wiki/Breadth-first_search)
algorithm and [Depth-First Search](https://en.wikipedia.org/wiki/Depth-first_search) algorithm.

To solve this..."Hitler Problem", the algorithm starts at Wikipedia's "Random Article" page. 
It looks at all of the links on this page to see if they match the target page title, if not,
it starts with the first link and repeats the process of searching the links and then clicking on the first link. 
This program will only go to the max depth specified before failing, causing the algorithm
to start searching the page at the next link of the previous level (in the style of a Breadth-First search).

## Results
What I found from this was honestly quite surprising (but could be misleading and is addressed later).
The raw data from my findings can be found in [this csv file](https://github.com/lharri73/wikipediaProject/blob/master/csvFiles/Hitler.csv).
Here are the highlights:
- xx% of all Wikipedia pages have a 3-click path to hitler
- xxx unique connections to Hitler
- xx% required 3 clicks
- xx% required 2 clicks
- xx% required 1 click

With the raw data, you can start from the first column, go to that Wikipedia page, and find the link in the second column. 
*Note that the csv file records the name of the page as it appears in its title, not the link text (the link "English" might point to the
"English Language" page, so "English Language" will appear in the csv file).* Continue this until you get to the target page, in this case, 
[Adolf Hitler](https://en.wikipedia.org/wiki/Adolf_Hitler). 

I decided to go further. With my experience generating various graphics including all sorts of decision shrubbery, 
I thought the best way to display this data was with a tree. A simple binary tree was not sufficient, so I settled on
a network where each node can have N connections, and the size of the node is proportional to N. I then used a 
[Force Atlas 2](https://github.com/bhargavchippada/forceatlas2) algorithm with X iterations to determine the optimal position
for each node, applied a color scheme, and ended up with this very satisfying image. An interactive version with text can be found (HERE).

## Limitations
Because of the approach this program takes in solving the Hitler problem, there are
a couple of limitations to the algorithm as well as nuances that correlate to 
somewhat of a bias.
- If the target could be reached from multiple paths, only the first path will be returned (which may not be the shortest path)
- It takes SIGNIFICANTLY longer to determine that it is not possible to reach the target page
  - This causes the data to be misleading. Because of this, this algorithm will, by nature, find more true paths than negative results. 
