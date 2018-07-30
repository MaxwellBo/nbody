#ifndef _QuadTree_h
#define _QuadTree_h
#include "Body.hpp"

// class QuadTree {
//     public:
//         // Constructors
//         QuadTree(double x, double y);
//         // Fields
//         double x;
//         double y;
//         double radius;
//         QuadTree* ne;
//         QuadTree* nw;
//         QuadTree* se;
//         QuadTree* sw;
//         Body& occupant;
//         // Methods
//         bool insert(Body& body);
//         bool withinBounds(Body& body);
//         void subdivide();
// };

typedef struct QuadTree {
    double x;
    double y;
    double radius;
    double mx;
    double my;
    double m;
    Body *occupant;
    QuadTree* ne;
    QuadTree* nw;
    QuadTree* se;
    QuadTree* sw;
} QuadTree;

QuadTree *new_QuadTree(double x, double y, double radius);
bool withinBounds(QuadTree *self, Body *body);
double distance(double x1, double y1, double x2, double y2);
void subdivide(QuadTree *self);
void calculate_force(QuadTree* self, Body *body);
bool insert(QuadTree *self, Body *body);


#endif