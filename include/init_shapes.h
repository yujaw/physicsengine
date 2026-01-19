#include <stdbool.h>
#include "vectors.h"

#ifndef SHAPES_H
#define SHAPES_H

#define COLOR_RED (Color){1.0f, 0.0f, 0.0f, 1.0f}
#define COLOR_GREEN (Color){0.0f, 1.0f, 0.0f, 1.0f}
#define COLOR_BLUE (Color){0.0f, 0.0f, 1.0f, 1.0f}
#define COLOR_YELLOW (Color){1.0f, 1.0f, 0.0f, 1.0f}
#define COLOR_WHITE (Color){1.0f, 1.0f, 1.0f, 1.0f}
#define COLOR_BLACK (Color){0.0f, 0.0f, 0.0f, 1.0f}

extern int body_count;

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
    SHAPE_POLYGON,
    SHAPE_ELLIPSE
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
            Vec2 vertices[2];
            Vec2 center;
        } line;
        struct
        {
            Vec2 *vertices;
            int numVertices;
            Vec2 center;
        } polygon;
        struct
        {
            Vec2 pos;
            Vec2 r;
            float rotation;
        } ellipse;
    } data;
} Body;

Body bodies[MAX_SHAPES];

Body *init_line(Vec2 a, Vec2 b, Color color);

Body *init_polygon(Vec2 *vertices, int numVertices, Color color);

Body *init_ellipse(Vec2 pos, Vec2 r, Color color);

Vec2 findCenter(Body *body);

bool removeBody(Body *body);

#endif