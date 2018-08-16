#!/usr/local/bin/python3
__author__ = "Maxwell Bo (43926871)"

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

def main():
    if len(sys.argv) not in [1 + 1, 1 + 2]:
        print("input [output]")
        exit(1)

    with open(sys.argv[1], "r") as f:
        contents = f.read()

        bodies_n, timestep_n, interval, delta_t = contents.split('\n')[0].split()
        bodies_n, timestep_n, interval, delta_t = int(bodies_n), int(timestep_n), float(interval), float(delta_t)
        print({"numBodies": bodies_n, "numTimeSteps": timestep_n, "outputInterval": interval, "deltaT": delta_t})

        masses = [ float(i) for i in contents.split('\n')[1:bodies_n + 1] ]
        assert(len(masses) == bodies_n)

        timestep_block = "\n".join(contents.split('\n')[bodies_n + 1:])
        timesteps = [ parse_timestep(i) for i in timestep_block.strip().split("\n\n") ]

        # import numpy as np
        # import matplotlib.pyplot as plt 
        # from matplotlib import gridspec

        # # generate some data
        # x = np.arange(0, 10, 0.2)
        # y = np.sin(x)

        # # plot it
        # fig = plt.figure(figsize=(8, 6)) 
        # gs = gridspec.GridSpec(1, 2, width_ratios=[3, 1]) 
        # ax0 = plt.subplot(gs[0])
        # ax0.plot(x, y)
        # ax1 = plt.subplot(gs[1])
        # ax1.plot(y, x)

        # plt.tight_layout()
        # plt.savefig('grid_figure.pdf')

        fig = plt.figure(figsize=(8, 12))

        ax1 = fig.add_subplot(211)
        ax2 = fig.add_subplot(212)

        ax1.set_xlim(-EDGE, EDGE)
        ax1.set_ylim(-EDGE, EDGE)

        # ax1.figure.set_size_inches(8, 8, forward=True)

        sizes = [ (math.log(mass / 1e14) + 1) * 10 for mass in masses ]
        colors = np.random.rand(bodies_n)

        cursor = ax2.axvline(x = 0, animated=True)

        particles = ax1.scatter(
            [particle.x for particle in timesteps[0].bodies], 
            [particle.y for particle in timesteps[0].bodies], 
            s=sizes,
            c=colors,
            animated=True
        )

        frames = np.arange(0, len(timesteps))

        ax2.plot(frames, [timestep.total_energy for timestep in timesteps], "bo")

        def init():
            return particles, cursor

        def animate(t):
            timestep = timesteps[t]

            cursor.set_xdata(t)

            xs = [particle.x for particle in timestep.bodies]
            ys = [particle.y for particle in timestep.bodies]

            xys = list(map(list, zip(*[xs, ys])))

            particles.set_offsets(xys)

            return particles, cursor

        ani = animation.FuncAnimation(fig, animate, init_func=init, frames=frames,
                                    interval=interval * 1000, blit=True)

        plt.tight_layout()

        if len(sys.argv) == 1 + 2:
            fps = round(1 / interval)

            print("Exporting at", fps, "FPS")
            ani.save(sys.argv[2], writer=animation.FFMpegWriter(
                fps=fps, 
                metadata={
                    "artist": "Max Bo",
                    "title": "nbody"
                }
            )
            )
            # ani.save('out.gif', writer='imagemagick')
        else:
            plt.show()

if __name__ == "__main__":
    main()