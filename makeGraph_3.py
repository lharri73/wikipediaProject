from pyvis.network import Network
import csv
import sys
from tqdm import tqdm


def main():
    maker = GraphMaker(sys.argv[1])
    maker.make_graph()
    maker.show_map()

class GraphMaker:
    def __init__(self, file):
        self.click_list = {
            "root": [],
            "first": {},
            "second": {},
            "third": {}
        }
        self.net = Network(height="100vh", width="100%", bgcolor="#222222", font_color="white")
        self.file = file

    def make_graph(self):
        self._find_sizes()
        self._make_nodes()
        self._add_edges()


    def _find_sizes(self):
        with open(self.file, "r") as csvFile:
            reader = csv.reader(csvFile)
            for row in reader:
                if len(row) == 2 and row[0] == "NOT POSSIBLE":
                    continue
                elif len(row) == 2:
                    self.click_list["root"].append(str(row[0]))
                    try:
                        self.click_list["third"][str(row[1])] = self.click_list["third"][str(row[1])] + 1
                    except KeyError:
                        self.click_list["third"].update({str(row[1]): 1})
                
                elif len(row) == 3:
                    self.click_list["root"].append(str(row[0]))
                    try:
                        self.click_list["second"][str(row[1])] = self.click_list["second"][str(row[1])] + 1
                    except KeyError:
                        self.click_list["second"].update({str(row[1]): 1})

                    try:
                        self.click_list["third"][str(row[2])] = self.click_list["third"][str(row[2])] + 1
                    except KeyError:
                        self.click_list["third"].update({str(row[2]): 1})

                elif len(row) == 4:
                    self.click_list["root"].append(str(row[0]))
                    try:
                        self.click_list["first"][str(row[1])] = self.click_list["first"][str(row[1])] + 1
                    except KeyError:
                        self.click_list["first"].update({str(row[1]): 1})

                    try:
                        self.click_list["second"][str(row[2])] = self.click_list["second"][str(row[2])] + 1
                    except KeyError:
                        self.click_list["second"].update({str(row[2]): 1})

                    try:
                        self.click_list["third"][str(row[3])] = self.click_list["third"][str(row[3])] + 1
                    except KeyError:
                        self.click_list["third"].update({str(row[3]): 1})
                else:
                    print(row)
                    raise Exception("Bad CSV file")
    
    def _make_nodes(self):
        for name in self.click_list["root"]:
            self.net.add_node(name, label=name, shape="dot", value=100)
        for name, size in self.click_list["first"].items():
            self.net.add_node(name, label=name + " " + str(size), shape="dot", value=size*100, color="#59ff6a")
        for name, size in self.click_list["second"].items():
            self.net.add_node(name, label=name + " " + str(size), shape="dot", value=size*100, color="#fcb335")    
        for name, size in self.click_list["third"].items():
            self.net.add_node(name, label=name + " " + str(size), shape="dot", value=size*100, color="#eb4034")

    def _add_edges(self):
        with open(self.file, "r") as csvFile:
            reader = csv.reader(csvFile)
            for row in reader:
                if len(row) == 2 and row[0] == "NOT POSSIBLE":
                    continue
                elif len(row) == 2:
                    self.net.add_edge(row[0], row[1])
                elif len(row) == 3:
                    self.net.add_edge(row[0], row[1])
                    self.net.add_edge(row[1], row[2])
                elif len(row) == 4:
                    self.net.add_edge(row[0], row[1])
                    self.net.add_edge(row[1], row[2])
                    self.net.add_edge(row[2], row[3])

    def show_map(self):
        # self.net.show_buttons()
        self.net.toggle_physics(False)
        self.net.show("jesus.html")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: python {} Path_to_csv".format(sys.argv[0]))
        exit(1)
    main()
