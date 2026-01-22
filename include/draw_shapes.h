#ifndef DRAW_SHAPES_H
#define DRAW_SHAPES_H

#include "init_shapes.h"
#include <glad/glad.h>

void initDraw(GLuint vao, GLuint vbo, GLint colorLoc);
void setColor(Color color);
void drawPolygon(Body *body);
void drawLine(Body *body);
void drawEllipse(Body *body);
void drawAllShapes();
void draw(Body *body);

#endif