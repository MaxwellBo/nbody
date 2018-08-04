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

def print_timestep(timestep: Timestep, turtles: list, masses: list):
    for (t, body, mass) in zip(turtles, timestep.bodies, masses):
        print(body)
        t.goto(body.x, body.y)
        t.pendown()
        t.dot(mass) # size

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
    turtle.tracer(1, 1)
    turtle.speed(0)

    for t in turtles:
        r, g, b = random(), random(), random()
        t.hideturtle()
        t.penup()
        t.pencolor(r, g, b)

    timestep_block = "\n".join(contents.split('\n')[bodies_n + 1:])
    timesteps = [ parse_timestep(i) for i in timestep_block.strip().split("\n\n") ]
    assert(len(timesteps) == timestep_n)

    # transpose = list(zip(*timesteps))
    # for (n, body_history) in enumerate(transpose):
    #     print_body_history(body_history, masses[n], colours[n])

    for timestep in timesteps:
        print_timestep(timestep, turtles, masses)

    turtle.done()


