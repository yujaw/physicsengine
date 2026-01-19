#include "kdtree.h"

KDNode *kd_insert(KDNode *node, Vec2 pos, Body *body, int depth)
{
    if (node == NULL)
    {
        KDNode *newNode = malloc(sizeof(KDNode));
        newNode->pos = pos;
        newNode->body = body;
        newNode->left = NULL;
        newNode->right = NULL;
        return newNode;
    }

    int axis = depth % 2;

    if ((axis == 0 && pos.x < node->pos.x) ||
        (axis == 1 && pos.y < node->pos.y))
    {
        node->left = kd_insert(node->left, pos, body, depth + 1);
    }
    else
    {
        node->right = kd_insert(node->right, pos, body, depth + 1);
    }

    return node;
}

void kd_search_range(KDNode* node, Vec2 point, float radius, int depth, Body** out, int* count)
{
    if (node == NULL) return;

    float dx = node->pos.x - point.x;
    float dy = node->pos.y - point.y;
    float dist2 = dx*dx + dy*dy;
    float r2 = radius * radius;

    if (dist2 <= r2)
    {
        out[*count] = node->body;
        (*count)++;
    }

    int axis = depth % 2;
    float delta = (axis == 0) ? dx : dy;

    if (delta > -radius)
        kd_search_range(node->left, point, radius, depth + 1, out, count);

    if (delta < radius)
        kd_search_range(node->right, point, radius, depth + 1, out, count);
}

void kd_free(KDNode* node)
{
    if (node == NULL) return;

    kd_free(node->left);
    kd_free(node->right);
    free(node);
}