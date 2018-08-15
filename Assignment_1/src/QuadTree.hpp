#ifndef _QuadTree_h
#define _QuadTree_h
#include "Body.hpp"

class QuadTree {
    public:
        // Constructors
        QuadTree();
        QuadTree(double x, double y, double radius);
        // Fields
        double x;
        double y;
        double radius;
        double mx;
        double my;
        double m;
        QuadTree* ne;
        QuadTree* nw;
        QuadTree* se;
        QuadTree* sw;
        Body *occupant;
        // Methods
        bool insert(Body& body);
        bool insert_all(std::vector<Body>& bodies);
        bool within_bounds(const Body& body) const;
        void subdivide();
        void calculate_force(Body& body);
};
#endif