# implementation of card game - Memory

import simplegui
import random

card_num_list = []
open_card_list = []
open_state = 0
isMatch = False
get_num = -1
turns = 0
CARDNUM = 16
IMG_WIDTH = 100
IMG_HEIGHT = 200
CAN_WIDTH = 50
CAN_HEIGHT = 100
image_joe = simplegui.load_image("http://i.imgur.com/dLFTzG3.jpg")
image_scott = simplegui.load_image("http://i.imgur.com/bz0w2ft.jpg")
image_list = [image_joe, image_scott]
image = image_joe

# helper function to initialize globals
def new_game():
    global card_num_list, isMatch, open_state
    global open_card_list, get_num, turns, image
    tmp_list1 = [i for i in range(8)] 
    tmp_list2 = [i for i in range(8)]
    random.shuffle(tmp_list1)
    random.shuffle(tmp_list2)
    card_num_list = tmp_list1 + tmp_list2
    get_num = -1
    open_card_list = []
    open_state = 0
    isMatch = False
    turns = 0
    image = image_list[random.randrange(0, 2)]
    label.set_text("Turns = "+str(turns))
    #print card_num_list
    
def open_card(id):
    if(id not in open_card_list):
        open_card_list.append(id)
        return True
    else:
        return False
     
# define event handlers
def mouseclick(pos):
    global open_card_list, open_state, get_num, isMatch, turns
    id = pos[0]/50
    # add game state logic here
    if open_state == 0:
        open_state = 1
        if open_card(id):
            turns += 1
        get_num = card_num_list[id]
    elif open_state == 1:
        open_state = 2
        open_card(id)
        if get_num == card_num_list[id]:
            isMatch = True
        else:
            isMatch = False
        
    else:
        open_state = 1
        if not isMatch:
            open_card_list.pop(-1)
            open_card_list.pop(-1)
        if open_card(id):
            turns += 1
        get_num = card_num_list[id]
  
    label.set_text("Turns = "+str(turns))
                        
# cards are logically 50x100 pixels in size    
def draw(canvas):
    global image
    num = 0
    for i in card_num_list:
        canvas.draw_line([num*50, 0],[num*50, 100], 1, "Red")
        
        if num in open_card_list:
            canvas.draw_text(str(i),[10+50*num, 70], 60, "White", "sans-serif")
        else:
            #canvas.draw_line([num*50+25, 0],[num*50+25, 100], 48, "Green")
            canvas.draw_image(image, 
                [IMG_WIDTH // 2, IMG_HEIGHT // 2], [IMG_WIDTH, IMG_HEIGHT], 
                [CAN_WIDTH // 2 + num*50, CAN_HEIGHT // 2], [CAN_WIDTH, CAN_HEIGHT])    
        num+=1

def restart():
    new_game()

# create frame and add a button and labels
frame = simplegui.create_frame("Memory", 800, 100)
frame.add_button("Reset", new_game)
label = frame.add_label("Turns = 0")

# register event handlers
frame.set_mouseclick_handler(mouseclick)
frame.set_draw_handler(draw)

# get things rolling
new_game()
frame.start()


# Always remember to review the grading rubric