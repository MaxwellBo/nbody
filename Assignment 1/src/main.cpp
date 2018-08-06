#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <assert.h>
#include <time.h>

// #include "CImg.h"
#include "Body.hpp"
#include "QuadTree.hpp"

const unsigned int MINUTE = 60;
// const unsigned int HOUR = MINUTE * 60;
const double TIME_STEP = 0.001; // millisecond precision
const double T_LAST = 2 * MINUTE;
const double INITIAL_SIMULATION_WIDTH = 2000;
const double OUTPUT_TIME_INTERVAL = 0.01;
const unsigned int OUTPUT_STEP_INTERVAL = OUTPUT_TIME_INTERVAL / TIME_STEP;

const bool ENABLE_BARNES_HUT = true;
const bool BRUTE_FORCE = !ENABLE_BARNES_HUT;

double cpu_time(void) {
    return (double)clock() / (double)CLOCKS_PER_SEC;
}

double calculate_total_energy(const std::vector<Body>& bodies) {
    double acc = 0;

    for (const auto& body: bodies) {
        acc++; // TODO
    }

    return acc;
}

void dump_masses(FILE* file, const std::vector<Body>& bodies) {
    for (const auto& body: bodies) {
        fprintf(file, "%f\n", body.m);
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
void dump_timestep(FILE* file, double timestamp, const std::vector<Body>& bodies) {
    double total_energy = calculate_total_energy(bodies);

    fprintf(file, "%f %f\n", timestamp, total_energy);

    for (const auto& body: bodies) {
        fprintf(file, "%f %f %f %f\n", body.x, body.y, body.vx, body.vy);
    }

    fprintf(file, "\n");
}

void dump_meta_info(FILE* file, const std::vector<Body>& bodies) {
    unsigned int bodies_n = bodies.size();
    unsigned int timesteps = T_LAST / TIME_STEP;
    double output_interval = OUTPUT_TIME_INTERVAL;
    double delta_t = T_LAST;

    fprintf(file, "%d %d %f %f\n", bodies_n, timesteps, output_interval, delta_t); 
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
int main(int argc, char **argv) {
    if (argc == 2) {
        printf("Please supply input and output filenames\n");
        exit(1);
    }

    const std::string &input_filename = argv[1];
    const std::string &output_filename = argv[2];

    std::string line;
    std::ifstream input_fh(input_filename);

    unsigned int total_energy = 0;
    unsigned int bodies_n = 0;
    
    std::vector<double> masses = {};
    std::vector<Body> bodies = {};

    if (input_fh.is_open()) {

        // numBodies
        getline(input_fh, line);
        sscanf(line.c_str(), "%u", &bodies_n);

        while (getline(input_fh, line)) {
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

                if (sscanf(line.c_str(), "%lf %lf %lf %lf", &x, &y, &vx, &vy)) {
                    Body body = Body();
                    body.x = x;
                    body.y = y;
                    body.vx = vx;
                    body.vy = vy;

                    bodies.push_back(body);
                } 
            }
        }

        input_fh.close();
    }

    assert(bodies_n == bodies.size());
    assert(bodies_n == masses.size());

    for(size_t i = 0; i < bodies.size(); i++) {
        bodies[i].m = masses[i];
    }

    double t = 0; // XXX: optimization - double source of truth, update both
    unsigned int step = 0;

    FILE* output_fh = fopen(output_filename.c_str(), "w");

    dump_meta_info(output_fh, bodies);
    dump_masses(output_fh, bodies);
    dump_timestep(output_fh, t, bodies);

    double start = cpu_time();

    while (t < T_LAST - TIME_STEP) {
        QuadTree root;

        if (!BRUTE_FORCE) {
            double root_x = 0;
            double root_y = 0;
            
            root = QuadTree(root_x, root_y, INITIAL_SIMULATION_WIDTH / 2);
            // the quad-tree uses half the width as an implementation detail
            // called "radius". Don't ask me why I picked such a stupid name.

            while (true) {
                if (root.insert_all(bodies)) {
                    break;
                }

                double simulation_radius = 2 * root.radius; 

                fprintf(stderr, "Simulation expanded to %lf\n", simulation_radius);

                root = QuadTree(root_x, root_y, simulation_radius);
            }

        }

        for (auto& body: bodies) {
            body.reset_force();

            if (!BRUTE_FORCE) {
                root.calculate_force(body);
            }
        }

        if (BRUTE_FORCE) {
            for (size_t i = 0; i < bodies.size() - 1; i++) {
                auto& x = bodies[i];

                for (size_t j = i + 1; j < bodies.size(); j++) {
                    auto& y = bodies[j];
                    x.exert_force_unidirectionally(y);
                    y.exert_force_unidirectionally(x);
                }
            }
        }

        for (auto& body: bodies) {
            body.timestep(TIME_STEP);
        }

        t += TIME_STEP;
        step++;

        if (step % OUTPUT_STEP_INTERVAL == 0) {
            dump_timestep(output_fh, t, bodies);
        }
    }

    printf("Total CPU time is %lf\n", cpu_time() - start);
    fclose(output_fh);
    
    return 0;
}
