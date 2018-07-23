#include <stdlib.h>
#include <iostream>

#include "QuadTree.hpp"

QuadTree *new_QuadTree(double x, double y, double radius) {
    QuadTree *self = (QuadTree *)malloc(sizeof(QuadTree));

    std::cout << "Creating tree at (" << x << ", " << y << ") with radius " << radius << "\n";

    self->x = x;
    self->y = y;
    self->radius = radius;

    self->occupant = nullptr;
    self->ne = nullptr;
    self->nw = nullptr;
    self->sw = nullptr;
    self->se = nullptr;

    return self;
}

bool withinBounds(QuadTree *self, Body *body) {
    // Yanking these out because later on we may move to classes
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

bool insert(QuadTree *self, Body *body) {
    if (!withinBounds(self, body)) {
        std::cout << "Body (" << body->x << ", " << body->y << ") OOB at (" << self->x << ", " << self->y << ") with radius " << self->radius << "\n"; 
        return false;
    }

    // Case 1 - empty external node
    if (self->occupant == nullptr && self->nw == nullptr) { 
        self->occupant = body;

        std::cout << "Inserted body (" << body->x << ", " << body->y << ") at (" << self->x << ", " << self->y << ") with radius " << self->radius << "\n"; 

        return true;
    }

    Body *displaced = nullptr;

    // Case 2 - occupied external node
    if (self->nw == nullptr) {
        displaced = self->occupant;
        self->occupant = nullptr;

        subdivide(self);
    }

    bool displaced_insertion_success = false;

    // Cases 2 and 3 - newly subdivided external external and internal nodes
    if (displaced) {
        std::cout << "Readding the displaced node" << "\n";
        displaced_insertion_success = 
            insert(self->nw, displaced) 
            || insert(self->ne, displaced)
            || insert(self->sw, displaced)
            || insert(self->se, displaced);
    }

    std::cout << "Adding the new node" << "\n";

    bool new_insertion_success = 
        insert(self->nw, body) 
        || insert(self->ne, body)
        || insert(self->sw, body)
        || insert(self->se, body);

    // panic on !(displaced_insertion_success || new_insertion_success)
    return displaced_insertion_success || new_insertion_success;
}