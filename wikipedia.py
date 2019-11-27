import wget
import os, glob, sys
import uuid
from bs4 import BeautifulSoup
from lib.wikipediaPage import Page
import copy
import csv

def main():
    if len(sys.argv) < 3:
        print('usage: python {} "goal_page_name" max_depth'.format(sys.argv[0]))
        exit(1)
    
    if len(sys.argv) == 2 and sys.argv[1] == "clean":
        finder.cleanup()
        exit()

    finder = Finder(sys.argv[1], int(sys.argv[2]))
    finder.begin()

class Finder:
    def __init__(self, search_for, max_n):
        self.MAX = max_n-1
        self.goal_page = search_for
        self.url = "https://en.wikipedia.org/wiki/Special:Random"

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
        page = Page(self.title, self.soup, os.path.abspath(self.filename))
        print("Root: {}...".format(page.name), end="")
        return page

    def find_hitler(self, n, page, path):
        
        # we must make a copy of the variable because of python's terrible
        # pass by reference for EVERYTHING
        level = copy.deepcopy(n+1)
        
        # base case
        if page.name == self.goal_page: 
            print("FOUND {}".format(self.goal_page))
            path.insert(0,self.goal_page)
            return True, path

        if n > self.MAX: 
            return False, path
        
        for link in page.links:
            # we go through every link and see if it links to
            # the page we're looking for
            if link.title == self.goal_page:
                path.insert(0,link.title)
                print("FOUND {}".format(self.goal_page))
                return True, path

        if n != self.MAX:
            for link in page.links:
                nextPage = page.get_sub_page(link)
                if nextPage is None:
                    continue

                res, path = self.find_hitler(level, nextPage, path)
                if res:
                    path.insert(0,link.title)
                    return True, path

        return False, path

    def cleanup(self):
        files = glob.glob("pages/*")
        print("cleaning {} files".format(len(files)))
        for file in files:
            os.remove(file)

    def write_result(self, results):
        #TODO: make this thread-safe
        with open(self.resultsFile, "a", newline='') as f:
            writer = csv.writer(f)
            writer.writerow(results)

    def begin(self):
        while True:
            self.cleanup()
            page = self.get_next_file()
            if page is None:
                continue

            res, path = self.find_hitler(0,page, [])

            if res:
                path.insert(0,page.name)
                self.write_result(path)
                print(path)
            else:
                print("NOT POSSIBLE")
                self.write_result(["NOT POSSIBLE", page.name])
                
if __name__ == "__main__":
    main()
