#include "init_shapes.h"
#include <math.h>
#include <stdlib.h>
#include "draw_shapes.h"

int body_count;
Body bodies[MAX_SHAPES];

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
    if (body->type == SHAPE_LINE)
    {
        Vec2 A = body->data.line.vertices[0];
        Vec2 B = body->data.line.vertices[1];
        return (Vec2){(A.x + B.x) * 0.5f, (A.y + B.y) * 0.5f};
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

void decompose(Body *body, Body **triangles, int *triangle_count)
{
    // Body *triangles[body->data.polygon.numVertices - 2];
    *triangle_count = 0;

    // Create a list of remaining vertex indices
    int remaining[body->data.polygon.numVertices];
    int num_remaining = body->data.polygon.numVertices;
    for (int i = 0; i < num_remaining; i++)
    {
        remaining[i] = i;
    }

    // Keep clipping ears until we have a triangle
    while (num_remaining > 3)
    {
        bool ear_found = false;

        for (int i = 0; i < num_remaining; i++)
        {
            int prev_idx = (i - 1 + num_remaining) % num_remaining;
            int next_idx = (i + 1) % num_remaining;

            int a = remaining[prev_idx];
            int b = remaining[i];
            int c = remaining[next_idx];

            Vec2 va = body->data.polygon.vertices[a];
            Vec2 vb = body->data.polygon.vertices[b];
            Vec2 vc = body->data.polygon.vertices[c];

            // Check if this is a convex vertex (not reflex)
            Vec2 edge1 = {vb.x - va.x, vb.y - va.y};
            Vec2 edge2 = {vc.x - vb.x, vc.y - vb.y};
            float cross = edge1.x * edge2.y - edge1.y * edge2.x;

            if (cross <= 0)
            {
                continue; // Reflex vertex, skip
            }

            // Check if any other vertex is inside this triangle
            bool is_ear = true;
            for (int j = 0; j < num_remaining; j++)
            {
                if (j == prev_idx || j == i || j == next_idx)
                {
                    continue;
                }

                Vec2 p = body->data.polygon.vertices[remaining[j]];

                // Point-in-triangle test using cross products
                Vec2 v0 = {vc.x - vb.x, vc.y - vb.y};
                Vec2 v1 = {p.x - vb.x, p.y - vb.y};
                float d1 = v0.x * v1.y - v0.y * v1.x;

                Vec2 v2 = {va.x - vc.x, va.y - vc.y};
                Vec2 v3 = {p.x - vc.x, p.y - vc.y};
                float d2 = v2.x * v3.y - v2.y * v3.x;

                Vec2 v4 = {vb.x - va.x, vb.y - va.y};
                Vec2 v5 = {p.x - va.x, p.y - va.y};
                float d3 = v4.x * v5.y - v4.y * v5.x;

                bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
                bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

                if (!(has_neg && has_pos))
                {
                    // Point is inside the triangle
                    is_ear = false;
                    break;
                }
            }

            if (is_ear)
            {
                // Create triangle
                triangles[*triangle_count] = malloc(sizeof(Body));
                triangles[*triangle_count]->type = SHAPE_POLYGON;
                triangles[*triangle_count]->color = COLOR_RED;
                triangles[*triangle_count]->data.polygon.vertices = malloc(3 * sizeof(Vec2));
                triangles[*triangle_count]->data.polygon.vertices[0] = va;
                triangles[*triangle_count]->data.polygon.vertices[1] = vb;
                triangles[*triangle_count]->data.polygon.vertices[2] = vc;
                triangles[*triangle_count]->data.polygon.numVertices = 3;
                (*triangle_count)++;

                // Remove vertex b from remaining list
                for (int k = i; k < num_remaining - 1; k++)
                {
                    remaining[k] = remaining[k + 1];
                }
                num_remaining--;

                ear_found = true;
                break;
            }
        }

        if (!ear_found)
        {
            // Degenerate polygon or wrong winding order
            break;
        }
    }

    // Add the final triangle
    if (num_remaining == 3)
    {
        triangles[*triangle_count] = malloc(sizeof(Body));
        triangles[*triangle_count]->type = SHAPE_POLYGON;
        triangles[*triangle_count]->color = COLOR_RED;
        triangles[*triangle_count]->data.polygon.vertices = malloc(3 * sizeof(Vec2));
        triangles[*triangle_count]->data.polygon.vertices[0] = body->data.polygon.vertices[remaining[0]];
        triangles[*triangle_count]->data.polygon.vertices[1] = body->data.polygon.vertices[remaining[1]];
        triangles[*triangle_count]->data.polygon.vertices[2] = body->data.polygon.vertices[remaining[2]];
        triangles[*triangle_count]->data.polygon.numVertices = 3;
        (*triangle_count)++;
    }
}

// Check if a point is inside a polygon
bool pointInPolygon(Vec2 point, Vec2 *vertices, int numVertices)
{
    bool inside = false;
    for (int i = 0, j = numVertices - 1; i < numVertices; j = i++)
    {
        Vec2 vi = vertices[i];
        Vec2 vj = vertices[j];

        if (((vi.y > point.y) != (vj.y > point.y)) &&
            (point.x < (vj.x - vi.x) * (point.y - vi.y) / (vj.y - vi.y) + vi.x))
        {
            inside = !inside;
        }
    }
    return inside;
}

// Check if a point is inside an ellipse
bool pointInEllipse(Vec2 point, Body *ellipse)
{
    Vec2 center = ellipse->data.ellipse.pos;
    Vec2 r = ellipse->data.ellipse.r;

    float dx = (point.x - center.x) / r.x;
    float dy = (point.y - center.y) / r.y;

    return (dx * dx + dy * dy) <= 1.0f;
}

bool isInsideShape(Body *a, Body *b)
{
    Vec2 centerA = findCenter(a);

    if (b->type == SHAPE_POLYGON)
    {
        return pointInPolygon(centerA, b->data.polygon.vertices, b->data.polygon.numVertices);
    }
    else if (b->type == SHAPE_ELLIPSE)
    {
        return pointInEllipse(centerA, b);
    }
    else if (b->type == SHAPE_LINE)
    {
        return false;
    }

    return false;
}