import math
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.animation as animation

OUTPUT_TIME_INTERVAL = 1 / 30

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

    fig = plt.figure()
    ax = plt.axes(xlim=(-100, 100), ylim=(-100, 100))
    sizes = [ math.log(mass / 1e14) * 4 for mass in masses ]
    colors = np.random.rand(bodies_n)

    particles = ax.scatter([], [], s=sizes, c=colors)

    def animate(t):
        timestep = timesteps[t]

        ax.scatter(
            [particle.x for particle in timestep.bodies], 
            [particle.y for particle in timestep.bodies],
            s=sizes,
            c=colors
        )

        return particles,

    ani = animation.FuncAnimation(fig, animate, frames=np.arange(0, len(timesteps)),
                                interval=OUTPUT_TIME_INTERVAL * 1000)
    # writer = animation.FFMpegWriter(fps=30)
    # ani.save('out.mp4', writer=writer)
    plt.show()
