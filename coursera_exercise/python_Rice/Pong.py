# Implementation of classic arcade game Pong

import simplegui
import random
import time

# initialize globals - pos and vel encode vertical info for paddles
WIDTH = 600
HEIGHT = 400       
BALL_RADIUS = 20
PAD_WIDTH = 8
PAD_HEIGHT = 80
HALF_PAD_WIDTH = PAD_WIDTH / 2
HALF_PAD_HEIGHT = PAD_HEIGHT / 2
LEFT = False
RIGHT = True
ball_pos = [WIDTH / 2, HEIGHT / 2]
ball_vel = [0, 0]
paddle1_vel = 0
paddle2_vel = 0
ball_accel = 1
paddle1_pos = [PAD_WIDTH / 2, HEIGHT / 2]
paddle2_pos = [WIDTH - 1 - PAD_WIDTH / 2, HEIGHT / 2]
color = ["Aqua", "Blue", "Fuchsia", "Green", "Lime", "Maroon", "Navy", "Olive", "Orange", "Purple", "Red", "Silver", "Teal", "Yellow", "White"]
ball_color = "White"
score1 = 0
score2 = 0
socore1_color = "white"
socore2_color = "white"
keydownLock = 0

# initialize ball_pos and ball_vel for new bal in middle of table
# if direction is RIGHT, the ball's velocity is upper right, else upper left
def spawn_ball(direction):
    global ball_pos, ball_vel, ball_accel, ball_color # these are vectors stored as lists
    ball_accel = 1
    ball_pos = [WIDTH / 2, HEIGHT / 2]
    if(direction == RIGHT):
        ball_vel[0] = 0.02*random.randrange(120, 240)
    else:
        ball_vel[0] = -0.02*random.randrange(120, 240)
        
    ball_color = color[random.randrange(0, len(color))]
    ball_vel[1] = -0.02*random.randrange(60, 180)
    

# define event handlers
def new_game():
    global paddle1_pos, paddle2_pos, paddle1_vel, paddle2_vel  # these are numbers
    global score1, score2  # these are ints
    score1 = score2 = 0
    spawn_ball(random.randint(0,1))

def draw(canvas):
    global score1, score2, paddle1_pos, paddle2_pos, ball_pos, ball_vel, ball_color
    global paddle1_vel, paddle2_vel, socore1_color, socore2_color 
    # update ball
    ball_pos[0] += ball_vel[0]
    ball_pos[1] += ball_vel[1]  
    
    if(ball_pos[0] <= PAD_WIDTH+BALL_RADIUS-3): 
        if( ball_pos[1] +BALL_RADIUS>= paddle1_pos[1] - HALF_PAD_HEIGHT and ball_pos[1] -BALL_RADIUS<= paddle1_pos[1] + HALF_PAD_HEIGHT):
            ball_vel[0] = -ball_vel[0]+1
        else:
            score2+=1
            spawn_ball(RIGHT)
    elif (ball_pos[0]>= (WIDTH-1-PAD_WIDTH-BALL_RADIUS+3)):
        if( ball_pos[1] +BALL_RADIUS>= paddle2_pos[1] - HALF_PAD_HEIGHT and ball_pos[1] -BALL_RADIUS<= paddle2_pos[1] + HALF_PAD_HEIGHT):
            ball_vel[0] = -ball_vel[0]-1
        else:
            score1+=1
            spawn_ball(LEFT)
        
    if(ball_pos[1] <= BALL_RADIUS or ball_pos[1]>= (HEIGHT-1-BALL_RADIUS)):
       ball_vel[1] *= -1

    
        
    # draw mid line and gutters
    canvas.draw_line([WIDTH / 2, 0],[WIDTH / 2, HEIGHT], 1, "White")
    canvas.draw_line([PAD_WIDTH, 0],[PAD_WIDTH, HEIGHT], 1, "White")
    canvas.draw_line([WIDTH - PAD_WIDTH, 0],[WIDTH - PAD_WIDTH, HEIGHT], 1, "White")
        
    
            
    # draw ball
    canvas.draw_circle(ball_pos, BALL_RADIUS, 2, ball_color, ball_color)
    # update paddle's vertical position, keep paddle on the screen
    if(paddle2_pos[1]-HALF_PAD_HEIGHT >=0):
        paddle2_pos[1] += paddle2_vel
    else:
        paddle2_pos[1] = HALF_PAD_HEIGHT
        paddle2_vel = 0
    
    if(paddle2_pos[1]+HALF_PAD_HEIGHT<=HEIGHT-1):
        paddle2_pos[1] += paddle2_vel
    else:
        paddle2_pos[1] = HEIGHT-1-HALF_PAD_HEIGHT
        paddle2_vel = 0
        
    if(paddle1_pos[1]-HALF_PAD_HEIGHT >=0): 
        paddle1_pos[1] += paddle1_vel        
    else:
        paddle1_pos[1] = HALF_PAD_HEIGHT 
        paddle1_vel = 0
        
    if(paddle1_pos[1]+HALF_PAD_HEIGHT<=HEIGHT-1):
        paddle1_pos[1] += paddle1_vel
    else:
        paddle1_pos[1] = HEIGHT-1-HALF_PAD_HEIGHT
        paddle1_vel = 0
    
    # draw paddles
    canvas.draw_line([paddle1_pos[0], paddle1_pos[1]-HALF_PAD_HEIGHT],[paddle1_pos[0], paddle1_pos[1]+HALF_PAD_HEIGHT], PAD_WIDTH, "White")
    canvas.draw_line([paddle2_pos[0], paddle2_pos[1]-HALF_PAD_HEIGHT],[paddle2_pos[0], paddle2_pos[1]+HALF_PAD_HEIGHT], PAD_WIDTH, "White")
    
    # draw scores
    if(score1 > score2):
        socore1_color = "Red"
        socore2_color = "White"
    elif(score1 < score2):
        socore1_color = "White"
        socore2_color = "Red"
    else:
        socore1_color = "white"
        socore2_color = "White"
        
    canvas.draw_text(str(score1),[WIDTH / 2 -100, 100], 60, socore1_color, "sans-serif")
    canvas.draw_text(str(score2),[WIDTH / 2 +60, 100], 60, socore2_color, "sans-serif")
        
def keydown(key):
    global keydownLock
    keydownLock+=1
    acc = 150*0.02
    global paddle1_vel, paddle2_vel
    if key==simplegui.KEY_MAP["up"]:
        paddle2_vel = -acc
    elif key==simplegui.KEY_MAP["down"]:
        paddle2_vel = acc
    elif key==simplegui.KEY_MAP["w"]:
        paddle1_vel = -acc
    elif key==simplegui.KEY_MAP["s"]:
        paddle1_vel = acc
   
def keyup(key):
    global paddle1_vel, paddle2_vel, keydownLock
    keydownLock-=1
    if (key==simplegui.KEY_MAP["up"] or key==simplegui.KEY_MAP["down"]) and keydownLock==0:
        paddle2_vel = 0
    elif (key==simplegui.KEY_MAP["w"] or key==simplegui.KEY_MAP["s"]) and keydownLock==0:
        paddle1_vel = 0
    
def restart():
    new_game()
    
# create frame
frame = simplegui.create_frame("Pong", WIDTH, HEIGHT)
frame.set_draw_handler(draw)
frame.set_keydown_handler(keydown)
frame.set_keyup_handler(keyup)
frame.add_button("Restart", restart, 100)

# start frame
new_game()
frame.start()
