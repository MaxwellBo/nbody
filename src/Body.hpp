#ifndef _Body_h
#define _Body_h

const double G = 6.674e-11; // N(m/kg) ^ 2

class Body {
    public:
        double m;
        double Gm;
        // Position
        double x;
        double y;
        // Velocity
        double vx;
        double vy;
        // Force
        double Fx;
        double Fy;
        Body();
        void euler_integrate(double dt);
        void kick_drift(double dt);
        void kick(double dt);
        void leap(double dt);
        void frog(double dt);
        void reset_force();
        double kinetic_energy() const;
        double gravitational_potential_energy(const Body& there) const;
        void exert_force_unidirectionally(const Body& there);
        void exert_force_bidirectionally(Body &there);
};

#endif
