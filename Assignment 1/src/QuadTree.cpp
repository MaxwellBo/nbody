#include <stdlib.h>
#include <iostream>
#include <cmath>

#include "QuadTree.hpp"

const double THETA = 0.5;
const double G     = 1; // N(m/kg) ^ 2

QuadTree *new_QuadTree(double x, double y, double radius) {
    QuadTree *self = (QuadTree *)malloc(sizeof(QuadTree));

    self->x = x;
    self->y = y;
    self->radius = radius;

    self->mx = 0;
    self->my = 0;
    self->m = 0;

    self->occupant = nullptr;
    self->ne = nullptr;
    self->nw = nullptr;
    self->sw = nullptr;
    self->se = nullptr;

    return self;
}

bool withinBounds(QuadTree *self, Body *body) {
    auto radius = self->radius;
    auto x = self->x;
    auto y = self->y;

    auto x_lower = x - radius;
    auto x_upper = x + radius;
    auto y_lower = y - radius;
    auto y_upper = y + radius;

    auto x_bounded = x_lower <= body->x && body->x < x_upper;
    auto y_bounded = y_lower <= body->y && body->y < y_upper;

    return x_bounded && y_bounded;
}

double distance(double x1, double y1, double x2, double y2) {
    double a2 = pow((x1 - x2), 2);
    double b2 = pow((y1 - y2), 2);

    return sqrt(a2 + b2);
}

void subdivide(QuadTree *self) {
    // this will be the radius of our children
    auto r = self->radius / 2;

    std::cout << "Subdividing (" << self->x << ", " << self->y << ") with radius " << self->radius << "\n ";

    auto x = self->x;
    auto y = self->y;

    // (x, y, radius), love me positional calling semantics
    self->ne = new_QuadTree(x + r, y + r, r);
    self->nw = new_QuadTree(x - r, y + r, r);
    self->se = new_QuadTree(x + r, y - r, r);
    self->sw = new_QuadTree(x - r, y - r, r);
}

/*  To calculate the net force acting on body b, use the following recursive procedure, 
    starting with the root of the quad-tree:

    1.  If the current node is an external node (and it is not body b), 
        calculate the force exerted by the current node on b, 
        and add this amount to b’s net force.

    2.  Otherwise, calculate the ratio s/d. 

        s is the width of the region represented by the internal node, and
        d is the distance between the body and the node's center-of-mass
    
        If s/d < θ, (the internal node is sufficiently far away) 
        treat this internal node as a single body, 
        and calculate the force it exerts on body b, 
        and add this amount to b’s net force.

    3. Otherwise, run the procedure recursively on each of the current node’s children.

    NB: Note that if θ = 0, then no internal node is treated as a single body, 
        and the algorithm degenerates to brute force.
*/
void calculate_force(QuadTree* self, Body *body) {
    // Case 1 - empty external node
    if (self->occupant == nullptr && self->nw == nullptr) { 
        return;
    }

    // Case 2 - occupied external node
    if (self->nw == nullptr) {
        if (self->occupant == body) {
            return;
        } else {
            auto here = body;
            auto there = self->occupant;

            auto m1 = here->m;
            auto m2 = there->m;
            auto r = distance(here->x, here->y, there->x, there->y);
            auto r2 = pow(r, 2);

            auto F = G * (( m1 * m2 ) / r2);

            auto delta_x = there->x - here->x;
            auto delta_y = there->y - here->y;

            // turn the vector between our two points into a force vector
            // of the desired magnitude
            auto scale_factor = F / r; 

            here->Fx += delta_x * scale_factor;
            here->Fy += delta_y * scale_factor;

            return;
        }
    }

    // Case 3 - internal node
    assert(self->occupant == nullptr && self->nw != nullptr);

    auto s = self-> radius * 2; // need width
    auto d = distance(body->x, body->y, self->x, self->y);

    if (s / d < THETA) {
        auto here = body;
        auto there = self;

        auto m1 = here->m;
        auto m2 = there->m;
        auto r = distance(here->x, here->y, there->mx, there->my);
        auto r2 = pow(r, 2);

        auto F = G * (( m1 * m2 ) / r2);

        auto delta_x = there->mx - here->x;
        auto delta_y = there->my - here->y;

        // turn the vector between our two points into a force vector
        // of the desired magnitude
        auto scale_factor = F / r; 

        here->Fx += delta_x * scale_factor;
        here->Fy += delta_y * scale_factor;

    } else {
        calculate_force(self->nw, body);
        calculate_force(self->ne, body);
        calculate_force(self->sw, body);
        calculate_force(self->se, body);
    }
}

/*  To construct the Barnes-Hut tree, insert the bodies one after another.
    To insert a body b into the tree rooted at node x, use the following recursive procedure:

    1.  If node x does not contain a body, put the new body b here.
    
    2.  If node x is an internal node, update the center-of-mass and total mass of x.
        Recursively insert the body b in the appropriate quadrant.
        
    3.  If node x is an external node, say containing a body named c, 
        then there are two bodies b and c in the same region.
        Subdivide the region further by creating four children.
        Then, recursively insert both b and c into the appropriate quadrant(s).
        Since b and c may still end up in the same quadrant, 
        there may be several subdivisions during a single insertion.
        Finally, update the center-of-mass and total mass of x.
*/
        
bool insert(QuadTree *self, Body *body) {
    if (!withinBounds(self, body)) {
        return false;
    }

    // We're going to be tricky here and update the centre of mass on the fly
    // Most Barnes-Hut implementations traverse the tree twice, and naively
    // log all the bodies that are decendants of the node.
    // We're not going to do that :)
    
    // the centre of mass lies at the following x and y coordinates
    // m = m1 + m2
    // mx = (x1m1 + x2m2) / m
    // my = (y1m1 + y2m2) / m
    auto new_x_total = (self->mx * self->m) + (body->x * body->m);
    auto new_y_total = (self->my * self->m) + (body->y * body->m);

    auto new_total_mass = self->m + body->m;

    self->m = new_total_mass;
    self->mx = new_x_total / new_total_mass;
    self->my = new_y_total / new_total_mass;

    // Case 1 - empty external node
    if (self->occupant == nullptr && self->nw == nullptr) { 
        self->occupant = body;

        return true;
    }

    Body *displaced = nullptr;

    // Case 2 - occupied external node
    if (self->nw == nullptr) {
        assert(self->occupant != nullptr);

        displaced = self->occupant;
        self->occupant = nullptr;

        subdivide(self);
    }

    bool displaced_insertion_success = false;

    // Cases 2 and 3 - newly subdivided external node and internal node
    assert(self->occupant == nullptr);

    if (displaced) {
        displaced_insertion_success = 
               insert(self->nw, displaced) 
            || insert(self->ne, displaced)
            || insert(self->sw, displaced)
            || insert(self->se, displaced);
    }

    bool new_insertion_success = 
           insert(self->nw, body) 
        || insert(self->ne, body)
        || insert(self->sw, body)
        || insert(self->se, body);

    assert(displaced_insertion_success || new_insertion_success);

    return true;
}