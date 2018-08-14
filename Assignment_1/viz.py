import math
import turtle
from random import random
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

FULL_SPEED = True
TURTLE_ENABLED = True

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

def print_timestep(timestep: Timestep, turtles: list, masses: list):
    for (t, body, mass) in zip(turtles, timestep.bodies, masses):
        t.goto(body.x, body.y)
        t.pendown()
        t.dot(math.log(mass / 1e14) * 4) # size

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

    timestep_block = "\n".join(contents.split('\n')[bodies_n + 1:])
    timesteps = [ parse_timestep(i) for i in timestep_block.strip().split("\n\n") ]
    print((len(timesteps) * interval) - interval, timestep_n)

    if TURTLE_ENABLED:
        turtles = [ turtle.Turtle() for i in range(bodies_n) ]
        
        turtle.setworldcoordinates(llx=-100, lly=-100, urx=100, ury=100)
        turtle.hideturtle()

        if FULL_SPEED:
            turtle.tracer(0, 0)

        turtle.speed(0)

        for t in turtles:
            r, g, b = random(), random(), random()
            t.hideturtle()
            t.penup()
            t.pencolor(r, g, b)
            
        turtle.done()
    else:
        fig = plt.figure()
        ax = plt.axes(xlim=(-100, 100), ylim=(-100, 100))
        particles, = ax.plot([], [], 'bo')

        def animate(t):
            timestep = timesteps[t]

            particles.set_data(
                [particle.x for particle in timestep.bodies], 
                [particle.y for particle in timestep.bodies]
            )

            return particles,

        ani = animation.FuncAnimation(fig, animate, frames=np.arange(0, len(timesteps)),
                                    interval=25, blit=True)
        # ani.save('line.gif', dpi=80, writer='imagemagick')
        plt.show()


