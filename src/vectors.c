#include "vectors.h"
#include <stdio.h>
#include <math.h>

float vec_dot(Vec2 a, Vec2 b)
{
    return a.x * b.x + a.y * b.y;
}

Vec2 vec_sub(Vec2 a, Vec2 b)
{
    return (Vec2){a.x - b.x, a.y - b.y};
}

float vec_cross(Vec2 a, Vec2 b)
{
    return a.x * b.y - a.y * b.x;
}

void vec_print(Vec2 vec)
{
    printf("%f\t%f\n", vec.x, vec.y);
}

Vec2 vec_neg(Vec2 vec)
{
    return (Vec2){-vec.x, -vec.y};
}

Vec2 vec_normalize(Vec2 vec)
{
    float mag = sqrtf(vec.x * vec.x + vec.y * vec.y);

    if (mag < 1e-8f)
        return (Vec2){0.0f, 0.0f};

    return (Vec2){vec.x / mag, vec.y / mag};
}

Vec2 vec_tripleProduct(Vec2 a, Vec2 b, Vec2 c)
{
    float ac = vec_dot(a, c);
    float bc = vec_dot(b, c);

    return (Vec2){
        b.x * ac - a.x * bc,
        b.y * ac - a.y * bc};
}

float vec_length(Vec2 vec)
{
    return sqrtf(vec.x * vec.x + vec.y * vec.y);
}

bool vec_cmp(Vec2 a, Vec2 b)
{
    return fabs(a.x - b.x) < 1e-6f && fabs(a.y - b.y) < 1e-6f;
}

Vec2 vec_scale(Vec2 v, float scalar)
{
    return (Vec2){v.x * scalar, v.y * scalar};
}

Vec2 vec_add(Vec2 a, Vec2 b)
{
    return (Vec2){a.x + b.x, a.y + b.y};
}