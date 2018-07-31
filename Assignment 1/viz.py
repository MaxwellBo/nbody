import turtle
from dataclasses import dataclass
from random import random

DEFUALT_MASS = 5

def print_body_history(history: list, mass: float):
    r, g, b = random(), random(), random()
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

def print_timestep(timestep: list, masses: list, colours: list):
    turtle.penup()

    for (body, mass, color) in zip(timestep, masses, colours):
        r, g, b = color
        turtle.pencolor(r, g, b)
        turtle.goto(body.x, body.y)
        turtle.dot(mass)

@dataclass
class Body:
    x: float
    y: float
    vx: float
    vy: float

def parse_body(line: str) -> Body:
    x, y, vx, vy = [ float(i) for i in line.split() ]
    return Body(x=x, y=y, vx=vx, vy=vy)

def parse_timestep(block: str) -> list:
    return [ parse_body(i) for i in block.split("\n") ]

with open("out", "r") as f:
    contents = f.read()
    timesteps = [ parse_timestep(i) for i in contents.strip().split("\n\n") ]

    transpose = list(zip(*timesteps))

    turtle.hideturtle()
    turtle.speed(0)

    colours = [(random(), random(), random()) for i in range(len(transpose))]
    masses = [DEFUALT_MASS] * len(transpose)

    # for (n, body_history) in enumerate(transpose):
    #     print_body_history(body_history, masses[n])

    for timestep in timesteps:
        print_timestep(timestep, masses, colours)

    turtle.done()


