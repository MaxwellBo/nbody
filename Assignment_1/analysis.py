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

        print(data)

        ##########
        # LAYOUT #
        ##########
        fig = plt.figure(figsize=(6, 6))
        plt.boxplot([(entry['numBodies'], entry['time']) for entry in data])

        ############
        # PLOTTING #
        ############

        if len(sys.argv) == 1 + 2:

            out_filename = sys.argv[2]
            # ani.save('out.gif', writer='imagemagick')
        else:
            plt.show()

if __name__ == "__main__":
    main()