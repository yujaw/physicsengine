// draw_shapes.c
#include "draw_shapes.h"
#include "init_shapes.h"
#include <math.h>

#define PI 3.14159265358979323846f

static GLuint VAO_global;
static GLuint VBO_global;
static GLint colorLocation_global;

void initDraw(GLuint vao, GLuint vbo, GLint colorLoc)
{
    VAO_global = vao;
    VBO_global = vbo;
    colorLocation_global = colorLoc;
}

void setColor(float r, float g, float b, float a)
{
    glUniform4f(colorLocation_global, r, g, b, a);
}

void drawQuad(float x1, float y1, float x2, float y2,
              float x3, float y3, float x4, float y4)
{
    float v[] = {
        x1, y1, x2, y2, x3, y3,
        x1, y1, x3, y3, x4, y4};

    glBindVertexArray(VAO_global);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_global);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// void drawCircle(float x, float y, float r)
// {
//     int steps = ceilf(r * 256);
//     float verts[(steps + 2) * 2];

//     verts[0] = x;
//     verts[1] = y;

//     float angStep = 2 * PI / steps;

//     for (int i = 0; i <= steps; i++) {
//         float t = i * angStep;
//         verts[2 + 2*i] = x + cosf(t) * r;
//         verts[2 + 2*i + 1] = y + sinf(t) * r;
//     }

//     glBindVertexArray(VAO_global);
//     glBindBuffer(GL_ARRAY_BUFFER, VBO_global);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
//     glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);
//     glEnableVertexAttribArray(0);
//     glDrawArrays(GL_TRIANGLE_FAN, 0, steps + 2);
// }

void drawCircle(Circle circle)
{
    int steps = ceilf(circle.r * 200.0f);

    if (steps < 16) steps = 16;
    if (steps > 64) steps = 64; 

    float verts[(steps + 2) * 2];


    verts[0] = circle.x;
    verts[1] = circle.y;

    float angStep = 2 * PI / steps;

    for (int i = 0; i <= steps; i++)
    {
        float t = i * angStep;
        verts[2 + 2 * i] = circle.x + cosf(t) * circle.r;
        verts[2 + 2 * i + 1] = circle.y + sinf(t) * circle.r;
    }

    glBindVertexArray(VAO_global);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_global);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, steps + 2);
}

// void drawLine(float x1, float y1, float x2, float y2)
// {
//     float v[] = { x1, y1, x2, y2 };

//     glBindVertexArray(VAO_global);
//     glBindBuffer(GL_ARRAY_BUFFER, VBO_global);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
//     glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);
//     glEnableVertexAttribArray(0);
//     glDrawArrays(GL_LINES, 0, 2);
// }

void drawLine(Line line)
{
    float v[] = {line.x1, line.y1, line.x2, line.y2};

    glBindVertexArray(VAO_global);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_global);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_LINES, 0, 2);
}