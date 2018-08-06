#include <stdio.h>
#include <stdlib.h>
#include "Body.hpp"

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
