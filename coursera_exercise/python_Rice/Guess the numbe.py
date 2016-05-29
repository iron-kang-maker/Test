# template for "Guess the number" mini-project
# input will come from buttons and an input field
# all output for the game will be printed in the console
import simplegui
import random
import math

secret_number = 0
num_range = 100
guess_times = 0

# helper function to start and restart the game
def new_game():
    # initialize global variables used in your code here
    global num_range, secret_number, guess_times
    if(num_range == 100):
        guess_times = int(math.ceil(math.log(100+1)/math.log(2)))
        secret_number = random.randrange(0, 100)
        print "New game. Range is from 0 to 100"
    elif(num_range == 1000):
        guess_times = int(math.ceil(math.log(1000+1)/math.log(2)))
        secret_number = random.randrange(0, 1000)
        print "New game. Range is from 0 to 1000"
    else:
        print "Invaild range!!"
        return
    print "Number of remaining guesses is ", guess_times
    print " "

# define event handlers for control panel
def range100():
    # button that changes the range to [0,100) and starts a new game 
    global num_range
    num_range = 100 
    new_game()

def range1000():
    # button that changes the range to [0,1000) and starts a new game     
    global num_range
    num_range = 1000
    new_game()
    
def input_guess(guess):
    # main game logic goes here
    global secret_number,guess_times
    guess_number = int(guess)
    guess_times -= 1
    
    print "Guess was ", guess_number
    print "Number of remaining guesses is ", guess_times
    if(secret_number > guess_number):
        print "Higher!"
    elif(secret_number < guess_number):
        print "Lower!"
    else:
        print "Correct!"
        print " "
        new_game()
        
    if(guess_times == 0):
        print "these guesses are exhausted, please restart game!"
        print " "
        new_game()
                
    print " "

    
# create frame
frame = simplegui.create_frame("guess number", 200, 200)
# register event handlers for control elements and start frame
frame.add_input("Guess", input_guess, 100)
frame.add_button("Range is [0, 100)", range100, 100)
frame.add_button("Range is [0, 1000)", range1000, 100)
frame.start()

# call new_game 
new_game()


# always remember to check your completed program against the grading rubric
