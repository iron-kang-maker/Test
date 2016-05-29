# program template for Spaceship
import simplegui
import math
import random

# globals for user interface
WIDTH = 800
HEIGHT = 600
score = 0
lives = 3
time = 0.5
ROT_SPEED = 0.08
FRICTION_F = 0.9
FRICTION_S = 0.96
SHIP_SIZE = 90
SHOOT_SPD = 4
LIVES = 3
SCORE = 0

class ImageInfo:
    def __init__(self, center, size, radius = 0, lifespan = None, animated = False):
        self.center = center
        self.size = size
        self.radius = radius
        if lifespan:
            self.lifespan = lifespan
        else:
            self.lifespan = float('inf')
        self.animated = animated

    def get_center(self):
        return self.center

    def get_size(self):
        return self.size

    def get_radius(self):
        return self.radius

    def get_lifespan(self):
        return self.lifespan

    def get_animated(self):
        return self.animated

    
# art assets created by Kim Lathrop, may be freely re-used in non-commercial projects, please credit Kim
    
# debris images - debris1_brown.png, debris2_brown.png, debris3_brown.png, debris4_brown.png
#                 debris1_blue.png, debris2_blue.png, debris3_blue.png, debris4_blue.png, debris_blend.png
debris_info = ImageInfo([320, 240], [640, 480])
debris_image = simplegui.load_image("http://commondatastorage.googleapis.com/codeskulptor-assets/lathrop/debris2_blue.png")

# nebula images - nebula_brown.png, nebula_blue.png
nebula_info = ImageInfo([400, 300], [800, 600])
nebula_image = simplegui.load_image("http://commondatastorage.googleapis.com/codeskulptor-assets/lathrop/nebula_blue.f2014.png")

# splash image
splash_info = ImageInfo([200, 150], [400, 300])
splash_image = simplegui.load_image("http://commondatastorage.googleapis.com/codeskulptor-assets/lathrop/splash.png")

# ship image
ship_info = ImageInfo([SHIP_SIZE/2, SHIP_SIZE/2], [SHIP_SIZE, SHIP_SIZE], 35)
ship_image = simplegui.load_image("http://commondatastorage.googleapis.com/codeskulptor-assets/lathrop/double_ship.png")

# missile image - shot1.png, shot2.png, shot3.png
missile_info = ImageInfo([5,5], [10, 10], 3, 50)
missile_image = simplegui.load_image("http://commondatastorage.googleapis.com/codeskulptor-assets/lathrop/shot2.png")

# asteroid images - asteroid_blue.png, asteroid_brown.png, asteroid_blend.png
asteroid_info = ImageInfo([45, 45], [90, 90], 40)
asteroid_image = simplegui.load_image("http://commondatastorage.googleapis.com/codeskulptor-assets/lathrop/asteroid_blue.png")

# animated explosion - explosion_orange.png, explosion_blue.png, explosion_blue2.png, explosion_alpha.png
explosion_info = ImageInfo([64, 64], [128, 128], 17, 24, True)
explosion_image = simplegui.load_image("http://commondatastorage.googleapis.com/codeskulptor-assets/lathrop/explosion_alpha.png")

# sound assets purchased from sounddogs.com, please do not redistribute
soundtrack = simplegui.load_sound("http://commondatastorage.googleapis.com/codeskulptor-assets/sounddogs/soundtrack.mp3")
missile_sound = simplegui.load_sound("http://commondatastorage.googleapis.com/codeskulptor-assets/sounddogs/missile.mp3")
missile_sound.set_volume(.5)
ship_thrust_sound = simplegui.load_sound("http://commondatastorage.googleapis.com/codeskulptor-assets/sounddogs/thrust.mp3")
explosion_sound = simplegui.load_sound("http://commondatastorage.googleapis.com/codeskulptor-assets/sounddogs/explosion.mp3")

# helper functions to handle transformations
def angle_to_vector(ang):
    return [math.cos(ang), math.sin(ang)]

def dist(p,q):
    return math.sqrt((p[0] - q[0]) ** 2+(p[1] - q[1]) ** 2)

def random_pos():
    return [random.random() * WIDTH, random.random() * HEIGHT]

def random_rot():
    return (random.random() * .1 - random.random() * .1)

def random_vel():
    return [random.random() * 1 - random.random() * 1,
            random.random() * 1 - random.random() * 1]

def shoot_pos(ship_pos, ship_angle):
    forward = angle_to_vector(ship_angle)
    return [ship_pos[0] + ((SHIP_SIZE/2) * forward[0]), ship_pos[1] + ((SHIP_SIZE/2) * forward[1])]

def shoot_speed(ship_vel, ship_angle):
    forward = angle_to_vector(ship_angle)
    return [ship_vel[0] + forward[0]*SHOOT_SPD,
            ship_vel[1] + forward[1]*SHOOT_SPD]
    
# Ship class
class Ship:
    def __init__(self, pos, vel, angle, image, info):
        self.pos = [pos[0],pos[1]]
        self.vel = [vel[0],vel[1]]
        self.thrust = False
        self.angle = angle
        self.angle_vel = 0
        self.image = image
        self.image_center = info.get_center()
        self.image_size = info.get_size()
        self.radius = info.get_radius()
        
    def draw(self,canvas):
        #canvas.draw_circle(self.pos, self.radius, 1, "White", "White")
        if self.thrust:
            # changes image center X to thrusting ship and plays sound
            self.image_center[0] = SHIP_SIZE * 1.5
            ship_thrust_sound.play()
        else:
            # changes image center X to off ship and stops sound
            self.image_center[0] = SHIP_SIZE * 0.5
            ship_thrust_sound.rewind()
            
        canvas.draw_image(self.image, self.image_center, self.image_size, self.pos, self.image_size, self.angle)

    def update(self):
        #compute acceleration vector based on rotation angle and thrust
        acc_forward = [0, 0]
        if self.thrust:
            acc_forward = angle_to_vector(self.angle)
        
        #update velocity forward vector
        self.vel[0] += acc_forward[0]
        self.vel[1] += acc_forward[1]
        
        #update velocity considering friction
        if self.thrust:
            self.vel[0] *= FRICTION_F
            self.vel[1] *= FRICTION_F
        else:
            self.vel[0] *= FRICTION_S
            self.vel[1] *= FRICTION_S
        
        #update position and angle
        self.pos[0] = (self.pos[0] + self.vel[0]) % WIDTH
        self.pos[1] = (self.pos[1] + self.vel[1]) % HEIGHT
        self.angle += self.angle_vel
        
  
        
    def forward(self):
        self.thrust = not self.thrust

    def rotation_left(self):
        self.angle_vel += -ROT_SPEED
    
    def rotation_right(self):
        self.angle_vel += ROT_SPEED
    
    def shoot(self):
        global a_missile
        forward = angle_to_vector(self.angle)
        a_missile = Sprite(shoot_pos(self.pos, self.angle), shoot_speed(self.vel, self.angle)  , 0, 0, missile_image, missile_info, missile_sound)
    
# Sprite class
class Sprite:
    def __init__(self, pos, vel, ang, ang_vel, image, info, sound = None):
        self.pos = [pos[0],pos[1]]
        self.vel = [vel[0],vel[1]]
        self.angle = ang
        self.angle_vel = ang_vel
        self.image = image
        self.image_center = info.get_center()
        self.image_size = info.get_size()
        self.radius = info.get_radius()
        self.lifespan = info.get_lifespan()
        self.animated = info.get_animated()
        self.age = 0
        if sound:
            sound.rewind()
            sound.play()
   
    def draw(self, canvas):
        canvas.draw_image(self.image, self.image_center, self.image_size, self.pos, self.image_size, self.angle)
    
    def update(self):
        self.pos[0] = (self.pos[0] + self.vel[0]) % WIDTH
        self.pos[1] = (self.pos[1] + self.vel[1]) % HEIGHT
        self.angle += self.angle_vel

           
def draw(canvas):
    global time
    
    # animiate background
    time += 1
    wtime = (time / 4) % WIDTH
    center = debris_info.get_center()
    size = debris_info.get_size()
    canvas.draw_image(nebula_image, nebula_info.get_center(), nebula_info.get_size(), [WIDTH / 2, HEIGHT / 2], [WIDTH, HEIGHT])
    canvas.draw_image(debris_image, center, size, (wtime - WIDTH / 2, HEIGHT / 2), (WIDTH, HEIGHT))
    canvas.draw_image(debris_image, center, size, (wtime + WIDTH / 2, HEIGHT / 2), (WIDTH, HEIGHT))

    # draw ship and sprites
    my_ship.draw(canvas)
    a_rock.draw(canvas)
    a_missile.draw(canvas)
    
    # update ship and sprites
    my_ship.update()
    a_rock.update()
    a_missile.update()
    
    #messages
    canvas.draw_text('Lives: ' + str(LIVES), (WIDTH * .1, HEIGHT * .1), 25, 'White', 'sans-serif')
    canvas.draw_text('Score: ' + str(SCORE), (WIDTH * .75, HEIGHT * .1), 25, 'White', 'sans-serif')

def keyd_handler(key):
    global inputs_d
    
    # manage key down with inputs_d dictionary
    for i in inputs_d: 
        if key == simplegui.KEY_MAP[i]:
            inputs_d[i]()
            

def keyu_handler(key):
    global inputs_u
    
    # manage key up with inputs_u dictionary
    for i in inputs_u: 
        if key == simplegui.KEY_MAP[i]:
            inputs_u[i]()
 

    
# timer handler that spawns a rock    
def rock_spawner():
    global a_rock
    a_rock = Sprite(random_pos(), random_vel(), 0, random_rot(), asteroid_image, asteroid_info)
    
# initialize frame
frame = simplegui.create_frame("Asteroids", WIDTH, HEIGHT)

# initialize ship and two sprites
my_ship = Ship([WIDTH / 2, HEIGHT / 2], [0, 0], 0, ship_image, ship_info)
a_rock = Sprite(random_pos(), random_vel(), 0, random_rot(), asteroid_image, asteroid_info)
a_missile = Sprite(random_pos(), random_vel(), 0, 0, missile_image, missile_info)

inputs_d = {"left": my_ship.rotation_left,
          "right": my_ship.rotation_right,
          "up": my_ship.forward,
          "space": my_ship.shoot}

inputs_u = {"left": my_ship.rotation_right,
          "right": my_ship.rotation_left,
          "up": my_ship.forward}

# register handlers
frame.set_draw_handler(draw)
frame.set_keydown_handler(keyd_handler)
frame.set_keyup_handler(keyu_handler)

timer = simplegui.create_timer(1000.0, rock_spawner)

# get things rolling
timer.start()
frame.start()
