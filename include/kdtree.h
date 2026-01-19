#include "init_shapes.h"
#include <stdlib.h>

#ifndef KDTREE_H
#define KDTREE_H

typedef struct KDNode
{
    Vec2 pos;
    Body *body;
    struct KDNode *left;
    struct KDNode *right;
} KDNode;

KDNode *kd_insert(KDNode *node, Vec2 pos, Body *body, int depth);
void kd_search_range(KDNode *node, Vec2 point, float radius, int depth, Body **out, int *count);
void kd_free(KDNode *node);

#endif