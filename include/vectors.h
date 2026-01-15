#ifndef VECTORS_H
#define VECTORS_H

#include <stdbool.h>

typedef struct
{
    float x;
    float y;
} Vec2;

float vec_dot(Vec2 a, Vec2 b);
Vec2 vec_sub(Vec2 a, Vec2 b);
float vec_cross(Vec2 a, Vec2 b);
void vec_print(Vec2 vec);
Vec2 vec_neg(Vec2 vec);
Vec2 vec_normalize(Vec2 vec);
Vec2 vec_tripleProduct(Vec2 A, Vec2 B, Vec2 C);
float vec_length(Vec2 vec);
bool vec_cmp(Vec2 a, Vec2 b);
Vec2 vec_scale(Vec2 v, float s);
Vec2 vec_add(Vec2 a, Vec2 b);

#endif