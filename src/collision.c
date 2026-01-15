#include "collision.h"
#include <stdlib.h>
#include <math.h>
#include <float.h>

Vec2 support(Body *body, Vec2 direction)
{
    if (vec_length(direction) < 1e-8f)
        direction = (Vec2){1.0f, 0.0f};
    else
        direction = vec_normalize(direction);

    switch (body->type)
    {
    case SHAPE_POLYGON:
    {
        Vec2 *verts = body->data.polygon.vertices;
        int n = body->data.polygon.numVertices;

        Vec2 best = verts[0];
        float bestDot = vec_dot(best, direction);

        for (int i = 1; i < n; i++)
        {
            float d = vec_dot(verts[i], direction);
            if (d > bestDot)
            {
                bestDot = d;
                best = verts[i];
            }
        }
        return best;
    }

    case SHAPE_ELLIPSE:
    {
        float dx = direction.x;
        float dy = direction.y;

        float rx = body->data.ellipse.r.x;
        float ry = body->data.ellipse.r.y;

        float denom = sqrtf((rx * dx) * (rx * dx) + (ry * dy) * (ry * dy));

        if (denom < 1e-8f) // If degenerate, fall back to center
            return body->data.ellipse.pos;

        return (Vec2){
            body->data.ellipse.pos.x + (rx * rx * dx) / denom,
            body->data.ellipse.pos.y + (ry * ry * dy) / denom};
    }

    case SHAPE_LINE:
    {
        Vec2 A = body->data.line.vertices[0];
        Vec2 B = body->data.line.vertices[1];

        float da = vec_dot(A, direction);
        float db = vec_dot(B, direction);

        return (da > db) ? A : B;
    }

    default:
        return (Vec2){0.0f, 0.0f};
    }
}

bool handleLine(Vec2 *simplex, int *count, Vec2 *dir)
{
    Vec2 A = simplex[*count - 2];
    Vec2 B = simplex[*count - 1];

    Vec2 AO = vec_neg(A);
    Vec2 AB = vec_sub(B, A);

    Vec2 perp = vec_tripleProduct(AB, AO, AB);

    if (vec_length(perp) < 1e-6)
    {
        perp = (Vec2){AB.y, -AB.x};
    }

    if (vec_dot(AB, AO) > 0)
        *dir = perp;
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

bool checkGJK(Body *A, Body *B, Vec2 simplexOut[3], int *simplexCountOut)
{
    Vec2 simplex[3];
    int count = 0;

    // Initial direction
    Vec2 direction = vec_sub(findCenter(A), findCenter(B));
    if (vec_length(direction) < 1e-8)
        direction = (Vec2){1, 0};

    // First support
    simplex[count++] = vec_sub(support(A, direction),
                               support(B, vec_neg(direction)));

    direction = vec_neg(simplex[0]);

    while (1)
    {
        if (vec_length(direction) < 1e-6)
            direction = (Vec2){-direction.y, direction.x};

        Vec2 newPoint = vec_sub(support(A, direction),
                                support(B, vec_neg(direction)));

        // No collision
        if (vec_dot(newPoint, direction) <= 0)
            return false;

        simplex[count++] = newPoint;

        if (handleSimplex(simplex, &count, &direction))
        {
            // OUTPUT SIMPLEX CLEANLY
            for (int i = 0; i < count; i++)
                simplexOut[i] = simplex[i];

            *simplexCountOut = count;
            return true;
        }
    }
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

CollisionResult calculateEPA(Body *A, Body *B, Vec2 simplex[3], int simplexCount)
{
    const float EPS = 1e-8f;
    const int MAX_ITER = 64;
    Vec2 poly[64];
    int count = simplexCount;

    // Copy simplex into polytope
    for (int i = 0; i < count; i++)
        poly[i] = simplex[i];

    for (int iter = 0; iter < MAX_ITER; iter++)
    {
        float minDist = FLT_MAX;
        int edge = -1;
        Vec2 bestNormal = {0.0f, 0.0f};

        for (int i = 0; i < count; i++)
        {
            Vec2 a = poly[i];
            Vec2 b = poly[(i + 1) % count];
            Vec2 e = vec_sub(b, a);

            Vec2 n = (Vec2){e.y, -e.x};
            n = vec_normalize(n);

            float dist = vec_dot(n, a);

            if (dist < 0)
            {
                n = vec_neg(n);
                dist = -dist;
            }

            if (dist < minDist)
            {
                minDist = dist;
                bestNormal = n;
                edge = i;
            }
        }

        if (edge < 0)
            return (CollisionResult){.hit = false};

        Vec2 p = vec_sub(
            support(A, bestNormal),
            support(B, vec_neg(bestNormal)));

        float pDist = vec_dot(bestNormal, p);

        if (pDist - minDist < EPS)
        {
            return (CollisionResult){
                .hit = true,
                .normal = bestNormal,
                .depth = minDist};
        }

        for (int i = count; i > edge + 1; i--)
            poly[i] = poly[i - 1];
        poly[edge + 1] = p;
        count++;

        if (count >= 63)
            break;
    }

    return (CollisionResult){.hit = false};
}

bool checkCollision(Body *A, Body *B, CollisionResult *out)
{
    Vec2 simplex[3];
    int simplexCount = 0;

    // Run GJK
    if (!checkGJK(A, B, simplex, &simplexCount))
    {
        out->hit = false;
        return false;
    }

    // Run EPA
    *out = calculateEPA(A, B, simplex, simplexCount);
    return out->hit;
}
