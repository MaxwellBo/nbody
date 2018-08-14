#ifndef _Body_h
#define _Body_h

class Body {
    public:
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
};

#endif