#!/usr/bin/env python3
################################################################################
## Date Created  : November 23rd, 2019                                        ##
## Authors       : Landon Harris                                              ##
## Last Modified : November 23rd, 2019                                        ##
## Copyright (c) 2019                                                         ##
################################################################################

from wikipediaLink import Link
from tqdm import tqdm
import uuid
import wget
import os
from bs4 import BeautifulSoup
class Page:
    subUrlRoot = "https://en.wikipedia.org"
    pages_prefix = "pages"
    def __init__(self, name, soup):
        self.name = name
        self.soup = soup
        self.level = level

        self.subPages = {}
        self.links = []

        self.get_links(soup)
        self.get_all_files()

    def addPage(self, name, fileName,soup):
        self.subPages.update({name:{"file":fileName, "soup":soup}})

    def _get_sub_page(self, subLink):
        id = str(uuid.uuid4())
        try:
            subFileName = wget.download(subLink.href, bar=None, out=os.path.join(self.pages_prefix,id + ".webpage"))
        except Exception:
            goodLink = False
        else:
            with open(subFileName, "r") as f:
                soup = BeautifulSoup(f, 'html.parser')
            title = soup.title.string
            index = title.find("- Wikipedia")
            title = title[:index]
            self.addPage(title, subFileName, soup)
            goodLink = True
        return goodLink

    def get_links(self, soup):
        links = soup.find_all('a', recursive=True)
        for link in links:
            try:
                if link.attrs["href"].find("/wiki")==0 and "title" in link.attrs.keys() and len(link.attrs.keys()) == 2:
                    if link.attrs["href"] == "/wiki/Help:Category": 
                        break
                    self.links.append(Link(link))
            except KeyError:
                continue
    
    def get_all_files(self):
        badLinks = []
        for link in tqdm(self.links):
            if(not self._get_sub_page(link)):
                badLinks.append(link)
        
        # remove badLinks from self.links
        self.links = [x for x in self.links if x not in badLinks]
        