#include <cmath>

double distance(double x1, double y1, double x2, double y2) {
    double a = x1 - x2;
    double b = y1 - y2;
    
    double a2 = a * a;
    double b2 = b * b;

    return sqrt(a2 + b2);
}