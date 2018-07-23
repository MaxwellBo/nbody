#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

int main(int argc, char **argv)
{

    const std::string& filename = argv[1];

    std::string line;
    std::ifstream fh(filename);

    float total = 0;
    int count = 0;

    if (fh.is_open()) {
        while (getline(fh, line)) {
            std::stringstream line_stream(line);
            std::string segment;
            std::vector<std::string> seglist;

            while (getline(line_stream, segment, ','))
            {
                seglist.push_back(segment);
            }

            total += stof(seglist[1], nullptr);
            count++;
        }

        fh.close();
    }

    std::cout << (total / count);

    return 0;
}