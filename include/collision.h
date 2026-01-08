#include "init_shapes.h"
#include <stdlib.h>
#include <stdio.h>
#include <vectors.h>

#ifndef COLLISION_H
#define COLLISION_H

static int cmpVec2(const void *a, const void *b);
static Vec2 *convexHull(Vec2 *pts, int n, int *outCount);
void createMinkowskiDifference(Body *out, Body *A, Body *B, Color color);
bool checkGJK(Body *A, Body *B);
Vec2 support(Body *body, Vec2 direction);
bool handleSimplex(Vec2 *simplex, int *count, Vec2 *dir);
bool handleTriangle(Vec2 *simplex, int *count, Vec2 *dir);
bool handleLine(Vec2 *simplex, int *count, Vec2 *dir);

#endif