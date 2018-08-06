#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <time.h>

// #include "CImg.h"
#include "Body.hpp"
#include "QuadTree.hpp"

const double TIME_STEP = 0.2;
const double T_LAST = 150;
const double INITIAL_SIMULATION_WIDTH = 2000;

const bool BRUTE_FORCE = false;

double cpu_time(void){
    double value;

    value = (double)clock() / (double)CLOCKS_PER_SEC;

    return value;
}

double calculate_total_energy(const std::vector<Body *>& bodies) {
    return 1.0;
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
void parse_input(const std::vector<Body *>& bodies) {

}

void dump_masses(const std::vector<Body *>& bodies) {
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
void dump_timestep(double timestamp, const std::vector<Body *>& bodies) {
    double total_energy = 0;

    printf("%f %f\n", timestamp, total_energy);

    for (auto body: bodies) {
        printf("%f %f %f %f\n", body->x, body->y, body->vx, body->vy);
    }

    printf("\n");
}

void dump_meta_info(const std::vector<Body *>& bodies) {
    unsigned int bodies_n = bodies.size();
    unsigned int timesteps = T_LAST / TIME_STEP;
    double output_interval = TIME_STEP;
    double delta_t = T_LAST;

    printf("%d %d %f %f\n", bodies_n, timesteps, output_interval, delta_t); 
}

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("Please supply an input .csv file\n");
        exit(1);
    }

    const std::string &filename = argv[1];

    std::string line;
    std::ifstream fh(filename);

    unsigned int total_energy = 0;
    unsigned int bodies_n = 0;
    
    std::vector<double> masses = {};
    std::vector<Body *> bodies = {};

    if (fh.is_open()) {

        // numBodies
        getline(fh, line);
        sscanf(line.c_str(), "%u", &bodies_n);

        while (getline(fh, line)) {
            std::stringstream line_stream(line);
            std::string segment;
            std::vector<std::string> segs;

            while (getline(line_stream, segment, ' ')) {
                segs.push_back(segment);
            }

            // a mass
            if (segs.size() == 1) {
                double mass;

                if (sscanf(line.c_str(), "%lf", &mass)) {
                    masses.push_back(mass);
                }
            } 
            // a total energy description
            else if (segs.size() == 2) { 
                double timestamp;
                sscanf(line.c_str(), "%lf %u", &timestamp, &total_energy);
            }
            // a body
            else if (segs.size() == 4) {
                double x;
                double y;
                double vx;
                double vy; 

                double mass;

                if (sscanf(line.c_str(), "%lf %lf %lf %lf", &x, &y, &vx, &vy)) {
                    Body *body = new Body();
                    body->x = x;
                    body->y = y;
                    body->vx = vx;
                    body->vy = vy;

                    bodies.push_back(body);
                } 
            }
        }

        fh.close();
    }

    assert(bodies_n == bodies.size());
    assert(bodies_n == masses.size());

    for(size_t i = 0; i < bodies.size(); i++) {
        bodies[i]->m = masses[i];
    }

    double t = 0;

    dump_meta_info(bodies);
    dump_masses(bodies);
    dump_timestep(t, bodies);

    double start = cpu_time();

    while (t < T_LAST - TIME_STEP) {
        QuadTree* root;

        if (!BRUTE_FORCE) {
            double root_x = 0;
            double root_y = 0;
            
            root = new_QuadTree(root_x, root_y, INITIAL_SIMULATION_WIDTH / 2);
            // the quad-tree uses half the width as an implementation detail
            // called "radius". Don't ask me why I picked such a stupid name.

            while (true) {
                if (insert_all(root, bodies)) {
                    break;
                }

                auto simulation_radius = 2 * root->radius; 
                root = new_QuadTree(root_x, root_y, simulation_radius);
            }

        }

        for (auto& body: bodies) {
            body->reset_force();

            if (!BRUTE_FORCE) {
                calculate_force(root, body);
            }
        }

        if (BRUTE_FORCE) {
            for (size_t i = 0; i < bodies.size() - 1; i++) {
                auto x = bodies[i];

                for (size_t j = i + 1; j < bodies.size(); j++) {
                    auto y = bodies[j];
                    exert_force_unidirectionally(x, y);
                    exert_force_unidirectionally(y, x);
                }
            }
        }

        for (auto body: bodies) {
            body->timestep(TIME_STEP);
        }

        t += TIME_STEP;
        dump_timestep(t, bodies);
    }

    fprintf(stderr, "Total CPU time is %lf\n", cpu_time() - start);

    return 0;
}