#include "init_shapes.h"

Circle init_circle(int id, float x, float y, float r)
{
    Circle c = {id, x, y, r};

    return c;
}

Line init_line (int id, float x1, float y1, float x2, float y2) {
    Line l = {id, x1, y1, x2, y2};

    return l;
}