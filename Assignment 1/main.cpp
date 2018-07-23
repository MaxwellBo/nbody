#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

// #include "Body.cpp"
#include "QuadTree.cpp"

int main(int argc, char **argv)
{
    QuadTree *root = new_QuadTree(0, 0, 10);

    Body *foursix = new_Body();
    foursix->x = 4; foursix->y = 6;

    Body *fivefive = new_Body();
    fivefive->x = 5; fivefive->y = 5;

    insert(root, foursix);
    insert(root, fivefive);

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