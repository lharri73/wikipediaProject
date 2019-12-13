import networkx as nx
from fa2 import ForceAtlas2
import matplotlib.pyplot as plt
import numpy
from pyvis.network import Network
import csv
import sys
from tqdm import tqdm
import cv2
from sklearn.preprocessing import normalize

def main():
    maker = GraphMaker(sys.argv[1])
    maker.make_graph()

class Node:
    node_radius_base = 20
    textOffset=5
    colors = {0: (244,190,164),
              1: (106,255,89),
              2: (53,179,252),
              3: (52,64,235)}
    def __init__(self, name, clickLevel, numLinks):
        self.name=name
        self.clickLevel = clickLevel
        self.numLinks = numLinks
        self.adjustment_val = None
        
        self.pos = (None, None)
        self.pos_set = False
        self.node_radius = self.node_radius_base*(self.clickLevel+1)

    def __repr__(self):
        return "level {} NODE: '{}' with {} connections".format(self.clickLevel, self.name, self.numLinks)

    def set_position(self, pos, size, dims):
        self.pos = linear_ext(pos, size, dims)
        self.size_cache = size
        self.dims_cache = dims
        self.pos_set = True

    def draw(self,image):
        fontScale=.5
        assert self.pos_set, "Position of node must be set before the node can be drawn."
        radius = int(self.node_radius*self.adjustment_val)
        image.circle.append([(self.pos[0],self.pos[1]), radius, self.colors[self.clickLevel]])
        image.text.append([self.name, center_text_position(self.name, (self.pos[0],self.pos[1] +radius +self.textOffset),fontScale=fontScale),
                    cv2.FONT_HERSHEY_SIMPLEX, fontScale, (255,255,255)])
        # cv2.circle(image, (self.pos[0],self.pos[1]), self.node_radius, self.colors[self.clickLevel], thickness=-1)
        # cv2.putText(image, self.name, center_text_position(self.name, (self.pos[0],self.pos[1] +self.node_radius +self.textOffset),fontScale=fontScale),
        #             cv2.FONT_HERSHEY_SIMPLEX, fontScale, (255,255,255))
        # cv2.putText()

class Image:
    image_border=20
    def __init__(self, width, height):
        self._img = numpy.zeros((height+self.image_border, width+self.image_border,3), dtype=numpy.uint8)
        self.text = []
        self.circle = []
        self.edges = []

    def write(self):
        for edge in tqdm(self.edges, desc="edges"):
            self._img = cv2.line(self._img, edge[0], edge[1], edge[2], edge[3])
        for circle in tqdm(self.circle, desc="circles"):
            self._img = cv2.circle(self._img, circle[0], circle[1], circle[2], thickness=-1)
        for text in tqdm(self.text, desc="text"):
            self._img = cv2.putText(self._img, text[0], text[1], text[2], text[3], text[4])
            
        cv2.imwrite("images/image.png",self._img)#, [cv2.IMWRITE_JPEG_QUALITY, 50])

class GraphMaker:
    def __init__(self, file):
        self.click_list = {
            "root": [],
            "first": {},
            "second": {},
            "third": {}
        }
        self.net = nx.Graph()
        self.file = file
        self.size = 0

        self.nodes = []
        self.nodeMap = {} #{name: number}

    def make_graph(self):
        self._find_sizes()
        self._make_nodes()
        self._add_edges()
        self._gen_positions()

    def _find_sizes(self):
        with open(self.file, "r") as csvFile:
            reader = csv.reader(csvFile)
            for row in reader:
                self.size+=1
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
            self.net.add_node(str(len(self.nodes)), label=str(len(self.nodes)), shape="dot", value=100)
            self.nodeMap.update({name: len(self.nodes)})
            self.nodes.append(Node(name, 0, 0))

        for name, size in self.click_list["first"].items():
            self.net.add_node(str(len(self.nodes)), label=str(len(self.nodes)) + str(size), shape="dot", value=size*100, color="#59ff6a")
            self.nodeMap.update({name: len(self.nodes)})
            self.nodes.append(Node(name, 1, size))

        for name, size in self.click_list["second"].items():
            self.net.add_node(str(len(self.nodes)), label=str(len(self.nodes)) + str(size), shape="dot", value=size*100, color="#fcb335")
            self.nodeMap.update({name: len(self.nodes)})  
            self.nodes.append(Node(name, 2, size))

        for name, size in self.click_list["third"].items():
            self.net.add_node(str(len(self.nodes)), label=str(len(self.nodes)) + str(size), shape="dot", value=size*100, color="#eb4034")
            self.nodeMap.update({name: len(self.nodes)})
            self.nodes.append(Node(name, 3, size))

    def _add_edges(self):
        with open(self.file, "r") as csvFile:
            reader = csv.reader(csvFile)
            for row in reader:
                if len(row) == 2 and row[0] == "NOT POSSIBLE":
                    continue
                elif len(row) == 2:
                    self.net.add_edge(str(self.nodeMap[row[0]]), str(self.nodeMap[row[1]]))
                elif len(row) == 3:
                    self.net.add_edge(str(self.nodeMap[row[0]]), str(self.nodeMap[row[1]]))
                    self.net.add_edge(str(self.nodeMap[row[1]]), str(self.nodeMap[row[2]]))
                elif len(row) == 4:
                    self.net.add_edge(str(self.nodeMap[row[0]]), str(self.nodeMap[row[1]]))
                    self.net.add_edge(str(self.nodeMap[row[1]]), str(self.nodeMap[row[2]]))
                    self.net.add_edge(str(self.nodeMap[row[2]]), str(self.nodeMap[row[3]]))
    
    def _gen_positions(self):
        forceatlas2 = ForceAtlas2(
                                # Behavior alternatives
                                outboundAttractionDistribution=True,  # Dissuade hubs
                                linLogMode=False,  # NOT IMPLEMENTED
                                adjustSizes=False,  # Prevent overlap (NOT IMPLEMENTED)
                                edgeWeightInfluence=1.0,

                                # Performance
                                jitterTolerance=1.0,  # Tolerance
                                barnesHutOptimize=True,
                                barnesHutTheta=1.2,
                                multiThreaded=False,  # NOT IMPLEMENTED

                                # Tuning
                                scalingRatio=5.0,
                                strongGravityMode=False,
                                gravity=10.0,

                                # Log
                                verbose=True)

        positions = forceatlas2.forceatlas2_networkx_layout(self.net, pos=None, iterations=1000)

        self.reverseMap = reverse_dictionary(self.nodeMap)
        
        positions_array = numpy.empty((0,2))

        i = 0
        node_connections = []
        for number, position in positions.items():
            node_connections.append(self.nodes[i].numLinks)
            positions_array = numpy.vstack((positions_array, numpy.array(position)))
            i +=1
        
        node_connections = normalize(numpy.array(node_connections).reshape((1,-1)))
        
        node_connections = node_connections / 2 + 1.0
        
        x_min = numpy.amin(positions_array[:,0])
        x_max = numpy.amax(positions_array[:,0])
        y_min = numpy.amin(positions_array[:,1])
        y_max = numpy.amax(positions_array[:,1])

        x_range = x_max - x_min
        y_range = y_max - y_min

        width=multiply_up(x_range, 10)
        height=multiply_up(y_range,10)
        # self.img = numpy.zeros((height+self.image_border, width+self.image_border,3), dtype=numpy.uint8)
        self.img = Image(width, height)
        
        for node in self.nodes:
            node.adjustment_val = node_connections[0,int(self.nodeMap[node.name])]
            node.set_position(positions[str(self.nodeMap[node.name])], (x_min, x_max, y_min, y_max), (width, height))
            # node.draw(self.img)

        with open(self.file, "r") as csvFile:
            reader = csv.reader(csvFile)
            for row in reader:
                if len(row) == 2 and row[0] == "NOT POSSIBLE":
                    continue
                elif len(row) == 2:
                    self.add_edge(self.nodes[int(self.nodeMap[row[0]])], self.nodes[int(self.nodeMap[row[1]])])
                elif len(row) == 3:
                    self.add_edge(self.nodes[int(self.nodeMap[row[0]])], self.nodes[int(self.nodeMap[row[1]])])
                    self.add_edge(self.nodes[int(self.nodeMap[row[1]])], self.nodes[int(self.nodeMap[row[2]])])
                elif len(row) == 4:
                    self.add_edge(self.nodes[int(self.nodeMap[row[0]])], self.nodes[int(self.nodeMap[row[1]])])
                    self.add_edge(self.nodes[int(self.nodeMap[row[1]])], self.nodes[int(self.nodeMap[row[2]])])
                    self.add_edge(self.nodes[int(self.nodeMap[row[2]])], self.nodes[int(self.nodeMap[row[3]])])
        
        self.img.write()

    def add_edge(self, node1, node2):
        self.img.edges.append([node1.pos, node2.pos, (100,100,100), 1])
        node1.draw(self.img)
        node2.draw(self.img)


#############################################
## helper functions
def reverse_dictionary(my_map):
    rMap = {v: k for k, v in my_map.items()}
    return rMap

def multiply_up(i, factor, power=1):
    i += 10-(i%(10**power))
    i *= factor
    return int(i)

def center_text_position(text, org, fontScale=1,font=cv2.FONT_HERSHEY_SIMPLEX):
    assert type(org)==tuple, "org must be a tuple, got {}".format(type(org))

    # get boundary of this text
    textsize = cv2.getTextSize(text, font, 1, 2)[0]

    # get coords based on boundary
    textX = int(org[0] - textsize[0]/(2*1/fontScale))
    textY = int(org[1] + textsize[1] / (2*1/fontScale))

    # add text centered on image
    ret = (textX, textY)
    return ret

def linear_ext(pos, size, dims):
    #(x_min, x_max, y_min, y_max), (width, height)
    assert len(size)==4, "size parameter must be (x_min, x_max, y_min, y_max)"
    assert len(dims)==2, "dims must be (width, height)"
    x_range = (size[1]-size[0])
    x_factor = dims[0]/x_range

    y_range = (size[3]-size[2])
    y_factor = dims[1]/y_range
    ret = (int((pos[0]-size[0])*x_factor), int((pos[1]-size[2])*y_factor))
    return ret

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: python {} Path_to_csv".format(sys.argv[0]))
        exit(1)
    main()
