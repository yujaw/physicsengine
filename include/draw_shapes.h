#include "init_shapes.h"

// draw_shapes.h
#ifndef DRAW_SHAPES_H
#define DRAW_SHAPES_H

#include <glad/glad.h>

// You must give these from main.c
void initDraw(GLuint vao, GLuint vbo, GLint colorLoc);

// Drawing utilities
void setColor(float r, float g, float b, float a);
void drawQuad(float x1, float y1, float x2, float y2,
              float x3, float y3, float x4, float y4);
// void drawCircle(float x, float y, float r);
void drawCircle (Circle circle);
// void drawLine(float x1, float y1, float x2, float y2);
void drawLine(Line line);

#endif