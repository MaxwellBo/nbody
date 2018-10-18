#!/usr/local/bin/python3
__author__ = "Maxwell Bo (43926871)"

import sys
import json
import math
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec

from mpl_toolkits.mplot3d import Axes3D  # noqa: F401 unused import

def ms2():
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
        ax1.set(ylabel="time elapsed (s)", xlabel="bodies (n)")

        ax2 = plt.subplot(gs[1])
        ax2.set_title("60000 timestep running time with Barnes-Hut enabled")
        ax2.set(ylabel="time elapsed (s)", xlabel="bodies (n)")

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

def ms3():
    if len(sys.argv) not in [1 + 1]:
        print("input")

    in_filename = sys.argv[1]

    with open(in_filename, "r") as f:
        data = json.loads(f.read())

        fig = plt.figure()
        ax = fig.add_subplot(111, projection='3d')

        bodies = 1024

        valid_data = [ i for i in data if "time" in i and i["bodies"] == bodies ]

        x = {
            "label": "CPUs per task",
            "field": "cpusPerTask"
        }

        y = {
            "label": "nodes",
            "field": "nodes"
        }

        for c, m, data, label in [
            ('b', 'o', [ i for i in valid_data if not i["enable_barnes_hut"]], "No Barnes-Hut"), 
            ('r', '^', [ i for i in valid_data if i["enable_barnes_hut"]], "Barnes-Hut")
        ]:
            xs = [ i[x["field"]] for i in data ]
            ys = [ i[y["field"]] for i in data ]
            zs = [ i["time"] for i in data ]
            ax.scatter(xs, ys, zs, c=c, marker=m, label=label)

        ax.set_title("{bodies} bodies, 10 timesteps".format(bodies=bodies))
        ax.set_xlabel('{label} (n)'.format(label=x["label"]))
        ax.set_ylabel('{label} (n)'.format(label=y["label"]))
        ax.set_zlabel('user + sys time (s)')

        print()

        ax.legend()

        plt.savefig("reports/images/{bodies}-{xlabel}-{ylabel}".format(bodies=bodies, xlabel=x["field"], ylabel=y["field"]))
        plt.show()

if __name__ == "__main__":
    # ms2()
    ms3()
    