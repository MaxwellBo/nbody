#include <stdio.h>
#include <stdlib.h>
#include "body.hpp"

Body *new_Body() {
    Body *self = (Body *)malloc(sizeof(Body));

    // TODO: Panic on mass zero
    self->m = 1;
    self->x = 0;
    self->y = 0;

    self->vx = 0;
    self->vy = 0;

    self->Fx = 0;
    self->Fy = 0;

    return self;
}


void timestep(Body *self, double dt) {
    printf("Step (%f, %f) to ", self->x, self->y);

    // F = ma
    auto ax = self->Fx / self->m;
    auto ay = self->Fy / self->m;

    // a = Δv / Δt
    auto dvx = ax / dt;
    auto dvy = ay / dt;

    self->vx += dvx;
    self->vy += dvy;

    // v = s / t
    auto dx = self->vx * dt;
    auto dy = self->vy * dt;

    self->x += dx;
    self->y += dy;

    printf("(%f, %f)\n", self->x, self->y);
}

void reset_force(Body *self) {
    self->Fx = 0;
    self->Fy = 0;
}
