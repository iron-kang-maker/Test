# template for "Stopwatch: The Game"
import simplegui
import math
# define global variables
num = 0
width = 300
height = 200
success_num = 0
stop_num = 0
stop_flag = 1
d = 0

# define helper function format that converts time
# in tenths of seconds into formatted string A:BC.D
def format(t):
    global d,num
    a = int(math.floor(t/600))
    if(a==10):
        num = 0
    bc = int(math.floor((t%600)/10))
    d = int(t%10)
    if(bc<10):
        s = str(a)+":"+"0"+str(bc)+"."+str(d)
    else:
        s = str(a)+":"+str(bc)+"."+str(d)
    return s
    
# define event handlers for buttons; "Start", "Stop", "Reset"
def start():
    global stop_flag
    stop_flag=0
    timer.start()
    
def stop():
    global stop_num,d, success_num, stop_flag
    if(stop_flag==0):
        stop_num += 1
        if(d==0):
            success_num+=1
        timer.stop()
    stop_flag=1
    
def reset():
    global num, stop_num, success_num
    stop_num = 0
    success_num = 0
    num = 0
    timer.stop()

# define event handler for timer with 0.1 sec interval
def increase():
    global num
    num += 1
    print num

# define draw handler
def draw(canvas):
    canvas.draw_text(format(num),[width/2-50,height/2+20],48, "white")
    canvas.draw_text(str(success_num)+"/"+str(stop_num),[width-50,30],24, "red")
    
# create frame
frame = simplegui.create_frame("Stopwatch", width, height)

# register event handlers
frame.add_button("Start", start, 100)
frame.add_button("Stop", stop, 100)
frame.add_button("Reset", reset, 100)
frame.set_draw_handler(draw)
timer = simplegui.create_timer(100, increase)

# start frame
frame.start()


# Please remember to review the grading rubric
