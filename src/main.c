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
#include "dynamic.h"

#define gravity 1.0f

GLFWwindow *window;
GLuint VAO, VBO;
int colorLocation;

Body *polygon1;

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
        rotate(polygon1, 0.02f);
    }
    if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        rotate(polygon1, -0.02f);
    }
    if (key == GLFW_KEY_W)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            move(polygon1, 0.0f, 0.01f);
        }
    }
    if (key == GLFW_KEY_S)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            move(polygon1, 0.0f, -0.01f);
        }
    }
    if (key == GLFW_KEY_A)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            move(polygon1, -0.01f, 0.0f);
        }
    }
    if (key == GLFW_KEY_D)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            move(polygon1, 0.01f, 0.0f);
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
                    1.0f};
        Vec2 radius = {0.025f, 0.025f};

        Body *b = init_ellipse(pos, radius, COLOR_RED);
        b->filled = true;
        b->isDynamic = true;
    }

    polygon1 = init_polygon((Vec2[]){{-0.5f, -0.5f}, {0.5f, -0.5f}, {0.5f, 0.5f}, {0.0f, 0.25f}, {-0.5f, 0.5f}}, 5, COLOR_BLUE);
    polygon1->isDynamic = false;
    polygon1->filled = true;

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

            accelerate(b, (Vec2){0.0f, -1E-7f});
        }

        

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        int frameBufferWidth, frameBufferHeight;
        glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

        float aspect = (float)frameBufferHeight / (float)frameBufferWidth;
        int aspectLoc = glGetUniformLocation(shaderProgram, "uAspect");
        glUniform1f(aspectLoc, aspect);

        int zoomLoc = glGetUniformLocation(shaderProgram, "uZoom");
        glUniform1f(zoomLoc, screenZoom);

        drawAllShapes();

        glfwSwapBuffers(window);
        glfwSwapInterval(0);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}