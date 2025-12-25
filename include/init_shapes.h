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

typedef enum
{
    SHAPE_LINE,
    SHAPE_CIRCLE,
    SHAPE_QUAD
} ShapeType;

typedef struct
{
    int id;
    ShapeType type;

    Vec2 acceleration;
    Vec2 velocity;

    float mass;
    float restitution;
    float friction;
    bool isDynamic;

    union
    {
        struct
        {
            float x, y, r;
        } circle;
        struct
        {
            Vec2 vertices[2];
        } line;
    } data;
} Body;

Circle init_circle(int id, float x, float y, float r);
Line init_line(int id, float x1, float y1, float x2, float y2);

#endif