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

    for (int i = 0; i < 200; i++)
    {
        Vec2 pos = {(float)(rand() % 200 - 100) / 100.0f,
                    (float)(rand() % 200 - 100) / 100.0f};
        Vec2 radius = {0.05f, 0.05f};

        Body *b = init_ellipse(pos, radius, COLOR_RED);
        b->filled = true;
        b->isDynamic = true;
    }

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

        Body *candidates[64];

        for (int i = 0; i < body_count; i++)
        {
            Body *b = &bodies[i];
            Vec2 center = findCenter(b);

            int count = 0;
            Body *candidates[64];
            float searchRadius = b->data.ellipse.r.x * 2.0f;
            kd_search_range(node, center, searchRadius, 0, candidates, &count);

            for (int j = 0; j < count; j++)
            {
                Body *other = candidates[j];

                if (other->id <= b->id)
                    continue;

                CollisionResult result;
                if (checkCollision(b, other, &result))
                {
                    if (!result.hit)
                        continue;

                    float percent = 0.95f;
                    float slop = 0.000001f;

                    float correctionDepth = fmaxf(result.depth - slop, 0.0f) * percent;

                    float correction = correctionDepth / 2.0f;

                    // Apply positional correction along the normal
                    b->data.ellipse.pos.x -= result.normal.x * correction;
                    b->data.ellipse.pos.y -= result.normal.y * correction;
                    other->data.ellipse.pos.x += result.normal.x * correction;
                    other->data.ellipse.pos.y += result.normal.y * correction;
                }
            }
        }

        glUseProgram(shaderProgram);

        float dt = 1.0f / 60.0f;
        float scaledGravity = 0.001f; // tune as needed

        for (int i = 0; i < body_count; i++)
        {
            Body *b = &bodies[i];
            if (!b->isDynamic)
                continue;

            b->velocity.y -= scaledGravity * dt;

            b->data.ellipse.pos.x += b->velocity.x * dt;
            b->data.ellipse.pos.y += b->velocity.y * dt;

            float radiusX = b->data.ellipse.r.x;
            float radiusY = b->data.ellipse.r.y;

            if (b->data.ellipse.pos.x - radiusX < left)
            {
                b->data.ellipse.pos.x = left + radiusX;
                b->velocity.x *= -1.0f;
            }
            if (b->data.ellipse.pos.x + radiusX > right)
            {
                b->data.ellipse.pos.x = right - radiusX;
                b->velocity.x *= -1.0f;
            }
            if (b->data.ellipse.pos.y - radiusY < bottom)
            {
                b->data.ellipse.pos.y = bottom + radiusY;
                b->velocity.y *= -1.0f;
            }
            if (b->data.ellipse.pos.y + radiusY > top)
            {
                b->data.ellipse.pos.y = top - radiusY;
                b->velocity.y *= -1.0f;
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