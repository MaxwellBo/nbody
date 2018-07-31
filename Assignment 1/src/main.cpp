#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>

// #include "CImg.h"
#include "Body.hpp"
#include "QuadTree.hpp"

const double SIMULATION_WIDTH = 2000;
const double TIME_STEP = 1;

void calculate_total_energy(std::list<Body *> bodies) {
}

/*
numBodies
Mass1
Mass2
...
massN
0.00 totalEnergy
x1 y1 vx1 vy1
..
xN yN vxN vyN
*/
void parse_input(std::list<Body *> bodies) {

}

void dump_masses(std::list<Body *> bodies) {
    for (auto body: bodies) {
        printf("%f\n", body->m);
    }
}

/*
numBodies numTimeSteps outputInterval deltaT
mass1
Mass2
...
massN
timestamp totalEnergy
x1 y1 vx1 vy1
...
xN yN vxN vyN
...
endTime totalEnergy
x1 y1 vx1 vy1
...
xN yN vxN vyN
*/
void dump_timestep(std::list<Body *> bodies) {
    for (auto body: bodies) {
        printf("%f %f %f %f\n", body->x, body->y, body->vx, body->vy);
    }

    printf("\n");
}

int main(int argc, char **argv) {

    if (argc == 1) {
        printf("Please supply an input .csv file\n");
        exit(1);
    }

    std::list<Body *> bodies = {};

    Body foursix = Body();
    foursix.x = 0;
    foursix.y = 100;
    foursix.vx = 10;
    foursix.m = 5;

    Body sixfour = Body();
    sixfour.x = 0;
    sixfour.y = -100;
    sixfour.vx = -10;
    sixfour.m = 5;

    Body three = Body();
    three.x = -200;
    three.m = 10;

    bodies.push_back(&foursix);
    bodies.push_back(&sixfour);
    bodies.push_back(&three);

    double t = 0;

    while (t < 50) {
        QuadTree *root = new_QuadTree(0, 0, SIMULATION_WIDTH / 2);
        // the quad-tree uses half the width as an implementation detail
        // called "radius". Don't ask me why I picked such a stupid name.

        for (auto body: bodies) {
            bool did_insert = insert(root, body);

            if (!did_insert) {
                // printf("Culling out of bound body at (%f, %f)", body->x, body->y);
                bodies.remove(body); // out-of-bounds
            }
        }

        for (auto body: bodies) {
            body->reset_force();
            calculate_force(root, body);
            t += TIME_STEP;
            body->timestep(TIME_STEP);
            dump_timestep(bodies);
        }
    }

    const std::string &filename = argv[1];

    std::string line;
    std::ifstream fh(filename);

    if (fh.is_open()) {
        while (getline(fh, line)) {
            std::stringstream line_stream(line);
            std::string segment;
            std::vector<std::string> seglist;

            while (getline(line_stream, segment, ',')) {
                seglist.push_back(segment);
            }
        }

        fh.close();
    }

    return 0;
}