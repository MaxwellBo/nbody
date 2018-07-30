#ifndef _Body_h
#define _Body_h

typedef struct Body {
    double m;

    // Position
    double x;
    double y;
    
    // Velocity
    double vx;
    double vy;

    // Force
    double Fx;
    double Fy;
} Body;

Body *new_Body();
void timestep(Body *self, double dt);
void reset_force(Body *self);

#endif