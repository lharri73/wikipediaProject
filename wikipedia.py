#!/usr/bin/env python3
################################################################################
## Date Created  : November 22rd, 2019                                        ##
## Authors       : Landon Harris                                              ##
## Last Modified : November 23rd, 2019                                        ##
## Copyright (c) 2019                                                         ##
################################################################################

import wget
import os, glob
import uuid
from bs4 import BeautifulSoup
from wikipediaPage import rootPage

class Finder:
    def __init__(self):
        self.url = "https://en.wikipedia.org/wiki/Special:Random"
        self.subUrlRoot = "https://en.wikipedia.org"
        self.pages_prefix = "pages"
    
    def get_next_file(self):
        self.cleanup()
        id = str(uuid.uuid4())
        self.filename = wget.download(self.url, bar=None, out=os.path.join(self.pages_prefix, id + ".webpage"))
        with open(self.filename, "r") as f:
            self.soup = BeautifulSoup(f, 'html.parser')
        self.title = self.soup.title.string
        index = self.title.find("- Wikipedia")
        self.title = self.title[:index]
        page = rootPage(self.title, self.soup)
        return page

    def cleanup(self):
        #make this remove all webpages including the sub pages
        return 
        try:
            os.remove(self.filename)
        except:
            pass

if __name__ == "__main__":
    finder = Finder()
    page = finder.get_next_file()
    finder.get_links(page.soup)
    finder.cleanup()
