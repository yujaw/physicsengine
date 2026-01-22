#include <stdio.h>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "draw_shapes.h"
#include "init_shapes.h"
#include "collision.h"
#include "vectors.h"
#include "fps.h"
#include "movement.h"
#include "kdtree.h"

#define gravity 1.0f

GLFWwindow *window;
GLuint VAO, VBO;
int colorLocation;

int screenHeight = 800;
int screenWidth = 1000;
float screenZoom = 1.0f;

float mousePosX;
float mousePosY;

float left = -1.0f;
float right = 1.0f;
float bottom = -1.0f;
float top = 1.0f;

KDNode *node;

const char *vertexShaderSource =
    "#version 330 core\n"
    "uniform float uAspect;\n"
    "uniform float uZoom;\n"
    "layout (location = 0) in vec2 aPos;\n"
    "void main() {\n"
    "    // Scale X by aspect to match window ratio\n"
    "    gl_Position = vec4(aPos.x * uZoom, aPos.y / uAspect * uZoom, 0.0, 1.0);\n"
    "}\0";

const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 uColor;\n"
    "void main() {\n"
    "    FragColor = uColor;\n"
    "}\0";

void scrollCallback(GLFWwindow *window, double offsetX, double offsetY)
{
    if (offsetY > 0)
        screenZoom *= 1.1f;
    else
        screenZoom /= 1.1f;

    if (screenZoom < 0.1f)
        screenZoom = 0.1f;
    if (screenZoom > 10.0f)
        screenZoom = 10.0f;
}

void cursorPositionCallback(GLFWwindow *window, double posX, double posY)
{
    // Nothing to See Here!
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    // Nothing to See Here!
}

void keyCallBack(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        // rotate(line, 0.02f);
    }
    if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        // rotate(line, -0.02f);
    }
    if (key == GLFW_KEY_W)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            // move(line, 0.0f, 0.01f);
        }
    }
    if (key == GLFW_KEY_S)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            // move(line, 0.0f, -0.01f);
        }
    }
    if (key == GLFW_KEY_A)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            // move(line, -0.01f, 0.0f);
        }
    }
    if (key == GLFW_KEY_D)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            // move(line, 0.01f, 0.0f);
        }
    }
}

void handleCollisionResponse(Body *a, Body *b, CollisionResult *result)
{
    if (!result->hit)
        return;

    float percent = 1.0f;
    float slop = 0.001f;
    float separationBias = 0.01f; // Small bias to prevent kissing

    float correctionDepth = fmaxf(result->depth - slop, 0.0f) * percent + separationBias;

    bool aDynamic = a->isDynamic;
    bool bDynamic = b->isDynamic;

    float correctionA = 0.001f;
    float correctionB = 0.001f;

    if (aDynamic && bDynamic)
    {
        correctionA = correctionDepth * 0.5f;
        correctionB = correctionDepth * 0.5f;
    }
    else if (aDynamic)
    {
        correctionA = correctionDepth;
    }
    else if (bDynamic)
    {
        correctionB = correctionDepth;
    }

    if (aDynamic)
    {
        if (a->type == SHAPE_ELLIPSE)
        {
            a->data.ellipse.pos.x += result->normal.x * correctionA;
            a->data.ellipse.pos.y += result->normal.y * correctionA;
        }
        else if (a->type == SHAPE_POLYGON)
        {
            for (int v = 0; v < a->data.polygon.numVertices; v++)
            {
                a->data.polygon.vertices[v].x += result->normal.x * correctionA;
                a->data.polygon.vertices[v].y += result->normal.y * correctionA;
            }
        }
        else if (a->type == SHAPE_LINE)
        {
            a->data.line.vertices[0].x += result->normal.x * correctionA;
            a->data.line.vertices[0].y += result->normal.y * correctionA;
            a->data.line.vertices[1].x += result->normal.x * correctionA;
            a->data.line.vertices[1].y += result->normal.y * correctionA;
        }
    }

    if (bDynamic)
    {
        if (b->type == SHAPE_ELLIPSE)
        {
            b->data.ellipse.pos.x -= result->normal.x * correctionB;
            b->data.ellipse.pos.y -= result->normal.y * correctionB;
        }
        else if (b->type == SHAPE_POLYGON)
        {
            for (int v = 0; v < b->data.polygon.numVertices; v++)
            {
                b->data.polygon.vertices[v].x -= result->normal.x * correctionB;
                b->data.polygon.vertices[v].y -= result->normal.y * correctionB;
            }
        }
        else if (b->type == SHAPE_LINE)
        {
            b->data.line.vertices[0].x -= result->normal.x * correctionB;
            b->data.line.vertices[0].y -= result->normal.y * correctionB;
            b->data.line.vertices[1].x -= result->normal.x * correctionB;
            b->data.line.vertices[1].y -= result->normal.y * correctionB;
        }
    }

    if (aDynamic)
    {
        float vn = vec_dot(a->velocity, result->normal);
        if (vn < 0)
        {
            Vec2 impulse = vec_scale(result->normal, -vn * (1.0f + a->restitution));
            a->velocity = vec_add(a->velocity, impulse);
        }
    }

    if (bDynamic)
    {
        Vec2 negNormal = vec_neg(result->normal);
        float vn = vec_dot(b->velocity, negNormal);
        if (vn < 0)
        {
            Vec2 impulse = vec_scale(negNormal, -vn * (1.0f + b->restitution));
            b->velocity = vec_add(b->velocity, impulse);
        }
    }
}

void checkShapeCollision(Body *a, Body *b)
{
    if (b->type == SHAPE_POLYGON)
    {
        bool isConvex = polygonIsConvex(b->data.polygon.vertices, b->data.polygon.numVertices);

        if (isConvex)
        {
            if (b->filled && isInsideShape(a, b))
            {
                return;
            }

            CollisionResult result;
            if (checkCollision(a, b, &result))
            {
                handleCollisionResponse(a, b, &result);
            }
        }
        else
        {
            Body **triangles = malloc(sizeof(Body *) * (b->data.polygon.numVertices - 2));
            int triangle_count;
            decompose(b, triangles, &triangle_count);

            if (b->filled && isInsideShape(a, b))
            {
                for (int t = 0; t < triangle_count; t++)
                {
                    free(triangles[t]->data.polygon.vertices);
                    free(triangles[t]);
                }
                free(triangles);
                return;
            }

            for (int t = 0; t < triangle_count; t++)
            {
                CollisionResult result;
                if (checkCollision(a, triangles[t], &result))
                {
                    handleCollisionResponse(a, b, &result);
                }
            }

            for (int t = 0; t < triangle_count; t++)
            {
                free(triangles[t]->data.polygon.vertices);
                free(triangles[t]);
            }
            free(triangles);
        }
    }
    else if (b->type == SHAPE_ELLIPSE)
    {
        if (b->filled && isInsideShape(a, b))
        {
            return;
        }

        CollisionResult result;
        if (checkCollision(a, b, &result))
        {
            handleCollisionResponse(a, b, &result);
        }
    }
    else
    {
        CollisionResult result;
        if (checkCollision(a, b, &result))
        {
            handleCollisionResponse(a, b, &result);
        }
    }
}

int main(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(screenWidth, screenHeight, "Physics Simulator", NULL, NULL);

    if (window == NULL)
    {
        printf("Failed to initialize window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, keyCallBack);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetScrollCallback(window, scrollCallback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glUseProgram(shaderProgram);
    colorLocation = glGetUniformLocation(shaderProgram, "uColor");

    initDraw(VAO, VBO, colorLocation);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    for (int i = 0; i < 30; i++)
    {
        Vec2 pos = {(float)(rand() % 200 - 100) / 100.0f,
                    (float)(rand() % 200 - 100) / 100.0f};
        Vec2 radius = {0.025f, 0.025f};

        Body *b = init_ellipse(pos, radius, COLOR_RED);
        b->filled = true;
        b->isDynamic = true;
    }

    for (int i = 0; i < 20; i++)
    {
        Vec2 pos = {(float)(rand() % 200 - 100) / 100.0f,
                    (float)(rand() % 200 - 100) / 100.0f};
        Vec2 radius = {0.025f, 0.025f};

        Body *b = init_ellipse(pos, radius, COLOR_YELLOW);
        b->filled = false;
        b->isDynamic = true;
    }

    Body *polygon1 = init_polygon((Vec2[]){{-0.5f, -0.5f}, {0.5f, -0.5f}, {0.5f, 0.5f}, {0.0f, 0.25f}, {-0.5f, 0.5f}}, 5, COLOR_BLUE);
    polygon1->isDynamic = false;
    polygon1->filled = true;

    Body *polygon2 = init_polygon((Vec2[]){{0.6f, -0.3f}, {0.9f, -0.3f}, {0.75f, 0.2f}}, 3, COLOR_GREEN);
    polygon2->isDynamic = false;
    polygon2->filled = false;

    Body *line1 = init_line((Vec2){-0.8f, -0.8f}, (Vec2){-0.6f, 0.8f}, COLOR_CYAN);
    line1->isDynamic = false;

    Body *line2 = init_line((Vec2){0.6f, 0.6f}, (Vec2){0.9f, 0.9f}, COLOR_MAGENTA);
    line2->isDynamic = false;

    while (!glfwWindowShouldClose(window))
    {
        double currentFPS = calculateFPS(glfwGetTime());
        char title[256];
        sprintf(title, "Physics Simulator - FPS: %.1f", currentFPS);
        glfwSetWindowTitle(window, title);

        kd_free(node);
        node = NULL;

        for (int i = 0; i < body_count; i++)
        {
            Body *b = &bodies[i];
            Vec2 center = findCenter(b);
            node = kd_insert(node, center, b, 0);
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (int i = 0; i < body_count; i++)
        {
            Body *a = &bodies[i];

            for (int j = i + 1; j < body_count; j++)
            {
                Body *b = &bodies[j];

                if (!a->isDynamic && !b->isDynamic)
                    continue;

                if (a->type == SHAPE_POLYGON)
                {
                    bool isConvex = polygonIsConvex(a->data.polygon.vertices, a->data.polygon.numVertices);

                    if (!isConvex)
                    {
                        if (a->filled && isInsideShape(b, a))
                        {
                            continue;
                        }

                        Body **triangles = malloc(sizeof(Body *) * (a->data.polygon.numVertices - 2));
                        int triangle_count;
                        decompose(a, triangles, &triangle_count);

                        for (int t = 0; t < triangle_count; t++)
                        {
                            checkShapeCollision(triangles[t], b);
                        }

                        for (int t = 0; t < triangle_count; t++)
                        {
                            free(triangles[t]->data.polygon.vertices);
                            free(triangles[t]);
                        }
                        free(triangles);
                        continue;
                    }
                    else
                    {
                        if (a->filled && isInsideShape(b, a))
                        {
                            continue;
                        }
                    }
                }
                else if (a->type == SHAPE_ELLIPSE)
                {
                    if (a->filled && isInsideShape(b, a))
                    {
                        continue;
                    }
                }

                checkShapeCollision(a, b);
            }
        }

        glUseProgram(shaderProgram);

        float dt = 1.0f / 60.0f;
        float scaledGravity = 0.001f;

        for (int i = 0; i < body_count; i++)
        {
            Body *b = &bodies[i];

            if (b->isDynamic)
            {
                b->velocity.y -= scaledGravity * dt;

                if (b->type == SHAPE_ELLIPSE)
                {
                    b->data.ellipse.pos.x += b->velocity.x * dt;
                    b->data.ellipse.pos.y += b->velocity.y * dt;

                    float radiusX = b->data.ellipse.r.x;
                    float radiusY = b->data.ellipse.r.y;

                    if (b->data.ellipse.pos.x - radiusX < left)
                    {
                        b->data.ellipse.pos.x = left + radiusX;
                        b->velocity.x *= -b->restitution;
                    }
                    if (b->data.ellipse.pos.x + radiusX > right)
                    {
                        b->data.ellipse.pos.x = right - radiusX;
                        b->velocity.x *= -b->restitution;
                    }
                    if (b->data.ellipse.pos.y - radiusY < bottom)
                    {
                        b->data.ellipse.pos.y = bottom + radiusY;
                        b->velocity.y *= -b->restitution;
                    }
                    if (b->data.ellipse.pos.y + radiusY > top)
                    {
                        b->data.ellipse.pos.y = top - radiusY;
                        b->velocity.y *= -b->restitution;
                    }
                }
                else if (b->type == SHAPE_POLYGON)
                {
                    for (int v = 0; v < b->data.polygon.numVertices; v++)
                    {
                        b->data.polygon.vertices[v].x += b->velocity.x * dt;
                        b->data.polygon.vertices[v].y += b->velocity.y * dt;
                    }
                }
                else if (b->type == SHAPE_LINE)
                {
                    b->data.line.vertices[0].x += b->velocity.x * dt;
                    b->data.line.vertices[0].y += b->velocity.y * dt;
                    b->data.line.vertices[1].x += b->velocity.x * dt;
                    b->data.line.vertices[1].y += b->velocity.y * dt;
                }
            }

            draw(b);
        }

        int frameBufferWidth, frameBufferHeight;
        glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

        float aspect = (float)frameBufferHeight / (float)frameBufferWidth;
        int aspectLoc = glGetUniformLocation(shaderProgram, "uAspect");
        glUniform1f(aspectLoc, aspect);

        int zoomLoc = glGetUniformLocation(shaderProgram, "uZoom");
        glUniform1f(zoomLoc, screenZoom);

        glfwSwapBuffers(window);
        glfwSwapInterval(0);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}