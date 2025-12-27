#include "init_shapes.h"

#ifndef COLLISION_H
#define COLLISION_Hs

typedef struct
{
    bool collided;
    Vec2 normal;
    float penetration;
    Body *bodyA;
    Body *bodyB;
} CollisionInfo;

#endif