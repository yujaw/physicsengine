#include "movement.h"
#include <math.h>

void move(Body *body, float dx, float dy)
{
    switch (body->type)
    {
    case SHAPE_ELLIPSE:

        body->data.ellipse.pos.x += dx;
        body->data.ellipse.pos.y += dy;
        break;
    case SHAPE_POLYGON:

        for (int i = 0; i < body->data.polygon.numVertices; i++)
        {
            body->data.polygon.vertices[i].x += dx;
            body->data.polygon.vertices[i].y += dy;
        }
        break;
    case SHAPE_LINE:

        for (int i = 0; i < 2; i++)
        {
            body->data.line.vertices[i].x += dx;
            body->data.line.vertices[i].y += dy;
        }
        break;
    default:
        break;
    }
}

void rotate(Body *body, float angle)
{
    float sin = sinf(angle);
    float cos = cosf(angle);
    switch (body->type)
    {
    case SHAPE_POLYGON:
        Vec2 center = findCenter(body);
        for (int i = 0; i < body->data.polygon.numVertices; i++)
        {
            float x = body->data.polygon.vertices[i].x - center.x;
            float y = body->data.polygon.vertices[i].y - center.y;
            body->data.polygon.vertices[i].x = (x * cos - y * sin) + center.x;
            body->data.polygon.vertices[i].y = (x * sin + y * cos) + center.y;
        }
        break;
    case SHAPE_ELLIPSE:
        body->data.ellipse.rotation += angle;
        break;
    case SHAPE_LINE:
    {
        Vec2 center = {
            (body->data.line.vertices[0].x + body->data.line.vertices[1].x) / 2.0f,
            (body->data.line.vertices[0].y + body->data.line.vertices[1].y) / 2.0f};

        float cos = cosf(angle);
        float sin = sinf(angle);

        for (int i = 0; i < 2; i++)
        {
            float x = body->data.line.vertices[i].x - center.x;
            float y = body->data.line.vertices[i].y - center.y;

            body->data.line.vertices[i].x = x * cos - y * sin + center.x;
            body->data.line.vertices[i].y = x * sin + y * cos + center.y;
        }
        break;
    }
    }
}