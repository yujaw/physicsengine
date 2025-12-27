#include <stdbool.h>
#include "vectors.h"

#ifndef SHAPES_H
#define SHAPES_H

#define COLOR_RED (Color){1.0f, 0.0f, 0.0f, 1.0f}
#define COLOR_GREEN (Color){0.0f, 1.0f, 0.0f, 1.0f}
#define COLOR_BLUE (Color){0.0f, 0.0f, 1.0f, 1.0f}
#define COLOR_YELLOW (Color){1.0f, 1.0f, 0.0f, 1.0f}

typedef struct
{
    float r;
    float g;
    float b;
    float a;
} Color;

#define MAX_SHAPES 100

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

    bool filled;
    bool isDynamic;

    Color color;

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
        struct
        {
            Vec2 vertices[4];
        } quad;
    } data;
} Body;

Body bodies[MAX_SHAPES];

void init_circle(Vec2 pos, float r, bool filled, Color color, float mass, bool isDyanamic);

void init_line(Vec2 a, Vec2 b, Color color, float mass, bool isDynamic);

#endif