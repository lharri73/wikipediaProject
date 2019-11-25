#!/usr/bin/env python3
################################################################################
## Date Created  : November 23rd, 2019                                        ##
## Authors       : Landon Harris                                              ##
## Last Modified : November 23rd, 2019                                        ##
## Copyright (c) 2019                                                         ##
################################################################################

class Link:
    def __init__(self, link):
        self._link = link
        self.href  = "https://en.wikipedia.org" + link.attrs["href"]
        self.title = link.attrs["title"]
        self.name  = link.string