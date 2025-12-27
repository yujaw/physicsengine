#include "init_shapes.h"
#include <math.h>

extern int body_count;

void init_circle(Vec2 pos, float r, bool filled, Color color, float mass, bool isDynamic)
{
    if (body_count >= MAX_SHAPES)
        return;

    Body *object = &bodies[body_count++];

    object->id = body_count;
    object->type = SHAPE_CIRCLE;
    object->filled = filled;
    object->color = color;
    object->acceleration = (Vec2){0.0f, 0.0f};
    object->velocity = (Vec2){0.0f, 0.0f};
    object->mass = isDynamic ? mass : INFINITY;
    object->restitution = 0.8f;
    object->friction = 0.3f;
    object->isDynamic = isDynamic;
    object->data.circle.r = r;
    object->data.circle.x = pos.x;
    object->data.circle.y = pos.y;
}

void init_line(Vec2 a, Vec2 b, Color color, float mass, bool isDynamic)
{
    if (body_count >= MAX_SHAPES)
        return;

    Body *object = &bodies[body_count++];

    object->id = body_count;
    object->type = SHAPE_LINE;
    object->acceleration = (Vec2){0.0f, 0.0f};
    object->velocity = (Vec2){0.0f, 0.0f};
    object->mass = isDynamic ? mass : INFINITY;
    object->restitution = 0.8f;
    object->friction = 0.3f;
    object->isDynamic = isDynamic;
    object->data.line.vertices[0] = a;
    object->data.line.vertices[1] = b;
}