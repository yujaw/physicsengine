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

void setColor(Color color)
{
    glUniform4f(colorLocation_global, color.r, color.g, color.b, color.a);
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

void drawCircle(Body body)
{
    if (body.type != SHAPE_CIRCLE)
        return;

    int steps = ceilf(body.data.circle.r * 200.0f);
    if (steps < 24)
        steps = 24;
    if (steps > 64)
        steps = 64;

    float angStep = 2.0f * PI / steps;

    setColor(body.color);

    // ---- FILLED CIRCLE ----
    if (body.filled)
    {
        float verts[(steps + 2) * 2];

        verts[0] = body.data.circle.x;
        verts[1] = body.data.circle.y;

        for (int i = 0; i <= steps; i++)
        {
            float t = i * angStep;
            verts[2 + 2 * i] = body.data.circle.x + cosf(t) * body.data.circle.r;
            verts[2 + 2 * i + 1] = body.data.circle.y + sinf(t) * body.data.circle.r;
        }

        glBindVertexArray(VAO_global);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_global);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, steps + 2);
    }
    // ---- OUTLINE CIRCLE ----
    else
    {
        float verts[(steps + 1) * 2];

        for (int i = 0; i <= steps; i++)
        {
            float t = i * angStep;
            verts[2 * i] = body.data.circle.x + cosf(t) * body.data.circle.r;
            verts[2 * i + 1] = body.data.circle.y + sinf(t) * body.data.circle.r;
        }

        glBindVertexArray(VAO_global);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_global);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_LINE_LOOP, 0, steps);
    }
}

void drawLine(Body body)
{
    float v[] = {body.data.line.vertices[0].x, body.data.line.vertices[0].y, body.data.line.vertices[1].x, body.data.line.vertices[1].y};
    setColor(body.color);
    glBindVertexArray(VAO_global);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_global);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_LINES, 0, 2);
}