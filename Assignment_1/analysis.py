#!/usr/local/bin/python3
__author__ = "Maxwell Bo (43926871)"

import sys
import json
import math
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec

def main():
    if len(sys.argv) not in [1 + 1, 1 + 2]:
        print("input [output]")
        exit(1)

    in_filename = sys.argv[1]

    with open(in_filename, "r") as f:
        ###########
        # PARSING #
        ###########
        malformed_json = f.read()

        wellformed_json = "[" + malformed_json.replace('\'', '\"') + "]"

        data = json.loads(wellformed_json)

        ##########
        # LAYOUT #
        ##########
        fig = plt.figure(figsize=(6, 8))
        gs = gridspec.GridSpec(2, 1) 

        ax1 = plt.subplot(gs[0])
        ax1.set_title("60000 timestep running time")
        ax1.set(ylabel="time elapsed (s)", xlabel="number of bodies (n)")

        ax2 = plt.subplot(gs[1])
        ax2.set_title("60000 timestep running time with Barnes-Hut enabled")
        ax2.set(ylabel="time elapsed (s)", xlabel="number of bodies (n)")

        plt.tight_layout()

        xs = [(entry['numBodies']) for entry in data if not entry['barnesHut']]
        ys = [(entry['time']) for entry in data if not entry['barnesHut']]
        ax1.plot(xs, ys, "x")

        xs = [(entry['numBodies']) for entry in data if entry['barnesHut']]
        ys = [(entry['time']) for entry in data if entry['barnesHut']]
        ax2.plot(xs, ys, "x")

        ############
        # PLOTTING #
        ############

        if len(sys.argv) == 1 + 2:
            out_filename = sys.argv[2]
        else:
            plt.show()

if __name__ == "__main__":
    main()