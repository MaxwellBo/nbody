#!/usr/local/bin/python3
__author__ = "Maxwell Bo (43926871)"

import sys
import math
import statistics
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.gridspec as gridspec

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

    in_filename = sys.argv[1]

    with open(in_filename, "r") as f:
        ###########
        # PARSING #
        ###########
        contents = f.read()

        bodies_n, timestep_n, interval, delta_t = contents.split('\n')[0].split()
        bodies_n, timestep_n, interval, delta_t = int(bodies_n), int(timestep_n), int(interval), float(delta_t)
        print({"input": in_filename, "numBodies": bodies_n, "numTimeSteps": timestep_n, "outputInterval": interval, "deltaT": delta_t})

        time_per_frame = interval * delta_t

        masses = [ float(i) for i in contents.split('\n')[1:bodies_n + 1] ]
        assert(len(masses) == bodies_n)

        timestep_block = "\n".join(contents.split('\n')[bodies_n + 1:])
        timesteps = [ parse_timestep(i) for i in timestep_block.strip().split("\n\n") ]

        ##########
        # LAYOUT #
        ##########
        fig = plt.figure(figsize=(6, 8))
        gs = gridspec.GridSpec(2, 1, height_ratios=[3, 1])

        ax1 = plt.subplot(gs[0])
        ax1.set_title(sys.argv[1])
        ax1.set_xlim(-EDGE, EDGE)
        ax1.set_ylim(-EDGE, EDGE)
        ax1.set(xlabel="x (m)", ylabel="y (m)")

        ax2 = plt.subplot(gs[1])
        ax2.set(xlabel="t (s)", ylabel="E (J)")

        plt.tight_layout()

        ############
        # PLOTTING #
        ############
        sizes = [ math.sqrt((mass / 1e14) / math.pi) * 6 for mass in masses ]
        colors = np.random.rand(bodies_n)
        particles = ax1.scatter(
            [particle.x for particle in timesteps[0].bodies],
            [particle.y for particle in timesteps[0].bodies],
            s=sizes,
            c=colors,
            animated=True
        )

        cursor = ax2.axvline(x = 0, animated=True)
        fps = round(1 / time_per_frame)
        frames = np.arange(0, len(timesteps))
        time = np.true_divide(frames, fps)

        energies = [timestep.total_energy for timestep in timesteps]
        print("Coefficient of variation of energy is", statistics.stdev(energies) / statistics.mean(energies) * 100)

        ax2.plot(time, energies, "bo", markersize=2)

        def init():
            return particles, cursor

        def animate(step):
            timestep = timesteps[step]

            xs = [particle.x for particle in timestep.bodies]
            ys = [particle.y for particle in timestep.bodies]

            xys = list(map(list, zip(*[xs, ys])))

            particles.set_offsets(xys)

            cursor.set_xdata(step / fps)

            return particles, cursor

        print("Constructing an animation with", time_per_frame, "seconds dedicated to each frame")
        ani = animation.FuncAnimation(fig, animate, init_func=init, frames=frames,
                                    interval=time_per_frame * 1000, blit=True)

        if len(sys.argv) == 1 + 2:

            out_filename = sys.argv[2]

            print("Exporting to", out_filename, "at", fps, "FPS")
            ani.save(out_filename, writer=animation.FFMpegWriter(
                fps=fps, 
                metadata={
                    "artist": "Max Bo",
                    "title": "nbody"
                }
            )
            )
            # ani.save('out.gif', writer='imagemagick')
        else:
            print("Animating at", fps, "FPS")
            plt.show()

if __name__ == "__main__":
    main()
