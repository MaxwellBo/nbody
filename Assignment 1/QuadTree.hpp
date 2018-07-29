#include "Body.cpp"

// class QuadTree {
//     public:
//         // Constructors
//         QuadTree(double x, double y);
//         // Fields
//         double x;
//         double y;
//         double radius;
//         QuadTree* ne;
//         QuadTree* nw;
//         QuadTree* se;
//         QuadTree* sw;
//         Body& occupant;
//         // Methods
//         bool insert(Body& body);
//         bool withinBounds(Body& body);
//         void subdivide();
// };

typedef struct QuadTree {
    double x;
    double y;
    double radius;
    double mx;
    double my;
    double m;
    Body *occupant;
    QuadTree* ne;
    QuadTree* nw;
    QuadTree* se;
    QuadTree* sw;
} QuadTree;
