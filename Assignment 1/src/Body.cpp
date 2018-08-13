#include <stdio.h>
#include <stdlib.h>
#include <cmath>

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
}

void Body::reset_force() {
    Fx = 0;
    Fy = 0;
}

double Body::kinetic_energy() const {
    double v2 = (vx * vx) + (vy * vy);

    return (m * v2) / 2;
}

double Body::gravitational_potential_energy(const Body& there) const {
    double R = distance(x, y, there.x, there.y); // final distance, aka, to edge

    return (-G * m * there.m) / R;
}

void Body::exert_force_unidirectionally(const Body& there) {
    double m1 = m;
    double m2 = there.m;
    double r = distance(x, y, there.x, there.y);
    double r2 = r * r;

    double F = (G * m1 * m2) / r2;

    double delta_x = there.x - x;
    double delta_y = there.y - y;

    // turn the displacement vector between our two points into a force vector
    // of the desired magnitude
    double scale_factor = F / r;

    Fx += delta_x * scale_factor;
    Fy += delta_y * scale_factor;
}