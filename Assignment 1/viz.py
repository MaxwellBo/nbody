import turtle
import math
from random import random

FULL_SPEED = False

class Body(object):
    def __init__(self, x, y, vx, vy):
        self.x = x
        self.y = y
        self.vx = vx
        self.vy = vy

class Timestep(object):
    def __init__(self, timestamp, total_energy, bodies):
        self.timestamp = timestamp
        self.total_energy = total_energy
        self.bodies = bodies

def print_body_history(history: list, mass: float, colour: tuple):
    r, g, b = colour
    turtle.pencolor(r, g, b)

    # snap to start
    turtle.penup()
    turtle.goto(history[0].x, history[0].y)

    # start the trace
    turtle.pendown()

    for step in history:
        turtle.goto(step.x, step.y)
        turtle.dot(math.log(mass) * 2)

    # end the trace
    turtle.penup()

def print_timestep(timestep: Timestep, turtles: list, masses: list):
    for (t, body, mass) in zip(turtles, timestep.bodies, masses):
        t.goto(body.x, body.y)
        t.pendown()
        t.dot(math.log(mass) * 4) # size

def parse_body(line: str) -> Body:
    x, y, vx, vy = [ float(i) for i in line.split() ]
    return Body(x=x, y=y, vx=vx, vy=vy)

def parse_timestep(block: str) -> Timestep:
    timestamp, total_energy = [ float(i) for i in block.split("\n")[0].split() ] 
    bodies = [ parse_body(i) for i in block.split("\n")[1:] ]

    return Timestep(timestamp=timestamp, total_energy=total_energy, bodies=bodies)

with open("out", "r") as f:
    contents = f.read()

    bodies_n, timestep_n, interval, delta_t = contents.split('\n')[0].split()
    bodies_n, timestep_n, interval, delta_t = int(bodies_n), int(timestep_n), float(interval), float(delta_t)

    masses = [ float(i) for i in contents.split('\n')[1:bodies_n + 1] ]
    assert(len(masses) == bodies_n)

    turtles = [ turtle.Turtle() for i in range(bodies_n) ]

    turtle.hideturtle()

    if FULL_SPEED:
        turtle.tracer(0, 0)

    turtle.speed(0)

    for t in turtles:
        r, g, b = random(), random(), random()
        t.hideturtle()
        t.penup()
        t.pencolor(r, g, b)

    timestep_block = "\n".join(contents.split('\n')[bodies_n + 1:])
    timesteps = [ parse_timestep(i) for i in timestep_block.strip().split("\n\n") ]
    print((len(timesteps) * interval) - interval, timestep_n)

    # transpose = list(zip(*timesteps))
    # for (n, body_history) in enumerate(transpose):
    #     print_body_history(body_history, masses[n], colours[n])

    for timestep in timesteps:
        print_timestep(timestep, turtles, masses)

    turtle.done()


