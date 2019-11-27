# Wikipedia Network Mapper
Within each wikipedia page, there are many links to other pages, 
creating a web of pages that are interconnected and often have
interesting patterns. This project focuses on the idea that one can
reach a particularly common wikipedia page from any other wikipedia page
and only clicking on a set number of links. 

The first iteration of this ``game'', for example, was based on the idea
that one could reach the "Adolf Hitler" page in just 3 clicks from a random
page on wikipedia. A variation is reaching the "Jesus Christ" page in
5 clicks from a random page (an interesting difference in itself). Because
these two are linked to so many other events and places in history, it is 
not difficult to reach them in a set number of clicks. 

## Finding Hitler
This project takes a semi-recursive approach to this so-called "Hitler Problem",
starting at Wikipedia's "Random Article" page. It looks at all of the links on 
this page to see if they match the target page title, if not, it starts with the first link
and repeats the process of searching the links and then clicking on the first link. 
This program will only go to the max depth specified before failing and going up one 
level and going back up a page. 

## Limitations
Because of the approach this program takes in solving the Hitler problem, 
if the target page can be reached through the first link AND the second link of 
the first page, for example, only the path through the first page would be 
returned because we start from the top of the page. This could be expanded to 
exhaust all links on all pages before failing, but would take significantly 
more time to execute and receive results from. 
