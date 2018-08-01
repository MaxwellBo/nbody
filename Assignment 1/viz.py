import turtle
from dataclasses import dataclass
from random import random

@dataclass
class Body:
    x: float
    y: float
    vx: float
    vy: float

@dataclass
class Timestep:
    timestamp: float
    total_energy: float
    bodies: list

def print_body_history(history: list, mass: float, colour: tuple):
    r, g, b = colour
    turtle.pencolor(r, g, b)

    # snap to start
    turtle.penup()
    turtle.goto(history[0].x, history[0].y)

    # start the trace
    turtle.pendown()

    for step in history:
        print(step)
        turtle.goto(step.x, step.y)
        turtle.dot(mass)

    # end the trace
    turtle.penup()
    print()

def print_timestep(timestep: Timestep, masses: list, colours: list):
    turtle.penup()

    for (body, mass, colour) in zip(timestep.bodies, masses, colours):
        print(body)
        r, g, b = colour
        turtle.pencolor(r, g, b)
        turtle.goto(body.x, body.y)
        turtle.dot(mass) # size

def parse_body(line: str) -> Body:
    x, y, vx, vy = [ float(i) for i in line.split() ]
    return Body(x=x, y=y, vx=vx, vy=vy)

def parse_timestep(block: str) -> Timestep:
    timestamp, total_energy = [ float(i) for i in block.split("\n")[0].split() ] 
    bodies = [ parse_body(i) for i in block.split("\n")[1:] ]

    return Timestep(timestamp=timestamp, total_energy=total_energy, bodies=bodies)

with open("out", "r") as f:
    contents = f.read()

    bodies, timestep_n, interval, delta_t = contents.split('\n')[0].split()
    bodies, timestep_n, interval, delta_t = int(bodies), int(timestep_n), float(interval), float(delta_t)

    masses = [ float(i) for i in contents.split('\n')[1:bodies + 1] ]
    assert(len(masses) == bodies)

    colours = [(random(), random(), random()) for i in range(bodies) ]

    timestep_block = "\n".join(contents.split('\n')[bodies + 1:])
    timesteps = [ parse_timestep(i) for i in timestep_block.strip().split("\n\n") ]
    print(len(timesteps), timestep_n)
    assert(len(timesteps) == timestep_n)

    turtle.hideturtle()
    turtle.speed(0)
    turtle.tracer(1, 1)

    # transpose = list(zip(*timesteps))
    # for (n, body_history) in enumerate(transpose):
    #     print_body_history(body_history, masses[n], colours[n])

    for timestep in timesteps:
        print_timestep(timestep, masses, colours)

    turtle.done()


