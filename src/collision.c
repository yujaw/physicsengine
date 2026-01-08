#include "collision.h"
#include <stdlib.h>
#include <math.h>

int cmpVec2(const void *a, const void *b)
{
    Vec2 *p = (Vec2 *)a;
    Vec2 *q = (Vec2 *)b;

    if (p->x != q->x)
        return (p->x < q->x) ? -1 : 1;
    return (p->y < q->y) ? -1 : 1;
}

Vec2 *convexHull(Vec2 *pts, int n, int *outCount)
{
    qsort(pts, n, sizeof(Vec2), cmpVec2);

    Vec2 *hull = malloc(sizeof(Vec2) * (2 * n));
    int k = 0;

    // Lower hull
    for (int i = 0; i < n; i++)
    {
        while (k >= 2 &&
               vec_cross(vec_sub(hull[k - 1], hull[k - 2]),
                         vec_sub(pts[i], hull[k - 2])) <= 0)
            k--;
        hull[k++] = pts[i];
    }

    // Upper hull
    for (int i = n - 2, t = k + 1; i >= 0; i--)
    {
        while (k >= t &&
               vec_cross(vec_sub(hull[k - 1], hull[k - 2]),
                         vec_sub(pts[i], hull[k - 2])) <= 0)
            k--;
        hull[k++] = pts[i];
    }

    *outCount = k - 1; // last point repeats first
    return hull;
}

void createMinkowskiDifference(Body *out, Body *A, Body *B, Color color)
{
    int maxPts =
        A->data.polygon.numVertices *
        B->data.polygon.numVertices;

    Vec2 *points = malloc(sizeof(Vec2) * maxPts);

    int idx = 0;
    for (int i = 0; i < A->data.polygon.numVertices; i++)
    {
        for (int j = 0; j < B->data.polygon.numVertices; j++)
        {
            points[idx++] = vec_sub(A->data.polygon.vertices[i], B->data.polygon.vertices[j]);
        }
    }

    int hullCount;
    Vec2 *hull = convexHull(points, idx, &hullCount);
    out->data.polygon.vertices = hull;
    out->data.polygon.numVertices = hullCount;
    free(points);
}

Vec2 support(Body *body, Vec2 direction)
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

bool handleLine(Vec2 *simplex, int *count, Vec2 *dir)
{
    Vec2 A = simplex[*count - 1];
    Vec2 B = simplex[*count - 2];

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

    Vec2 perpAB = vec_tripleProduct(AC, AB, AB);
    
    if (vec_dot(perpAB, AO) > 0)
    {
        simplex[0] = B;
        simplex[1] = A;
        *count = 2;
        *dir = perpAB;
        return false;
    }
    
    Vec2 perpAC = vec_tripleProduct(AB, AC, AC);

    if (vec_dot(perpAC, AO) > 0)
    {
        simplex[1] = A;
        *count = 2;
        *dir = perpAC;
        return false;
    }

    // Origin inside triangle
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
    Vec2 simplex[3];
    int count = 0;

    Vec2 direction = vec_sub(findCenter(A), findCenter(B));
    if (direction.x == 0 && direction.y == 0)
        direction = (Vec2){1, 0};

    simplex[count++] = vec_sub(support(A, direction), support(B, vec_neg(direction)));

    direction = vec_neg(simplex[0]);

    while (true)
    {
        Vec2 Anew = vec_sub(support(A, direction), support(B, vec_neg(direction)));

        if (vec_dot(Anew, direction) <= 0)
            return false;

        simplex[count++] = Anew;

        if (handleSimplex(simplex, &count, &direction))
            return true;
    }
}