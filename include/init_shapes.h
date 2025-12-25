#include <stdbool.h>

#ifndef SHAPES_H
#define SHAPES_H

typedef struct
{
    int id;
    float x;
    float y;
    float r;
} Circle;

typedef struct
{
    int id;
    float x1;
    float y1;
    float x2;
    float y2;
} Line;


Circle init_circle(int id, float x, float y, float r);
Line init_line(int id, float x1, float y1, float x2, float y2);

#endif