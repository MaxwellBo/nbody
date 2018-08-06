#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include "Body.hpp"
#include "utils.hpp"

const double G     = 10000; // N(m/kg) ^ 2

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
    auto ax = Fx / m;
    auto ay = Fy / m;

    // a = Δv / Δt
    auto dvx = ax * dt;
    auto dvy = ay * dt;

    vx += dvx;
    vy += dvy;

    // v = s / t
    auto dx = vx * dt;
    auto dy = vy * dt;

    x += dx;
    y += dy;

    // printf("(%f, %f)\n", x, y);
}

void Body::reset_force() {
    Fx = 0;
    Fy = 0;
}

void Body::exert_force_unidirectionally(const Body& there) {
    auto m1 = m;
    auto m2 = there.m;
    auto r = distance(x, y, there.x, there.y);
    auto r2 = pow(r, 2);

    auto F = G * ((m1 * m2) / r2);

    auto delta_x = there.x - x;
    auto delta_y = there.y - y;

    // turn the vector between our two points into a force vector
    // of the desired magnitude
    auto scale_factor = F / r;

    Fx += delta_x * scale_factor;
    Fy += delta_y * scale_factor;
}