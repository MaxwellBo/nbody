from random import random

BODIES = 30
EDGE = 100
VELOCITY = 10

print(BODIES)
for i in range(0, BODIES):
    print(str(random() * 100) + "e14")

print(0.00, 10)

for i in range(0, BODIES):
    print(
        (random() * 2 * EDGE) - EDGE, 
        (random() * 2 * EDGE) - EDGE, 
        (random() * 2 * VELOCITY) - VELOCITY,
        (random() * 2 * VELOCITY) - VELOCITY
    )

