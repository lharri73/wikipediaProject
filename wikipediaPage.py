#!/usr/bin/env python3
################################################################################
## Date Created  : November 23rd, 2019                                        ##
## Authors       : Landon Harris                                              ##
## Last Modified : November 23rd, 2019                                        ##
## Copyright (c) 2019                                                         ##
################################################################################

class Page:
    def __init__(self, name, soup):
        self.name = name
        self.soup = soup
        self.subPages = {}

    def addPage(self, name, fileName):
        self.subPages.update({name:fileName})

    def get_sub_page(self, subLink):
        url = self.subUrlRoot + subLink
        id = str(uuid.uuid4())
        subFileName = wget.download(url, bar=None, out=os.path.join(self.pages_prefix,id + ".webpage"))
        with open(subFilefilename, "r"):
            soup = BeautifulSoup(f, 'html.parser')
        title = soup.title.string
        index = title.find("- Wikipedia")
        title = title[:index]
        return subFileName, soup, title


    def get_links(self, soup):
        ret_links = []
        links = soup.find_all('a')
        for link in links:
            try:
                if link.attrs["href"].find("/wiki")==0 and "title" in link.attrs.keys() and len(link.attrs.keys()) == 2:
                    if link.attrs["href"] == "/wiki/Help:Category": 
                        break
                    ret_links.append(link)
            except KeyError:
                continue
        return ret_links