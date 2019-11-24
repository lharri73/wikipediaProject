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
import copy
import csv

class Finder:
    MAX = 3
    def __init__(self):
        self.MAX -=1
        # self.url = "https://en.wikipedia.org/wiki/Special:Random"
        # self.url = "https://en.wikipedia.org/wiki/World_War_II"
        self.url = "https://en.wikipedia.org/wiki/United_States"
        self.pages_prefix = "pages"
        self.resultsFile = "results/results.csv"
    
    def get_next_file(self):
        id = str(uuid.uuid4())
        self.filename = wget.download(self.url, bar=None, out=os.path.join(self.pages_prefix, id + ".webpage"))
        with open(self.filename, "r") as f:
            self.soup = BeautifulSoup(f, 'html.parser')
        self.title = self.soup.title.string
        index = self.title.find("- Wikipedia")
        self.title = self.title[:index]
        page = Page(self.title, self.soup, self.filename)
        print("Root: {}".format(page.name))
        return page

    def find_hitler(self, n, page, path):
        level = copy.deepcopy(n+1)
        if page.name == "Adolf Hitler": 
            print("FOUND HILTER")
            path.insert(0,"Adolf Hitler")
            return True, path

        if n > self.MAX: 
            return False, path
        
        for link in page.links:
            if link.title == "Adolf Hitler" or link.title == "Hitler":
                path.insert(0,link.title)
                print("FOUND HITLER")
                return True, path
        if n!= self.MAX:
            for link in page.links:
                nextPage = page.get_sub_page(link)

                if nextPage is None:
                    continue
                res, path = self.find_hitler(level, nextPage, path)
                if res:
                    print("Found: TRUE. Title: {}, n: {}".format(link.title, level))
                    path.insert(0,link.title)
                    return True, path
        return False, path

    def cleanup(self):
        #make this remove all webpages including the sub pages
        files = glob.glob("pages/*")
        print("cleaning {} files".format(len(files)))
        for file in files:
            os.remove(file)

    def write_result(self, results):
        with open(self.resultsFile, "a", newline='') as f:
            writer = csv.writer(f)
            writer.writerow(results)

    def begin(self):
        while True:
            self.cleanup()
            page = self.get_next_file()
            res, path = self.find_hitler(0,page, [])
            if(res):
                path.insert(0,page.name)
                self.write_result(path)
                print(path)
            else:
                self.write_result(["NOT POSSIBLE", page.name])
                

if __name__ == "__main__":
    finder = Finder()
    if len(sys.argv) != 1 and sys.argv[1] == "clean":
        finder.cleanup()
        exit()
    finder.begin()



