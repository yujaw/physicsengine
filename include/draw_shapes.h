#include "init_shapes.h"
#include <glad/glad.h>

#ifndef DRAW_SHAPES_H
#define DRAW_SHAPES_H

void initDraw(GLuint vao, GLuint vbo, GLint colorLoc);
void setColor(Color color);
void drawPolygon(Body *body);
void drawLine(Body *body);
void drawEllipse(Body *body);
void drawAllShapes();

#endif