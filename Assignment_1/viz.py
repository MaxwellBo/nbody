import sys
import math
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

EDGE = 125
SHOULD_EXPORT = False

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

with open(sys.argv[1], "r") as f:
    contents = f.read()

    bodies_n, timestep_n, interval, delta_t = contents.split('\n')[0].split()
    bodies_n, timestep_n, interval, delta_t = int(bodies_n), int(timestep_n), float(interval), float(delta_t)
    print({"numBodies": bodies_n, "numTimeSteps": timestep_n, "outputInterval": interval, "deltaT": delta_t})

    masses = [ float(i) for i in contents.split('\n')[1:bodies_n + 1] ]
    assert(len(masses) == bodies_n)

    timestep_block = "\n".join(contents.split('\n')[bodies_n + 1:])
    timesteps = [ parse_timestep(i) for i in timestep_block.strip().split("\n\n") ]

    fig = plt.figure(figsize=(8, 8))
    ax = plt.axes(xlim=(-EDGE, EDGE), ylim=(-EDGE, EDGE))
    sizes = [ (math.log(mass / 1e14) + 1) * 10 for mass in masses ]
    colors = np.random.rand(bodies_n)

    particles = ax.scatter(
        [particle.x for particle in timesteps[0].bodies], 
        [particle.y for particle in timesteps[0].bodies], 
        s=sizes,
        c=colors,
        animated=True
    )

    def init():
        return particles,

    def animate(t):
        timestep = timesteps[t]

        xs = [particle.x for particle in timestep.bodies]
        ys = [particle.y for particle in timestep.bodies]

        xys = list(map(list, zip(*[xs, ys])))

        particles.set_offsets(xys)

        return particles,

    ani = animation.FuncAnimation(fig, animate, init_func=init, frames=np.arange(0, len(timesteps)),
                                interval=interval * 1000, blit=True)


    fps = round(1 / interval)

    if SHOULD_EXPORT:
        print("Exporting at", fps, "FPS")
        ani.save('out.mp4', writer=animation.FFMpegWriter(
            fps=fps, 
            metadata={
                "artist": "Max Bo",
                "title": "nbody"
            }
        )
        )
        ani.save('out.gif', writer='imagemagick')

    plt.show()
