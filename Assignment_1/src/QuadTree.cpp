#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <assert.h>

#include "QuadTree.hpp"
#include "Body.hpp"
#include "utils.hpp"

const double THETA = 0.5;

QuadTree::QuadTree() {
}

QuadTree::QuadTree(double x, double y, double radius):
    x(x),
    y(y),
    radius(radius),
    mx(0),
    my(0),
    m(1),
    ne(nullptr),
    nw(nullptr),
    se(nullptr),
    sw(nullptr),
    occupant(nullptr) {
}

bool QuadTree::within_bounds(const Body& body) {
    auto x_lower = x - radius;
    auto x_upper = x + radius;
    auto y_lower = y - radius;
    auto y_upper = y + radius;

    auto x_bounded = x_lower <= body.x && body.x < x_upper;
    auto y_bounded = y_lower <= body.y && body.y < y_upper;

    return x_bounded && y_bounded;
}

void QuadTree::subdivide() {
    // this will be the radius of our children
    auto r = radius / 2;

    // (x, y, radius), love me positional calling semantics
    ne = new QuadTree(x + r, y + r, r);
    nw = new QuadTree(x - r, y + r, r);
    se = new QuadTree(x + r, y - r, r);
    sw = new QuadTree(x - r, y - r, r);
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
void QuadTree::calculate_force(Body& body) {
    // Case 1 - empty external node
    if (occupant == nullptr && nw == nullptr) { 
        return;
    }

    // Case 2 - occupied external node
    if (nw == nullptr) {
        if (occupant == &body) {
            return;
        } else {
            auto& there = *occupant;
        
            body.exert_force_unidirectionally(there);

            return;
        }
    }

    // Case 3 - internal node
    assert(occupant == nullptr && nw != nullptr);

    auto s = radius * 2; // need width
    auto d = distance(body.x, body.y, x, y);

    if (s / d < THETA) {
        Body pseudobody = Body();
        pseudobody.x = mx;
        pseudobody.y = my;
        pseudobody.m = m;

        // fprintf(stderr, "Coalesce tree rooted at (%lf, %lf)\n", x, y);

        body.exert_force_unidirectionally(pseudobody);
        return;

    } else {
        nw->calculate_force(body);
        ne->calculate_force(body);
        sw->calculate_force(body);
        se->calculate_force(body);

        return;
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

bool QuadTree::insert_all(std::vector<Body>& bodies) {
    for (auto& body : bodies) {
        bool did_insert = insert(body);
        if (!did_insert)
        {
            return false;
        }
    }
    return true;
}

bool QuadTree::insert(Body& body) {
    if (!within_bounds(body)) {
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
    auto new_x_total = (mx * m) + (body.x * body.m);
    auto new_y_total = (my * m) + (body.y * body.m);

    auto new_total_mass = m + body.m;

    m = new_total_mass;
    mx = new_x_total / new_total_mass;
    my = new_y_total / new_total_mass;

    // Case 1 - empty external node
    if (occupant == nullptr && nw == nullptr) { 
        occupant = &body;

        return true;
    }

    Body *displaced = nullptr;

    // Case 2 - occupied external node
    if (nw == nullptr) {
        assert(occupant != nullptr);

        displaced = occupant;
        occupant = nullptr;

        subdivide();
    }

    bool displaced_insertion_success = false;

    // Cases 2 and 3 - newly subdivided external node and internal node
    assert(occupant == nullptr);

    if (displaced) {
        displaced_insertion_success = 
               nw->insert(*displaced) 
            || ne->insert(*displaced)
            || sw->insert(*displaced)
            || se->insert(*displaced);
    }

    bool new_insertion_success = 
           nw->insert(body) 
        || ne->insert(body)
        || sw->insert(body)
        || se->insert(body);

    assert(displaced_insertion_success || new_insertion_success);

    return true;
}
