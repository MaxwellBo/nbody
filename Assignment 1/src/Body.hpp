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
        void timestep(double dt);
        void reset_force();
};

#endif