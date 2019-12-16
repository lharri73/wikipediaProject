import numpy
import cv2
import math
from tqdm import trange,tqdm
class tiledImage:
    image_border=20
    small_height=2000
    small_width=2000
    def __init__(self, width, height):
        self.max_num_pixels=self.small_height * self.small_width
        
        num_width = int(width/self.small_width)
        num_height = int(height/self.small_height)
        self.num_images = nearest_square((num_height if num_height>num_width else num_width)**2)

        self._images = []
        for image in range(self.num_images):
            self._images.append(numpy.zeros((self.small_height,self.small_width,3),dtype=numpy.uint8))
        
        self.text = [[] for _ in self._images]
        self.circle = [[] for _ in self._images]
        self.edges = [[] for _ in self._images]
        # raise Exception

    def add_circle(self, center, radius, color):
        assert type(center) == tuple, "center must be given as a tuple"
        assert radius >0, "invalid radius"
        assert type(color) == tuple, "color must be given as a tuple"

        imNum, pos = self.position_to_image(center)
        self.circle[imNum].append([pos, radius, color])
        # cv2.circle(self._images[imNum], pos, radius, color, thickness=-1)
    
    def add_text(self, text, center):
        org = center_text_position(text, center)
        imNum, pos = self.position_to_image(org)
        self.text[imNum].append([text, pos])
        # cv2.putText(text, org, face, scale, color)

    def add_line(self, pos1, pos2):
        imNum1, posA = self.position_to_image(pos1)
        imNum2, posB = self.position_to_image(pos2)
        
        if imNum1==imNum2:
            # the points are in the same image
            self.edges[imNum1].append([posA, posB])
            return

    def write(self):
        for i in trange(len(self._images), desc="Writing images", position=0):
            for edge in tqdm(self.edges[i], desc="edges", position=1, leave=False):
                self._images[i] = cv2.line(self._images[i], edge[0], edge[1], (100,100,100), thickness=2, lineType=cv2.LINE_AA)
            for circle in tqdm(self.circle[i], desc="circles", position=2, leave=False):
                self._images[i] = cv2.circle(self._images[i], circle[0], circle[1], circle[2], thickness=-1, lineType=cv2.LINE_AA)
            for text in tqdm(self.text[i], desc="text", position=3, leave=False):
                self._images[i] = cv2.putText(self._images[i], text[0], text[1], cv2.FONT_HERSHEY_SIMPLEX, 1, (255,255,255), thickness=1, lineType=cv2.LINE_AA)
            
            cv2.imwrite("images/image_{}.png".format(i),self._images[i])#, [cv2.IMWRITE_JPEG_QUALITY, 50])
    
    def position_to_image(self, pos):
        """
        pos: tuple, (x,y)

        return: image_number, new_pos -> new_pos=(x,y)
        """
        assert type(pos)==tuple, "Position must be a tuple"
        side_length = int(math.sqrt(self.num_images))

        if pos[0] > side_length*self.small_width:
            raise ValueError("Input width too large. ({} > {})".format(pos[0], self.small_width*side_length))
        if pos[1] > side_length*self.small_height:
            raise ValueError("Input Height too large. ({} > {})".format(pos[1], self.small_height*side_length))

        x = int(pos[0]%self.small_width)
        y = int(pos[1]%self.small_height)

        x_num = math.floor(pos[0]/self.small_width)
        y_num = math.floor(pos[1]/self.small_height)
        image_number = y_num*side_length + x_num
        if image_number > self.num_images:
            raise Exception("image element '{}' out of bounds for image with size '{}'".format(image_number, self.num_images))
        # image_number = math.floor(pos[0]/self.small_width) + math.floor(pos[1]/self.small_height)*side_length

        return image_number, (x,y)

    def position_relative_to_image(pos, image_num):
        assert type(pos) == tuple, "Position must be a tuple"
        


def nearest_square(number):
    y = 0
    while y**2 <=number:
        y+=1
    return y**2

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
