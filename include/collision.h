#include "init_shapes.h"
#include "draw_shapes.h"
#include <stdlib.h>
#include <stdio.h>
#include <vectors.h>

#ifndef COLLISION_H
#define COLLISION_H

typedef struct
{
    bool hit;
    Vec2 normal;
    float depth;
} CollisionResult;

// void createMinkowskiDifference(Body *out, Body *A, Body *B);
bool checkGJK(Body *A, Body *B, Vec2 simplexOut[3], int *simplexCountOut);
bool checkCollision(Body *A, Body *B, CollisionResult *result);
Vec2 support(Body *body, Vec2 direction);
bool handleSimplex(Vec2 *simplex, int *count, Vec2 *dir);
bool handleTriangle(Vec2 *simplex, int *count, Vec2 *dir);
bool handleLine(Vec2 *simplex, int *count, Vec2 *dir);
bool polygonIsConvex(Vec2 *p, int n);
CollisionResult calculateEPA();

#endif