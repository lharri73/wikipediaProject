from .wikipediaLink import Link
from tqdm import tqdm
import uuid
import wget
import os
from bs4 import BeautifulSoup
class Page:
    subUrlRoot = "https://en.wikipedia.org"
    pages_prefix = "pages"
    def __init__(self, name, soup, file):
        self.fileName = file
        self.name = name
        self.soup = soup

        self.subPages = {}
        self.links = []

        self.get_links(soup)

    def addPage(self, name, fileName,soup):
        self.subPages.update({name:{"file":fileName, "soup":soup}})

    def get_sub_page(self, subLink):
        id = str(uuid.uuid4())
        try:
            subFileName = wget.download(subLink.href, bar=None, out=os.path.join(self.pages_prefix, id + ".webpage"))
        except Exception as e:
            print(e)
            page = None
        else:
            with open(subFileName, "r") as f:
                soup = BeautifulSoup(f, 'html.parser')
            title = soup.title.string
            index = title.find("- Wikipedia")
            title = title[:index]
            page = Page(title, soup, os.path.abspath(subFileName))
        return page

    def get_links(self, soup):
        links = soup.find_all('a', recursive=True)
        for link in links:
            try:
                if link.attrs["href"].find("/wiki")==0 and "title" in link.attrs.keys() and len(link.attrs.keys()) == 2:
                    if link.attrs["href"] == "/wiki/Help:Category": 
                        break
                    if "Wikipedia:" in link.attrs["href"] or "Special:" in link.attrs["href"] or "Help:" in link.attrs["href"] or "Templage:" in link.attrs["href"]:
                        continue
                    self.links.append(Link(link))
            except KeyError:
                continue

    def __repr__(self):
        return "Page: {}".format(self.name)
        