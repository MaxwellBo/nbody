#include <stdlib.h>
#include "body.hpp"

Body *new_Body() {
    Body *self = (Body *)malloc(sizeof(Body));

    self->mass = 0;
    self->x = 0;
    self->y = 0;

    self->vx = 0;
    self->vy = 0;

    self->fx = 0;
    self->fy = 0;

    return self;
}