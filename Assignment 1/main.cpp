#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>

// #include "Body.cpp"
#include "QuadTree.cpp"

const double SIMULATION_WIDTH = 2000;

int main(int argc, char **argv) {

    if (argc == 1) {
        printf("Please supply an input .csv file\n");
        exit(1);
    }

    std::list<Body*> bodies = {};

    Body *foursix = new_Body();
    foursix->x = 4; foursix->y = 6; foursix->m = 5;

    Body *fivefive = new_Body();
    fivefive->x = 5; fivefive->y = 5; fivefive->m = 3;

    bodies.push_back(fivefive);
    bodies.push_back(foursix);

    while (true) {
        QuadTree *root = new_QuadTree(0, 0, SIMULATION_WIDTH / 2);
        // the quad-tree uses half the width as an implementation detail
        // called "radius". Don't ask me why I picked such a stupid name.

        for (auto body: bodies) {
            bool did_insert = insert(root, body);

            if (!did_insert) {
                bodies.remove(body); // out-of-bounds
            }
        }

        for (auto body: bodies) {
            reset_force(body);
            calculate_force(root, body);
            timestep(body, 1);
        }
    }

    const std::string &filename = argv[1];

    std::string line;
    std::ifstream fh(filename);

    if (fh.is_open())
    {
        while (getline(fh, line))
        {
            std::stringstream line_stream(line);
            std::string segment;
            std::vector<std::string> seglist;

            while (getline(line_stream, segment, ','))
            {
                seglist.push_back(segment);
            }
        }

        fh.close();
    }

    return 0;
}