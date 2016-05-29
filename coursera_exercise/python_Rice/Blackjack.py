# Mini-project #6 - Blackjack

import simplegui
import random

# load card sprite - 936x384 - source: jfitz.com
CARD_SIZE = (72, 96)
CARD_CENTER = (36, 48)
card_images = simplegui.load_image("http://storage.googleapis.com/codeskulptor-assets/cards_jfitz.png")

CARD_BACK_SIZE = (72, 96)
CARD_BACK_CENTER = (36, 48)
card_back = simplegui.load_image("http://storage.googleapis.com/codeskulptor-assets/card_jfitz_back.png")    

logo = simplegui.load_image("https://www.dropbox.com/s/8id7t76zvufn2vx/black_jack_logo.png?dl=1")
LOGO_SIZE = (275, 275)
LOGO_CENTER = (275/2, 275/2)
                    
chip1_image = simplegui.load_image("https://www.dropbox.com/s/wyvvzq0b1oc1g2u/chip_1.png?dl=1")
chip2_image = simplegui.load_image("https://www.dropbox.com/s/bfmtck7f3qdsu88/chip_2.png?dl=1")
chip3_image = simplegui.load_image("https://www.dropbox.com/s/yqwlkfwibvd9njo/chip_3.png?dl=1")
chip4_image = simplegui.load_image("https://www.dropbox.com/s/h373evf1d7pbu10/chip_4.png?dl=1")
chip5_image = simplegui.load_image("https://www.dropbox.com/s/o9cas3ceswbsb2m/chip_5.png?dl=1")
chip6_image = simplegui.load_image("https://www.dropbox.com/s/lt4cj62syt6fs0u/chip_6.png?dl=1")
chip7_image = simplegui.load_image("https://www.dropbox.com/s/n36se1qlpkauvbw/chip_7.png?dl=1")
chip_image_list = [chip1_image, chip2_image, chip3_image, chip4_image, chip5_image, chip6_image, chip7_image]
CHIP_SIZE = (256, 256)
CHIP_CENTER = (128, 128)
# initialize some useful global variables
in_play = False
outcome = ""
score = 0

# define globals for cards
SUITS = ('C', 'S', 'H', 'D')
RANKS = ('A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K')
VALUES = {'A':1, '2':2, '3':3, '4':4, '5':5, '6':6, '7':7, '8':8, '9':9, 'T':10, 'J':10, 'Q':10, 'K':10}
PLAYER_ID = 0
DEALER_ID = 1
player_pos = [50, 450]
dealer_pos = [50, 200]
message = ["New deal?", "Hit or stand?"]

# define card class
class Card:
    def __init__(self, suit, rank):
        if (suit in SUITS) and (rank in RANKS):
            self.suit = suit
            self.rank = rank
        else:
            self.suit = None
            self.rank = None
            print "Invalid card: ", suit, rank

    def __str__(self):
        return self.suit + self.rank

    def get_suit(self):
        return self.suit

    def get_rank(self):
        return self.rank

    def draw(self, canvas, pos):
        card_loc = (CARD_CENTER[0] + CARD_SIZE[0] * RANKS.index(self.rank), 
                    CARD_CENTER[1] + CARD_SIZE[1] * SUITS.index(self.suit))
        canvas.draw_image(card_images, card_loc, CARD_SIZE, [pos[0] + CARD_CENTER[0], pos[1] + CARD_CENTER[1]], CARD_SIZE)
        
# define hand class
class Hand:
    def __init__(self, id):
        self.card_list = []
        self.score = 0
        self.ID = id
        self.points = 7
        
    # return a string representation of a hand
    def __str__(self):
        ret = ""
        for card in self.card_list:
            ret = ret + str(card.get_suit())+str(card.get_rank())+" "	
        return "Hand contains "+ret 
            
    def add_card(self, card):
        self.card_list.append(card)
        
    # count aces as 1, if the hand has an ace, then add 10 to hand value if it doesn't bust
    def get_value(self):
        score = 0
        hasA = False
        for card in self.card_list:
            rank = card.get_rank() 
            score += int(VALUES[rank])
            if rank == 'A': 
                hasA = True
                
        if hasA and ((score+10) <= 21):
            score += 10
            
        return score
    
    # draw a hand on the canvas, use the draw method for cards    
    def draw(self, canvas, pos):
        card_pos = [0,0]
        i=0
        for card in self.card_list:
            card_pos[0] = pos[0]+i*CARD_SIZE[0]
            card_pos[1] = pos[1]
            card.draw(canvas, card_pos)
            i+=1
        
        if self.ID == DEALER_ID and in_play:
            card_loc = (CARD_BACK_CENTER[0], CARD_BACK_CENTER[1])
            canvas.draw_image(card_back, card_loc, CARD_BACK_SIZE, 
                              [pos[0] + CARD_BACK_CENTER[0], 
                               pos[1] + CARD_BACK_CENTER[1]], 
                              CARD_BACK_SIZE)
    def clear_list(self):
        for i in range(len(self.card_list)):
            self.card_list.pop(0)
        
# define deck class 
class Deck:
    # create a Deck object
    def __init__(self):
        self.card_list = []
        for suit in SUITS:
            for rank in RANKS:
                self.card_list.append(Card(suit, rank))
    # shuffle the deck 
    def shuffle(self):
        random.shuffle(self.card_list)
    # deal a card object from the deck
    def deal_card(self):
        return self.card_list.pop(-1)	
    
    # return a string representing the deck 
    def __str__(self):
        ret = ""
        for card in self.card_list:
            ret = ret + str(card.get_suit())+str(card.get_rank())+" "	
        return "Hand contains "+ret 

player = Hand(PLAYER_ID)
dealer = Hand(DEALER_ID)
deck = Deck()

#define event handlers for buttons
def deal():
    global outcome, in_play, deck, player, dealer
    if player.points == 14 or dealer.points == 14:
        player.points = dealer.points = 7
         
    if in_play == True:
        player.points -= 1
        dealer.points += 1   
    
    player.clear_list()# = Hand(PLAYER_ID)
    dealer.clear_list()# = Hand(DEALER_ID)
    deck = Deck()
    deck.shuffle()
    print deck
    for i in range(2):
        player.add_card(deck.deal_card())
        dealer.add_card(deck.deal_card())
    
    print "player:",player  
    print "dealer:",dealer
    print deck
    in_play = True

def hit():
    global in_play, outcome
    if not in_play:
        return
    if player.get_value() < 21:
        player.add_card(deck.deal_card())
        
    if(player.get_value()>21):
        outcome = "You went bust and lose!" 
        in_play = False
        player.points -= 1
        dealer.points += 1
    print deck
    print "player:",player, "scores: ",player.get_value()
           
def stand():
    global in_play, outcome
    if not in_play:
        return
    deal_score = 0
    while dealer.get_value()<17:
        dealer.add_card(deck.deal_card())
   
    deal_score = dealer.get_value()  
    print "dealer:",deal_score,"palyer:",player.get_value()
    if deal_score > 21 or player.get_value()>deal_score:
        outcome = "You win!"
        player.points += 1
        dealer.points -= 1
    else:
        outcome = "You lose!"
        player.points -= 1
        dealer.points += 1
    in_play = False
    print "player points:", player.points, "dealer points:",dealer.points

# draw handler    
def draw(canvas):
    global outcome
    if player.points == 14: 
        canvas.draw_text("GAME OVER!",[300-250, 300+30], 80, "Black", "sans-serif")
        canvas.draw_text("You win!",[300-150, 300+100], 80, "Red", "sans-serif")
        return
    elif dealer.points == 14:
        canvas.draw_text("GAME OVER!",[300-250, 300+30], 80, "Black", "sans-serif")
        canvas.draw_text("You lose!",[300-150, 300+100], 80, "Red", "sans-serif")
        return
    # test to make sure that card.draw works, replace with your code below
    if in_play == False:
        canvas.draw_text(outcome,[220, 380], 30, "Red", "sans-serif")
    canvas.draw_image(logo, [275/2, 275/2], LOGO_SIZE, [300, 50], CARD_SIZE)
    
    canvas.draw_text("Dealer scores:",[50, 180], 30, "Black", "sans-serif")
    canvas.draw_text(str(dealer.points),[270, 180], 30, "Black", "sans-serif")
    canvas.draw_text("Player scores:",[50, 430], 30, "Red", "sans-serif")
    canvas.draw_text(str(player.points),[270, 430], 30, "Red", "sans-serif")
    canvas.draw_text("BLACK",[30, 100], 80, "Black", "monospace")
    canvas.draw_text("JACK",[335, 100], 80, "White", "monospace")
    canvas.draw_text(message[in_play],[50, 380], 30, "Black", "sans-serif")
    
    player.draw(canvas, player_pos)
    dealer.draw(canvas, dealer_pos)
    
    if player.points > 0:
        canvas.draw_image(chip_image_list[(player.points-1)%7], [128, 128], CHIP_SIZE, [420, 500], [100,100])
        if player.points > 7:
            canvas.draw_image(chip_image_list[6], [128, 128], CHIP_SIZE, [520, 500], [100,100])
        
    if dealer.points > 0:
        canvas.draw_image(chip_image_list[(dealer.points-1)%7], [128, 128], CHIP_SIZE, [420, 250], [100,100])
        if dealer.points > 7:
            canvas.draw_image(chip_image_list[6], [128, 128], CHIP_SIZE, [520, 250], [100,100])
    

# initialization frame
frame = simplegui.create_frame("Blackjack", 600, 600)
frame.set_canvas_background("Green")

#create buttons and canvas callback
frame.add_button("Deal", deal, 200)
frame.add_button("Hit",  hit, 200)
frame.add_button("Stand", stand, 200)
frame.set_draw_handler(draw)

# get things rolling
deal()
frame.start()

