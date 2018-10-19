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
    BODIES = [4, 16, 64, 256, 1024, 4096]

    cpus_per_task = {
        "label": "CPUs per task",
        "field": "cpusPerTask"
    }

    nodes = {
        "label": "nodes",
        "field": "nodes"
    }

    tasks_per_cpu = {
        "label": "tasks per node",
        "field": "tasksPerNode"
    }

    if len(sys.argv) not in [1 + 1]:
        print("input")

    in_filename = sys.argv[1]

    with open(in_filename, "r") as f:
        data = json.loads(f.read())
        valid_data = [ i for i in data if "time" in i ]

        for bodies in BODIES:
            for (x, y) in [
                (cpus_per_task, nodes),
                (cpus_per_task, tasks_per_cpu),
                (nodes, tasks_per_cpu)
            ]:
                for elide_8_tpn in [True, False]:
                    display_figure(
                        bodies=bodies, 
                        data=[i for i in valid_data if i["bodies"] == bodies ], 
                        x=x, 
                        y=y,
                        elide_8_tpn=elide_8_tpn
                    )



def display_figure(bodies, data, x, y, elide_8_tpn):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    if elide_8_tpn:
        data = [ i for i in data if i["tasksPerNode"] != 8 ]

    for c, m, d, label in [
        ('b', 'o', [ i for i in data if not i["enable_barnes_hut"]], "No Barnes-Hut"), 
        ('r', '^', [ i for i in data if i["enable_barnes_hut"]], "Barnes-Hut")
    ]:
        xs = [ i[x["field"]] for i in d ]
        ys = [ i[y["field"]] for i in d ]
        zs = [ i["time"] for i in d ]
        ax.scatter(xs, ys, zs, c=c, marker=m, label=label)

    ax.set_title("{bodies} bodies{elide_8_tpn}".format(
        bodies=bodies, 
        elide_8_tpn=(", --ntasks-per-node=8 elided" if elide_8_tpn else "")
    ))
    ax.set_xlabel('{label} (n)'.format(label=x["label"]))
    ax.set_ylabel('{label} (n)'.format(label=y["label"]))
    ax.set_zlabel('user + sys time (s)')

    ax.legend()

    name = "{bodies}-{xlabel}-{ylabel}{elide_8_tpn}".format(
        bodies=bodies, 
        xlabel=x["field"], 
        ylabel=y["field"],
        elide_8_tpn=("-n8tpn" if elide_8_tpn else "")
    )

    print("\includegraphics[width=2.2cm]{" + name + "}")
    plt.savefig("reports/images/" + name)

if __name__ == "__main__":
    # ms2()
    ms3()
    