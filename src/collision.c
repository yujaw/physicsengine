#include "collision.h"
#include <stdlib.h>
#include <math.h>

void createMinkowskiDifference(Body *out, Body *A, Body *B)
{
    const int samples = 64; // number of sampled directions
    Vec2 *pts = malloc(sizeof(Vec2) * samples);

    for (int i = 0; i < samples; i++)
    {
        float angle = (2.0f * M_PI * i) / samples;
        Vec2 d = (Vec2){cosf(angle), sinf(angle)};

        Vec2 pA = support(A, d);
        Vec2 pB = support(B, (Vec2){-d.x, -d.y});

        pts[i] = vec_sub(pA, pB);
    }

    out->type = SHAPE_POLYGON;
    out->data.polygon.vertices = pts;
    out->data.polygon.numVertices = samples;
    out->filled = false;
}

Vec2 support(Body *body, Vec2 direction)
{
    if (body->type == SHAPE_POLYGON)
    {
        Vec2 far = body->data.polygon.vertices[0];
        float maxDot = vec_dot(body->data.polygon.vertices[0], direction);

        for (int i = 1; i < body->data.polygon.numVertices; i++)
        {
            float dot = vec_dot(body->data.polygon.vertices[i], direction);
            if (dot > maxDot)
            {
                maxDot = dot;
                far = body->data.polygon.vertices[i];
            }
        }

        return far;
    }

    if (body->type == SHAPE_ELLIPSE)
    {
        float dx = direction.x;
        float dy = direction.y;

        float rx = body->data.ellipse.r.x;
        float ry = body->data.ellipse.r.y;

        float denom = sqrtf((rx * dx) * (rx * dx) + (ry * dy) * (ry * dy));

        // Avoid division by zero
        if (denom == 0)
            return body->data.ellipse.pos;

        Vec2 out = {
            body->data.ellipse.pos.x + (rx * rx * dx) / denom,
            body->data.ellipse.pos.y + (ry * ry * dy) / denom};

        return out;
    }

    if (body->type == SHAPE_LINE)
    {
        Vec2 A = body->data.line.vertices[0];
        Vec2 B = body->data.line.vertices[1];

        return (vec_dot(A, direction) > vec_dot(B, direction)) ? A : B;
    }

    return (Vec2){0.0f, 0.0f};
}

bool handleLine(Vec2 *simplex, int *count, Vec2 *dir)
{
    Vec2 A = simplex[*count - 2];
    Vec2 B = simplex[*count - 1];

    Vec2 AO = vec_neg(A);
    Vec2 AB = vec_sub(B, A);

    if (vec_dot(AB, AO) > 0)
    {
        *dir = vec_tripleProduct(AB, AO, AB);
    }
    else
    {
        simplex[0] = A;
        *count = 1;
        *dir = AO;
    }

    return false;
}

bool handleTriangle(Vec2 *simplex, int *count, Vec2 *dir)
{
    Vec2 A = simplex[2];
    Vec2 B = simplex[1];
    Vec2 C = simplex[0];

    Vec2 AO = vec_neg(A);
    Vec2 AB = vec_sub(B, A);
    Vec2 AC = vec_sub(C, A);

    // Compute perpendiculars
    Vec2 ABperp = vec_tripleProduct(AC, AB, AB);
    Vec2 ACperp = vec_tripleProduct(AB, AC, AC);

    // If origin is outside AB edge
    if (vec_dot(ABperp, AO) > 1e-6f)
    {
        simplex[0] = B;
        simplex[1] = A;
        *count = 2;
        *dir = ABperp;
        return false;
    }

    // If origin is outside AC edge
    if (vec_dot(ACperp, AO) > 1e-6f)
    {
        simplex[1] = A;
        *count = 2;
        *dir = ACperp;
        return false;
    }

    if (vec_length(*dir) < 1e-6f)
        *dir = (Vec2){-AO.y, AO.x};

    return true;
}

bool handleSimplex(Vec2 *simplex, int *count, Vec2 *dir)
{
    if (*count == 2)
        return handleLine(simplex, count, dir);

    if (*count == 3)
        return handleTriangle(simplex, count, dir);

    return false;
}

bool checkGJK(Body *A, Body *B)
{
    if (A->type == SHAPE_POLYGON)
    {
        if (!polygonIsConvex(A->data.polygon.vertices, A->data.polygon.numVertices))
            return false;
    }

    if (B->type == SHAPE_POLYGON)
    {
        if (!polygonIsConvex(B->data.polygon.vertices, B->data.polygon.numVertices))
            return false;
    }

    if (vec_cmp(findCenter(A), findCenter(B)))
        return true;

    Vec2 simplex[3];
    int count = 0;

    // Initial direction
    Vec2 direction = vec_sub(findCenter(A), findCenter(B));
    if (direction.x == 0 && direction.y == 0)
    {
        direction = (Vec2){1.0f, 0.0f};
    }
    if (fabs(direction.x) < 1e-6 && fabs(direction.y) < 1e-6)
    {
        direction = (Vec2){1.0f, 0.0f};
    }
    else if (fabs(direction.x) < 1e-6)
    {
        direction = (Vec2){1.0f, 0.0f};
    }
    else if (fabs(direction.y) < 1e-6)
    {
        direction = (Vec2){0.0f, 1.0f};
    }

    // First support point
    simplex[count++] = vec_sub(support(A, direction), support(B, vec_neg(direction)));

    // New search direction toward origin
    direction = vec_neg(simplex[0]);

    while (true)
    {
        if (vec_length(direction) < 1e-6f)
            direction = (Vec2){-direction.y, direction.x}; // perpendicular perturbation

        Vec2 Anew = vec_sub(support(A, direction), support(B, vec_neg(direction)));

        if (vec_dot(Anew, direction) <= 0)
            return false;

        simplex[count++] = Anew;

        if (handleSimplex(simplex, &count, &direction))
        {
            drawSimplex(simplex, count);
            return true;
        }
    }
}
void drawSimplex(Vec2 *simplex, int count)
{
    // Copy simplex vertices into temporary buffer
    Vec2 *verts = malloc(sizeof(Vec2) * count);
    for (int i = 0; i < count; i++)
        verts[i] = vec_neg(simplex[i]);

    // Create a temporary body
    Body temp;
    temp.type = SHAPE_POLYGON;
    temp.color = (Color){0.0f, 1.0f, 0.0f, 0.2f};
    temp.filled = true;
    temp.data.polygon.vertices = verts;
    temp.data.polygon.numVertices = count;

    drawPolygon(&temp);

    free(verts);
}

bool polygonIsConvex(Vec2 *p, int n)
{
    bool sign = false;
    for (int i = 0; i < n; i++)
    {
        Vec2 a = p[i];
        Vec2 b = p[(i + 1) % n];
        Vec2 c = p[(i + 2) % n];

        float cross = vec_cross(vec_sub(b, a), vec_sub(c, b));
        if (i == 0)
            sign = cross > 0;
        else if ((cross > 0) != sign)
            return false;
    }
    return true;
}