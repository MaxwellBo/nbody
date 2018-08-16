#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <assert.h>
#include <time.h>
#include <algorithm>
#include <limits>

// #include "CImg.h"
#include "Body.hpp"
#include "QuadTree.hpp"
#include "utils.hpp"

const unsigned int LEAP = 0;
const unsigned int FROG = 1;

const bool ENABLE_BARNES_HUT = false;
const bool ENABLE_LEAPFROG = true;
const bool ENABLE_LOGGING = false;

double cpu_time(void) {
    return (double)clock() / (double)CLOCKS_PER_SEC;
}

double maximum_deviation_from_root(const std::vector<Body>& bodies) {
    double lowest_x = std::numeric_limits<double>::max();
    double highest_x = std::numeric_limits<double>::min();
    double lowest_y = std::numeric_limits<double>::max();
    double highest_y = std::numeric_limits<double>::min();

    for (const auto& body: bodies) {
        if (body.x < lowest_x) {
            lowest_x = body.x;
        }
        else if (highest_x < body.x) {
            highest_x = body.x;
        }

        if (body.y < lowest_y) {
            lowest_y = body.y;
        }
        else if (highest_y < body.y) {
            highest_y = body.y;
        }
    }

    return std::max(highest_y - lowest_y, highest_x - lowest_x);
}

double calculate_total_energy(const std::vector<Body>& bodies) {
    double acc = 0;

    for (auto& body : bodies) {
        acc += body.kinetic_energy();
    }

    for (size_t i = 0; i < bodies.size() - 1; i++) {
        auto& x = bodies[i];

        for (size_t j = i + 1; j < bodies.size(); j++) {
            auto& y = bodies[j];

            acc += x.gravitational_potential_energy(y);
            acc += y.gravitational_potential_energy(x);
        }
    }
    
    return acc;
}

void dump_masses(const std::vector<Body>& bodies) {
    for (const auto& body: bodies) {
        fprintf(stdout, "%f\n", body.m);
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
void dump_timestep(double timestamp, const std::vector<Body>& bodies) {
    const double total_energy = calculate_total_energy(bodies);

    fprintf(stdout, "%f %f\n", timestamp, total_energy);

    for (const auto& body: bodies) {
        fprintf(stdout, "%f %f %f %f\n", body.x, body.y, body.vx, body.vy);
    }

    fprintf(stdout, "\n");
}

void dump_meta_info(
    unsigned int num_time_steps,
    double output_interval,
    double delta_t,
    const std::vector<Body>& bodies
) {
    const unsigned int bodies_n = bodies.size();

    fprintf(stdout, "%d %d %f %f\n", bodies_n, num_time_steps, output_interval, delta_t); 
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
std::vector<Body> parse_input_file(std::ifstream& input_fh) {
    // not a Body&; we want to maximise cache locality
    std::vector<Body> bodies = {};
    std::vector<double> masses = {};
    std::string line;

    unsigned int total_energy = 0;
    unsigned int bodies_n = 0;

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

    // we're going to throw bodies_n away here and let the Body vector
    // be the source of truth for how many we have
    assert(bodies_n == bodies.size());
    assert(bodies_n == masses.size());

    for (size_t i = 0; i < bodies.size(); i++) {
        bodies[i].m = masses[i];
        bodies[i].Gm = G * masses[i];
    }

    return bodies;
}

int main(int argc, char **argv) {
    if (argc != 5) {
        fprintf(stderr, "numTimeSteps outputInterval deltaT inputFile\n");
        exit(1);
    }

    const unsigned int num_time_steps = std::stoi(argv[1]);
    const double output_interval = std::stod(argv[2]);
    const double delta_t = std::stod(argv[3]);

    const double timestep = delta_t / num_time_steps;
    const double halfstep = timestep / 2;

    const unsigned int output_step_interval = 
        output_interval * (ENABLE_LEAPFROG ? 2 : 1) / timestep;

    const std::string &input_filename = argv[4];

    std::ifstream input_fh(input_filename);
    FILE *log_fh = fopen("nbody.log", "a");

    std::vector<Body> bodies = parse_input_file(input_fh);

    double t = 0; // XXX: optimization - double source of truth, update both
    // TODO: can we have a function that inlines and updates both of these?
    unsigned int step = 0;

    dump_meta_info(num_time_steps, output_interval, delta_t, bodies);
    dump_masses(bodies);
    dump_timestep(t, bodies);

    fprintf(stderr, "Barnes-Hut enabled: %s\n", ENABLE_BARNES_HUT ? "true" : "false");
    fprintf(stderr, "Leapfrog enabled: %s\n", ENABLE_LEAPFROG ? "true" : "false");

    double start = cpu_time();

    while (t < delta_t) {
        QuadTree root;

        if (ENABLE_BARNES_HUT && step % 2 == FROG) {
            const double root_x = 0;
            const double root_y = 0;
            const double radius = maximum_deviation_from_root(bodies) + 1;
            // the quad-tree uses half the width as an implementation detail
            // called "radius". We're trying to make a QuadTree that encapsulates
            // the most distant body

            root = QuadTree(root_x, root_y, radius);

            assert(root.insert_all(bodies));
        }

        if (!ENABLE_LEAPFROG || step % 2 == FROG) {
            for (auto& body: bodies) {
                body.reset_force();

                if (ENABLE_BARNES_HUT) {
                    root.calculate_force(body);
                }
            }
        }

        if (!ENABLE_BARNES_HUT) {
            for (size_t i = 0; i < bodies.size() - 1; i++) {
                auto& x = bodies[i];

                for (size_t j = i + 1; j < bodies.size(); j++) {
                    auto& y = bodies[j];
                    x.exert_force_bidirectionally(y);
                }
            }
        }

        if (ENABLE_LEAPFROG) {
            for (auto& body: bodies) {
                if (step % 2 == LEAP) {
                    body.leap(timestep);
                }
                else {
                    body.frog(timestep);
                }
            }

            t += halfstep;
            step++;
        } 
        else {
            for (auto& body: bodies) {
                body.euler_integrate(timestep);
            }

            t += timestep;
            step++;
        }

        if (step % output_step_interval == 0) {
            dump_timestep(t, bodies);
        }
    }

    const double cpu_time_elapsed = cpu_time() - start;

    if (ENABLE_LOGGING) {
        fprintf(
            log_fh,
            ",\n{ 'numTimeSteps': %d, 'inputFile': '%s', 'numBodies': %d, 'time': %lf, 'leapfrog': %s, 'barnesHut': %s, 'precomputedGm': true }",
            num_time_steps,
            input_filename.c_str(),
            static_cast<int>(bodies.size()), // thank-you Joel
            cpu_time_elapsed,
            ENABLE_LEAPFROG ? "true" : "false",
            ENABLE_BARNES_HUT ? "true" : "false"
        );
    }

    fprintf(stderr, "Total CPU time was %lf\n", cpu_time_elapsed);
    fclose(log_fh);
    
    return 0;
}
