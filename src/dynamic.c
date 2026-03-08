#include "dynamic.h"

static void updatePosition(Body *body);
static void updateVelocity(Body *body, Vec2 accn);

void accelerate(Body *body, Vec2 accn)
{
    if (!body || !body->isDynamic)
        return;

    updateVelocity(body, accn);
    updatePosition(body);
}

static void updateVelocity(Body *body, Vec2 accn)
{
    body->velocity.x += accn.x;
    body->velocity.y += accn.y;
}

static void updatePosition(Body *body)
{
    Vec2 *v = &body->velocity;

    switch (body->type)
    {
    case SHAPE_ELLIPSE:
        body->data.ellipse.pos.x += v->x;
        body->data.ellipse.pos.y += v->y;
        break;

    case SHAPE_POLYGON:
        for (int i = 0; i < body->data.polygon.numVertices; i++)
        {
            body->data.polygon.vertices[i].x += v->x;
            body->data.polygon.vertices[i].y += v->y;
        }
        break;

    case SHAPE_LINE:
        for (int i = 0; i < 2; i++)
        {
            body->data.line.vertices[i].x += v->x;
            body->data.line.vertices[i].y += v->y;
        }
        break;

    default:
        break;
    }
}