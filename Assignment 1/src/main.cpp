#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

// #include "CImg.h"
#include "Body.hpp"
#include "QuadTree.hpp"

const double TIME_STEP = 1;

void calculate_total_energy(std::vector<Body *> bodies) {
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
void parse_input(std::vector<Body *> bodies) {

}

void dump_masses(std::vector<Body *> bodies) {
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
void dump_timestep(std::vector<Body *> bodies) {
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

    std::vector<Body *> bodies = {};

    Body foursix = Body();
    foursix.x = 0;
    foursix.y = 200;
    foursix.vx = 5;
    foursix.m = 5;

    Body sixfour = Body();
    sixfour.x = 0;
    sixfour.y = -200;
    sixfour.vx = -5;
    sixfour.m = 5;

    Body three = Body();
    three.x = -200;
    three.m = 5;

    bodies.push_back(&foursix);
    bodies.push_back(&sixfour);
    // bodies.push_back(&three);

    double t = 0;
    double simulation_width = 2000;

    while (t < 150) {
        QuadTree *root = new_QuadTree(0, 0, simulation_width / 2);
        // the quad-tree uses half the width as an implementation detail
        // called "radius". Don't ask me why I picked such a stupid name.

        while (true) {
            if (insert_all(root, bodies)) {
                break;
            }

            simulation_width *= 2; 
            root = new_QuadTree(0, 0, simulation_width / 2);
        }

        for (auto body: bodies) {
            body->reset_force();
            // calculate_force(root, body);
        }

        for (size_t i = 0; i < bodies.size() - 1; i++) {
            auto x = bodies[i];

            for (size_t j = i + 1; j < bodies.size(); j++) {
                auto y = bodies[j];
                exert_force_unidirectionally(x, y);
                exert_force_unidirectionally(y, x);
            }
        }

        for (auto body: bodies) {
            body->timestep(TIME_STEP);
        }

        t += TIME_STEP;
        dump_timestep(bodies);
    }

    const std::string &filename = argv[1];

    std::string line;
    std::ifstream fh(filename);

    if (fh.is_open()) {
        while (getline(fh, line)) {
            std::stringstream line_stream(line);
            std::string segment;
            std::vector<std::string> segs;

            while (getline(line_stream, segment, ',')) {
                segs.push_back(segment);
            }
        }

        fh.close();
    }

    return 0;
}