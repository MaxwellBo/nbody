#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include "Body.hpp"
#include "utils.hpp"

Body::Body():
    m(1),
    Gm(G * m),
    x(0),
    y(0),
    vx(0),
    vy(0),
    Fx(0),
    Fy(0) {
}

void Body::euler_integrate(double dt) {
    // F = ma
    const double ax = Fx / m;
    const double ay = Fy / m;

    // a = Δv / Δt
    const double dvx = ax * dt;
    const double dvy = ay * dt;

    vx += dvx;
    vy += dvy;

    // v = s / t
    const double dx = vx * dt;
    const double dy = vy * dt;

    x += dx;
    y += dy;
}

// pos and velocity
void Body::leap(double dt) {
    x = x + (vx * dt);
    y = y + (vy * dt);
}

// velocity, and force
void Body::frog(double dt) {
    const double ax = Fx / m;
    const double ay = Fy / m;

    vx = vx + (ax * dt);
    vy = vy + (ay * dt);
}

void Body::kick_drift(double dt) {
    const double ax = Fx / m;
    const double ay = Fy / m;

    vx = vx + (ax * (dt / 2));
    vy = vy + (ay * (dt / 2));

    x = x + (vx * dt);
    y = y + (vy * dt);
}

void Body::kick(double dt) {
    const double ax = Fx / m;
    const double ay = Fy / m;

    vx = vx + (ax * (dt / 2));
    vy = vy + (ay * (dt / 2));
}

void Body::reset_force() {
    Fx = 0;
    Fy = 0;
}

double Body::kinetic_energy() const {
    const double v2 = (vx * vx) + (vy * vy);

    return (m * v2) / 2;
}

double Body::gravitational_potential_energy(const Body& there) const {
    const double R = distance(x, y, there.x, there.y); // final distance, aka, to edge

    return (-Gm * there.m) / R;
}

void Body::exert_force_unidirectionally(const Body& there) {
    // const double m1 = m; We have a precomputed Gm that we're going to use
    const double m2 = there.m;

    // Rather than calling `distance`, we re-use Δx and Δy
    // in a direct call to hypot and for the construction of our force vector
    // this gives us around a +4% performance improvement, primarily because
    // distance and exert_force_(un/b)idirectionally consume the majority of our
    // CPU time
    const double delta_x = there.x - x;
    const double delta_y = there.y - y;

    const double r = hypot(delta_x, delta_y);
    const double r2 = pow(r, 2);

    const double F = (Gm * m2) / r2;

    // turn the displacement vector between our two points into a force vector
    // of the desired magnitude
    const double scale_factor = F / r;

    Fx += delta_x * scale_factor;
    Fy += delta_y * scale_factor;
}

void Body::exert_force_bidirectionally(Body& there) {
    // See exert_force_unidirectionally for inline commentary
    const double m2 = there.m;

    const double delta_x = there.x - x;
    const double delta_y = there.y - y;

    const double r = hypot(delta_x, delta_y);
    const double r2 = pow(r, 2);

    const double F = (Gm * m2) / r2;

    const double scale_factor = F / r;

    #pragma omp critical
    {
    Fx += delta_x * scale_factor;
    Fy += delta_y * scale_factor;

    there.Fx -= delta_x * scale_factor;
    there.Fy -= delta_y * scale_factor;
    }
}
