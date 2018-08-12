#include <stdio.h>
#include <stdlib.h>

#include "Body.hpp"
#include "utils.hpp"

const double G = 6.674e-11; // N(m/kg) ^ 2

Body::Body():
    m(1),
    x(0),
    y(0),
    vx(0),
    vy(0),
    Fx(0),
    Fy(0) {
}

void Body::timestep(double dt) {
    // printf("Step (%f, %f) to ", x, y);

    // F = ma
    double ax = Fx / m;
    double ay = Fy / m;

    // a = Δv / Δt
    double dvx = ax * dt;
    double dvy = ay * dt;

    vx += dvx;
    vy += dvy;

    // v = s / t
    double dx = vx * dt;
    double dy = vy * dt;

    x += dx;
    y += dy;

    // printf("(%f, %f)\n", x, y);
}

void Body::reset_force() {
    Fx = 0;
    Fy = 0;
}

void Body::exert_force_unidirectionally(const Body& there) {
    double m1 = m;
    double m2 = there.m;
    double r = distance(x, y, there.x, there.y);
    double r2 = r * r;

    double F = (G * m1 * m2) / r2;

    double delta_x = there.x - x;
    double delta_y = there.y - y;

    // turn the vector between our two points into a force vector
    // of the desired magnitude
    double scale_factor = F / r;

    Fx += delta_x * scale_factor;
    Fy += delta_y * scale_factor;
}