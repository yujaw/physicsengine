// draw_shapes.c
#include "draw_shapes.h"
#include "init_shapes.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

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

void drawEllipse(Body *body)
{
    if (body->type != SHAPE_ELLIPSE)
        return;

    int steps = (int)ceilf(body->data.ellipse.r.x * 200.0f);
    if (steps < 24)
        steps = 24;
    if (steps > 64)
        steps = 64;

    float angStep = 2.0f * PI / steps;

    setColor(body->color);

    if (body->filled)
    {
        float verts[(steps + 2) * 2];

        verts[0] = body->data.ellipse.pos.x;
        verts[1] = body->data.ellipse.pos.y;

        for (int i = 0; i <= steps; i++)
        {
            float t = i * angStep;
            verts[2 + 2 * i] = body->data.ellipse.pos.x + cosf(t) * body->data.ellipse.r.x;
            verts[2 + 2 * i + 1] = body->data.ellipse.pos.y + sinf(t) * body->data.ellipse.r.y;
        }

        glBindVertexArray(VAO_global);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_global);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, steps + 2);
    }
    else
    {
        float verts[(steps + 1) * 2];

        for (int i = 0; i <= steps; i++)
        {
            float t = i * angStep;
            verts[2 * i] = body->data.ellipse.pos.x + cosf(t) * body->data.ellipse.r.x;
            verts[2 * i + 1] = body->data.ellipse.pos.y + sinf(t) * body->data.ellipse.r.y;
        }

        glBindVertexArray(VAO_global);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_global);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_LINE_LOOP, 0, steps);
    }
}

void drawLine(Body *body)
{
    float v[] = {body->data.line.vertices[0].x, body->data.line.vertices[0].y, body->data.line.vertices[1].x, body->data.line.vertices[1].y};

    setColor(body->color);

    glBindVertexArray(VAO_global);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_global);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_LINES, 0, 2);
}

void drawPolygon(Body *body)
{
    int numVertices = body->data.polygon.numVertices;
    Vec2 *vertices = body->data.polygon.vertices;

    float *v = (float *)malloc(numVertices * 2 * sizeof(float));

    for (int i = 0; i < numVertices; i++)
    {
        v[i * 2] = vertices[i].x;
        v[i * 2 + 1] = vertices[i].y;
    }

    setColor(body->color);

    if (body->filled)
    {
        glBindVertexArray(VAO_global);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_global);
        glBufferData(GL_ARRAY_BUFFER, numVertices * 2 * sizeof(float), v, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        if (numVertices == 3)
            glDrawArrays(GL_TRIANGLES, 0, 3);
        else
            glDrawArrays(GL_TRIANGLE_FAN, 0, numVertices);
    }
    else
    {
        glBindVertexArray(VAO_global);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_global);
        glBufferData(GL_ARRAY_BUFFER, numVertices * 2 * sizeof(float), v, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glDrawArrays(GL_LINE_LOOP, 0, numVertices);
    }

    free(v);
}

void drawAllShapes()
{
    for (int i = 0; i < body_count; i++)
    {
        if (bodies[i].type == SHAPE_ELLIPSE)
        {
            drawEllipse(&bodies[i]);
        }
        else if (bodies[i].type == SHAPE_LINE)
        {
            drawLine(&bodies[i]);
        }
        else if (bodies[i].type == SHAPE_POLYGON)
        {
            drawPolygon(&bodies[i]);
        }
    }
}