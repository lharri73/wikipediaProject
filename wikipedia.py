#!/usr/bin/env python3
################################################################################
## Date Created  : November 23rd, 2019                                        ##
## Authors       : Landon Harris                                              ##
## Last Modified : November 23rd, 2019                                        ##
## Copyright (c) 2019                                                         ##
################################################################################

import wget
import os
import uuid
from bs4 import BeautifulSoup

class Finder:
    def __init__(self):
        self.url = "https://en.wikipedia.org/wiki/Special:Random"

    
    def get_next_file(self):
        self.cleanup()
        id = str(uuid.uuid4())
        self.filename = wget.download(self.url, bar=None, out=id)
        with open(self.filename, "r") as f:
            self.soup = BeautifulSoup(f, 'html.parser')
        self.title = self.soup.title.string
        index = self.title.find("- Wikipedia")
        self.title = self.title[:index]
        self.links = []
    
    def get_links(self):
        links = self.soup.find_all('a')
        for link in links:
            try:
                if link.attrs["href"].find("/wiki")==0 and "title" in link.attrs.keys() and len(link.attrs.keys()) == 2:
                    if link.attrs["href"] == "/wiki/Help:Category": 
                        break
                    self.links.append(link)
            except KeyError:
                continue
        for link in self.links:
            print(link)
    def cleanup(self):
        return 
        try:
            os.remove(self.filename)
        except:
            pass

if __name__ == "__main__":
    finder = Finder()
    finder.get_next_file()
    finder.get_links()
    finder.cleanup()
