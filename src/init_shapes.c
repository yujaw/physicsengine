#include "init_shapes.h"
#include <math.h>
#include <stdlib.h>

int body_count;

Body *init_ellipse(Vec2 pos, Vec2 r, Color color)
{
    Body *object = 0;
    if (body_count < MAX_SHAPES)
    {
        object = &bodies[body_count++];

        object->id = body_count;
        object->type = SHAPE_ELLIPSE;
        object->filled = false;
        object->color = color;
        object->acceleration = (Vec2){0.0f, 0.0f};
        object->velocity = (Vec2){0.0f, 0.0f};
        object->mass = 0.0f;
        object->restitution = 0.8f;
        object->friction = 0.3f;
        object->isDynamic = false;
        object->data.ellipse.r = r;
        object->data.ellipse.pos = pos;
    }
    
    return object;
}

Body *init_line(Vec2 a, Vec2 b, Color color)
{
    Body *object = 0;
    if (body_count < MAX_SHAPES)
    {
        object = &bodies[body_count++];

        object->id = body_count;
        object->type = SHAPE_LINE;
        object->color = color;
        object->acceleration = (Vec2){0.0f, 0.0f};
        object->velocity = (Vec2){0.0f, 0.0f};
        object->mass = 0.0f;
        object->restitution = 0.8f;
        object->friction = 0.3f;
        object->isDynamic = false;
        object->data.line.vertices[0] = a;
        object->data.line.vertices[1] = b;
    }

    return object;
}

Body *init_polygon(Vec2 *vertices, int numVertices, Color color)
{
    Body *object = 0;
    if (body_count < MAX_SHAPES)
    {
        object = &bodies[body_count++];

        object->id = body_count;
        object->type = SHAPE_POLYGON;
        object->filled = false;
        object->color = color;
        object->acceleration = (Vec2){0.0f, 0.0f};
        object->velocity = (Vec2){0.0f, 0.0f};
        object->mass = 0.0f;
        object->restitution = 0.8f;
        object->friction = 0.3f;
        object->isDynamic = false;
        object->data.polygon.numVertices = numVertices;
        object->data.polygon.vertices = malloc(sizeof(Vec2) * numVertices);

        for (int i = 0; i < numVertices; i++)
        {
            object->data.polygon.vertices[i] = vertices[i];
        }
    }

    return object;
}

Vec2 findCenter(Body *body)
{
    if (body->type == SHAPE_POLYGON)
    {
        Vec2 center = {0.0f, 0.0f};
        for (int i = 0; i < body->data.polygon.numVertices; i++)
        {
            center.x += body->data.polygon.vertices[i].x;
            center.y += body->data.polygon.vertices[i].y;
        }

        center.x /= body->data.polygon.numVertices;
        center.y /= body->data.polygon.numVertices;

        return center;
    }
    if (body->type == SHAPE_ELLIPSE)
    {
        return body->data.ellipse.pos;
    }
    if(body->type == SHAPE_LINE)
    {
        Vec2 A = body->data.line.vertices[0];
        Vec2 B = body->data.line.vertices[1];
        return (Vec2){ (A.x + B.x) * 0.5f, (A.y + B.y) * 0.5f };
    }

    return (Vec2){0.0f, 0.0f};
}

bool removeBody(Body *body)
{
    if (!body || body_count == 0)
        return false;

    int index = body - bodies; // pointer → index

    if (index < 0 || index >= body_count)
        return false;

    // Free internal allocations
    if (body->type == SHAPE_POLYGON)
        free(body->data.polygon.vertices);

    // Move last body into this slot
    bodies[index] = bodies[body_count - 1];

    body_count--;
    return true;
}