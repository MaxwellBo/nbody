#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <assert.h>
#include <time.h>
#include <algorithm>
#include <limits>
#include <omp.h>
#include <mpi.h>

#include "Body.hpp"
#include "QuadTree.hpp"
#include "utils.hpp"

const unsigned int LEAP = 0;
const unsigned int FROG = 1;

const bool ENABLE_BARNES_HUT = false;
const bool ENABLE_LEAPFROG = true;
const bool ENABLE_LOGGING = true;

double cpu_time(void) {
    return (double)clock() / (double)CLOCKS_PER_SEC;
}

double maximum_deviation_from_root(const std::vector<Body>& bodies) {
    double lowest_x = std::numeric_limits<double>::max();
    double highest_x = std::numeric_limits<double>::min();
    double lowest_y = std::numeric_limits<double>::max();
    double highest_y = std::numeric_limits<double>::min();

    for (size_t i = 0; i < bodies.size(); i++) {
        auto& body = bodies[i];

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

    #pragma omp parallel for reduction(+:acc)
    for (size_t i = 0; i < bodies.size(); i++) {
        auto& body = bodies[i];
        acc += body.kinetic_energy();
    }

    // This does not parallelize nicely
    for (size_t i = 0; i < bodies.size() - 1; i++) {
        auto& x = bodies[i];

        for (size_t j = i + 1; j < bodies.size(); j++) {
            auto& y = bodies[j];

            acc += x.gravitational_potential_energy(y);
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
    unsigned int output_interval,
    double delta_t,
    const std::vector<Body>& bodies
) {
    const unsigned int bodies_n = bodies.size();

    fprintf(stdout, "%d %d %d %f\n", bodies_n, num_time_steps, output_interval, delta_t); 
}


void broadcast_bodies(std::vector<Body> bodies, int rank) {
    size_t bodies_n = bodies.size();
    
    double x[bodies_n];
    double y[bodies_n];
    double vx[bodies_n];
    double vy[bodies_n];

    MPI_Bcast(x, bodies_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(y, bodies_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(vx, bodies_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(vy, bodies_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (size_t i = 0; i < bodies.size(); i++) {
        auto& body = bodies[i];
        body.x = x[i];
        body.y = y[i];
        body.vx = vx[i];
        body.vy = vy[i];
    }

    printf("hello from %d\n", rank);
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
                masses.push_back(std::stod(segs[0]));
            } 
            // a total energy description
            else if (segs.size() == 2) { 
            }
            // a body
            else if (segs.size() == 4) {
                Body body = Body();
                body.x = std::stod(segs[0]);
                body.y = std::stod(segs[1]);
                body.vx = std::stod(segs[2]);
                body.vy = std::stod(segs[3]);

                bodies.push_back(body);
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

    int size;
    int rank;
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(comm, &size); // Get the number of processes
    MPI_Comm_rank(comm, &rank); // Get the rank of the process

    const unsigned int num_time_steps = std::stoi(argv[1]);
    const unsigned int desired_simulation_steps = num_time_steps * (ENABLE_LEAPFROG ? 2 : 1);

    const unsigned int output_interval = std::stoi(argv[2]);
    const unsigned int output_simulation_step_interval = output_interval * (ENABLE_LEAPFROG ? 2 : 1);

    const double timestep = std::stod(argv[3]);
    const double halfstep = timestep / 2;

    const std::string &input_filename = argv[4];

    std::ifstream input_fh(input_filename);

    std::vector<Body> bodies = parse_input_file(input_fh);

    double t = 0; 
    unsigned int step = 0;

    if (rank == 0) {
        dump_meta_info(num_time_steps, output_interval, timestep, bodies);
        dump_masses(bodies);
        dump_timestep(t, bodies);

        fprintf(stderr, "Barnes-Hut enabled: %s\n", ENABLE_BARNES_HUT ? "true" : "false");
        fprintf(stderr, "Leapfrog enabled: %s\n", ENABLE_LEAPFROG ? "true" : "false");
        fprintf(stderr, "OMP Max threads: %d\n", omp_get_max_threads());
    }

    broadcast_bodies(bodies, rank);

    double start = cpu_time();
    while (step < desired_simulation_steps) {
        QuadTree root;

        // only the Frog step (not the Leap step) needs updated forces
        // hence all the (step % 2 == FROG) tests
        const bool need_force_calc = !ENABLE_LEAPFROG || step % 2 == FROG;

        if (need_force_calc) {
            if (ENABLE_BARNES_HUT) {
                const double root_x = 0;
                const double root_y = 0;
                const double radius = maximum_deviation_from_root(bodies) + 1;
                // the quad-tree uses half the width as an implementation detail
                // called "radius". We're trying to make a QuadTree that encapsulates
                // the most distant body

                root = QuadTree(root_x, root_y, radius);

                assert(root.insert_all(bodies));
            }

            #pragma omp parallel for shared(bodies)
            for (size_t i = 0; i < bodies.size(); i++) {
                auto& body = bodies[i];
                body.reset_force();

                if (ENABLE_BARNES_HUT) {
                    root.calculate_force(body);
                }
            }

            if (!ENABLE_BARNES_HUT) {
                #pragma omp parallel for shared(bodies)
                for (size_t i = 0; i < bodies.size(); i++) {
                    auto& x = bodies[i];

                    for (size_t j = 0; j < bodies.size(); j++) {
                        auto& y = bodies[j];

                        if (&bodies[i] != &bodies[j]) {
                            // XXX: Do NOT swap these around. You will cause
                            // race conditions
                            x.exert_force_unidirectionally(y);
                        }
                    }
                }
            }
        }

        #pragma omp parallel for shared(bodies)
        for (size_t i = 0; i < bodies.size(); i++) {
            auto& body = bodies[i];

            if (step % 2 == LEAP) {
                body.leap(timestep);
            }
            else {
                body.frog(timestep);
            }
        }

        t += halfstep;
        step++;

        if (step % output_simulation_step_interval == 0) {
            if (rank == 0) {
                dump_timestep(t, bodies);
            }
        }
    }

    MPI_Finalize();

    const double cpu_time_elapsed = cpu_time() - start;

    if (rank == 0) {
        fprintf(stderr, "Total CPU time was %lf\n", cpu_time_elapsed);
    }

    if (ENABLE_LOGGING && rank == 0) {
        FILE *log_fh = fopen("nbody.log", "a");

        fprintf(
            log_fh,
            ",\n{ 'numTimeSteps': %d, 'inputFile': '%s', 'numBodies': %d, 'time': %lf, 'leapfrog': %s, 'barnesHut': %s, 'precomputedGm': true, 'ompMaxThreads': %d }",
            num_time_steps,
            input_filename.c_str(),
            static_cast<int>(bodies.size()), // thank-you Joel
            cpu_time_elapsed,
            ENABLE_LEAPFROG ? "true" : "false",
            ENABLE_BARNES_HUT ? "true" : "false",
            omp_get_max_threads()
        );

        fclose(log_fh);
    }
    
    return 0;
}
