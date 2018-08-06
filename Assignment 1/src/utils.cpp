#include <cmath>

double distance(double x1, double y1, double x2, double y2) {
    double a2 = pow((x1 - x2), 2);
    double b2 = pow((y1 - y2), 2);

    return sqrt(a2 + b2);
}