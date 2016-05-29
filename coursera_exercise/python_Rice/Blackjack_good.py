# Ni! Mini-project #6 - Blackjack

import simplegui
import random
import math

# load card sprite - 936x384 - source: jfitz.com
CARD_SIZE = (72, 96)
CARD_CENTER = (36, 48)
card_images = simplegui.load_image("http://storage.googleapis.com/codeskulptor-assets/cards_jfitz.png")

CARD_BACK_SIZE = (72, 96)
CARD_BACK_CENTER = (36, 48)
card_back = simplegui.load_image("http://storage.googleapis.com/codeskulptor-assets/card_jfitz_back.png")    

# initialize some useful global variables
in_play = False
outcome = ""
score = 0

# define globals for cards
SUITS = ('C', 'S', 'H', 'D')
RANKS = ('A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K')
VALUES = {'A':1, '2':2, '3':3, '4':4, '5':5, '6':6, '7':7, '8':8, '9':9, 'T':10, 'J':10, 'Q':10, 'K':10}
deck = None
player_hand = None
dealer_hand = None

# Frame constants
WIDTH = 600
HEIGHT = 600

# define card class
class Card:
    """ Represents a single Card """
    global outcome
    
    def __init__(self, suit, rank):
        """ Initialize from given suit and rank """
        # Check that the passed attributes are in range
        if (suit in SUITS) and (rank in RANKS):
            self.suit = suit
            self.rank = rank
            # Random angle for displaying the card
            angle_in_degrees = random.randrange(-15,15)
            angle_in_radians = math.pi * 2 * (angle_in_degrees%360) / 360
            self.rotation = angle_in_radians
            
        else:
            self.suit = None
            self.rank = None
            outcome = "Invalid card: ", suit, rank

    def __str__(self):
        """ Suit+Rank """
        return self.suit + self.rank

    def get_suit(self):
        """ Return suit of card. Suits are defined in the SUITS global tuple """
        return self.suit

    def get_rank(self):
        """ Return rank of card """
        return self.rank

    def draw(self, canvas, pos):
        """ Draw a single card on the canvas at the given postion """
        # Find position of the card in the tiled image of the deck of cards
        # Ranks are columns (width dimension), while SUITS are in individual rows (height dimension)
        card_loc = (CARD_CENTER[0] + CARD_SIZE[0] * RANKS.index(self.rank), 
                    CARD_CENTER[1] + CARD_SIZE[1] * SUITS.index(self.suit))
        # Draw image at specified position in the canvas given by pos
        canvas.draw_image(card_images, card_loc, CARD_SIZE, [pos[0] + CARD_CENTER[0], pos[1] + CARD_CENTER[1]], CARD_SIZE, self.rotation)
        
# define hand class
class Hand:
    """ Represents a Hand of cards for the Blackjack game """
    def __init__(self):
        """ create Hand object """
        self.cards = []
        # This parameter is internal and used for randomizing distances
        # between cards when placing them on the table
        self._card_distances = []

    def __str__(self):
        """ Return a string representation of a hand """
        # Here we use the card str representation and build a list of strings
        # which is then concatenated with a space in between
        return "Hand contains " + " ".join(str(card) for card in self.cards)

    def add_card(self, card, position = None):
        """ add a card object to a hand """
        
        width_offset = random.randrange(CARD_SIZE[0]/2, 4 * CARD_SIZE[0] / 3)
        height_offset = random.randrange(-CARD_SIZE[1]/6, CARD_SIZE[1]/6)
        
        if position is not None:
            self.cards.insert(position, card)
            self._card_distances.insert(position, [0, 0])
        else:
            self.cards.append(card)
            self._card_distances.append([width_offset, height_offset])

    def get_value(self):
        """ Return the value of the hand of cards """
        
        # count aces as 1, if the hand has an ace, then add 10 to hand value if it doesn't bust
        num_aces = 0
        hand_value = 0
        for card in self.cards:
            hand_value += VALUES[card.get_rank()]
            if card.get_rank() == 'A':
                num_aces += 1
        while num_aces > 0 and hand_value <= 11:
            hand_value += 10
            num_aces -= 1
            
        return hand_value        
   
    def draw(self, canvas, pos):
        """ Draw a hand on the canvas, using the draw method for cards """
        
        # Initial position of first card
        position = list(pos)
        index = 0
        for card in self.cards:
            # Update position with the internal offsets for each card for
            # a random offset effect
            position[0] += self._card_distances[index][0]
            position[1] += self._card_distances[index][1]
            index += 1
            # Pass actual drawing to the Card drawing method
            card.draw(canvas, position)
         
# define deck class 
class Deck:
    """ Represents a deck of cards """
    
    def __init__(self):
        """ Initialize the deck as a list from the global tuples of suits and ranks """
        
        self.deck = [Card(suit, rank) for suit in SUITS for rank in RANKS]

    def shuffle(self):
        """ Shuffle the deck of cards """
        random.shuffle(self.deck)

    def deal_card(self):
        """ Deal a card object from the deck """
        return self.deck.pop()
    
    def __str__(self):
        """ return a string representing the deck """
        return "Deck contains " + " ".join(str(card) for card in self.deck)



#define event handlers for buttons
def deal():
    """ Shuffle the deck and deal cards to players """
    global outcome, in_play, deck, player_hand, dealer_hand, score
    
    if in_play:
        score -= 1
        outcome = "Gave up? Thanks 4 the $$$! Hit or stand now?"
    else:
        outcome = "Hit or Stand?"
    
    # Deck of cards
    deck = Deck()
    deck.shuffle()
    # Players' hands
    player_hand = Hand()
    dealer_hand = Hand()
    # Deal cards
    player_hand.add_card(deck.deal_card())
    player_hand.add_card(deck.deal_card())
    dealer_hand.add_card(deck.deal_card())    
    
    in_play = True

def hit():
    """ Hit the deck and get a new card if player has not busted yet """
    global in_play, score, outcome
    
    if not in_play:
        return
    # if the hand is in play, hit the player
    if player_hand.get_value() <= 21:
        player_hand.add_card(deck.deal_card())
   
    # if busted, assign a message to outcome, update in_play and score
    if player_hand.get_value() > 21:
        outcome = "You have busted senorita!"
        # Add the dealers second card
        dealer_second_card = deck.deal_card()
        dealer_hand.add_card(dealer_second_card, 0)
        in_play = False
        score -= 1
       
def stand():
    """ Player stands. Deal the dealer if player has not busted """
    global in_play, outcome, score
    
    # if hand is in play, repeatedly hit dealer until his hand has value 17 or more
    if player_hand.get_value() > 21:
        outcome = "You're busted! Standing won't help! Deal?"""
        return
    elif in_play:
        # Deal second card
        dealer_second_card = deck.deal_card()
        dealer_hand.add_card(dealer_second_card, 0)
        while dealer_hand.get_value() < 17:
            dealer_hand.add_card(deck.deal_card())
        if dealer_hand.get_value() > 21:
            outcome = "Dealer's done gone & busted. You lucky so and so!"""
            score += 1
        else:
            # assign a message to outcome, update in_play and score
            if dealer_hand.get_value() >= player_hand.get_value():
                outcome = "Dealer won! Deal again?"
                score -= 1
            else:
                outcome = "You did it! Ka-ching-ching! Again?"
                score += 1
    else:
        outcome = "Deal again?"
    in_play = False

# draw handler    
def draw(canvas):
    """ Draw hands and messages """
    
    outcome_width = frame.get_canvas_textwidth(outcome, 30)
    game_message = "Blackjack      Wins: "
    game_message_width = frame.get_canvas_textwidth(game_message, 40)
    start_message = (WIDTH - game_message_width) / 3
    canvas.draw_text(game_message, [start_message, 50], 40, "Yellow")
    canvas.draw_text(str(score), [(start_message + game_message_width), 50],
                     40, "lime" if score > 0 else "Red")
    canvas.draw_text(outcome, [(WIDTH - outcome_width) / 2,
                               5*HEIGHT/9], 30, "Yellow")
    canvas.draw_text("Dealer", (30, 150), 40, "Pink")
    canvas.draw_text("Yourself", (30, HEIGHT - 100), 40, "Pink")
    # Draw hand
    player_hand.draw(canvas, [WIDTH / 4, 5 * HEIGHT / 7])
    # When the player is still choosing, the dealer's first card is hidden
    if in_play:
        canvas.draw_image(card_back, CARD_BACK_CENTER, CARD_BACK_SIZE,
                          [3 * WIDTH / 8, HEIGHT / 5], CARD_BACK_SIZE)
        dealer_hand.draw(canvas, [WIDTH / 4, HEIGHT / 7])
    else:
        dealer_hand.draw(canvas, [WIDTH / 4, HEIGHT / 7])


# initialization frame
frame = simplegui.create_frame("Blackjack", WIDTH, HEIGHT)
frame.set_canvas_background("Green")

#create buttons and canvas callback
frame.add_button("Deal", deal, 200)
frame.add_button("Hit",  hit, 200)
frame.add_button("Stand", stand, 200)
frame.set_draw_handler(draw)


# get things rolling
deal()
frame.start()
print "Ni!"


# remember to review the gradic rubric