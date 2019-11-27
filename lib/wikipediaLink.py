class Link:
    def __init__(self, link):
        self._link = link
        self.href  = "https://en.wikipedia.org" + link.attrs["href"]
        self.title = link.attrs["title"]
        self.name  = link.string
