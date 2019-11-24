#!/usr/bin/env python3
################################################################################
## Date Created  : November 22rd, 2019                                        ##
## Authors       : Landon Harris                                              ##
## Last Modified : November 23rd, 2019                                        ##
## Copyright (c) 2019                                                         ##
################################################################################

import wget
import os, glob, sys
import uuid
from bs4 import BeautifulSoup
from wikipediaPage import Page

class Finder:
    MAX = 3
    def __init__(self):
        self.url = "https://en.wikipedia.org/wiki/Special:Random"
        self.pages_prefix = "pages"
    
    def get_next_file(self):
        id = str(uuid.uuid4())
        self.filename = wget.download(self.url, bar=None, out=os.path.join(self.pages_prefix, id + ".webpage"))
        with open(self.filename, "r") as f:
            self.soup = BeautifulSoup(f, 'html.parser')
        self.title = self.soup.title.string
        index = self.title.find("- Wikipedia")
        self.title = self.title[:index]
        page = Page(self.title, self.soup, 0)
        return page

    def find_hitler(self, n, page, path):
        if page.name == "Adolf Hitler": return True
        if n == max: return False
        self.find_hitler(n+1, page, path)

    def cleanup(self):
        #make this remove all webpages including the sub pages
        files = glob.glob("pages/*.webpage")
        for file in files:
            os.remove(file)

if __name__ == "__main__":
    finder = Finder()
    if len(sys.argv) != 1 and sys.argv[1] == "clean":
        finder.cleanup()
    else:
        page = finder.get_next_file()
