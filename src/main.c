#include <stdio.h>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "draw_shapes.h"
#include "init_shapes.h"
#include "collision.h"
#include "vectors.h"

#define pi 3.1415926535897932384626433f
#define gravity 0.00005f

double lastTime = 0.0;
int frameCount = 0;
double currentFPS = 0.0;
double deltaTime = 0.0;
double lastFrameTime = 0.0;

int body_count = 0;

GLuint VAO, VBO;
int colorLocation;

GLFWwindow *window;

int screenHeight = 800;
int screenWidth = 1000;

Vec2 velocity = {0.0f, 0.0f};
Vec2 acceleration = {0.0f, 0.0f};

// void addLine(float x1, float y1, float x2, float y2);

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

void calculateFPS(void)
{
    double currentTime = glfwGetTime();
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    frameCount++;
    if (currentTime - lastTime >= 1.0)
    {
        currentFPS = frameCount / (currentTime - lastTime);

        // Display in window title
        char title[256];
        sprintf(title, "Physics Simulator - FPS: %.1f", currentFPS);
        glfwSetWindowTitle(window, title);

        frameCount = 0;
        lastTime = currentTime;
    }
}

float zoom = 1.0f; // 1.0 = normal scale

void scrollCallback(GLFWwindow *window, double offsetX, double offsetY)
{
    if (offsetY > 0)
        zoom *= 1.1f; // zoom in
    else
        zoom /= 1.1f; // zoom out

    if (zoom < 0.1f)
        zoom = 0.1f;
    if (zoom > 10.0f)
        zoom = 10.0f;
}

Vec2 mousePos = {0.0f, 0.0f};

void cursorPositionCallback(GLFWwindow *window, double posX, double posY)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    float aspect = (float)height / (float)width;

    // Map to [-1,1] in x, then scale y according to aspect
    mousePos.x = (float)posX / width * 4 - 1.0f;
    mousePos.y = -((float)posY / height * 4 - 1.0f);
    mousePos.x /= zoom;
    mousePos.y /= zoom;
    mousePos.y *= aspect;
}

bool leftButtonDown = false;

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            leftButtonDown = true;
        }
        else
        {
            leftButtonDown = false;
        }
    }
}

void keyCallBack(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    acceleration = (Vec2){0.0f, 0.0f};
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_W)
        {
            acceleration.y = 0.001f;
        }
        if (key == GLFW_KEY_S)
        {
            acceleration.y = -0.001f;
        }
        if (key == GLFW_KEY_A)
        {
            acceleration.x = -0.001f;
        }
        if (key == GLFW_KEY_D)
        {
            acceleration.x = 0.001f;
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

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, keyCallBack);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetScrollCallback(window, scrollCallback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

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

    init_line((Vec2){-0.0f, 0.4f}, (Vec2){0.8f, 0.4f}, COLOR_BLUE, 0.0f, false);
    init_line((Vec2){-0.8f, -0.4f}, (Vec2){0.0f, -0.4f}, COLOR_BLUE, 0.0f, false);
    init_line((Vec2){-0.8f, -0.4f}, (Vec2){0.0f, 0.4f}, COLOR_BLUE, 0.0f, false);
    init_line((Vec2){0.0f, -0.4f}, (Vec2){0.8f, 0.4f}, COLOR_BLUE, 0.0f, false);

    init_circle((Vec2){0.0f, 0.0f}, 0.05f, false, COLOR_BLUE, 0.0f, true);
    init_circle((Vec2){-0.15f, 0.0f}, 0.05f, true, COLOR_BLUE, 0.0f, true);

    while (!glfwWindowShouldClose(window))
    {
        calculateFPS();

        glClearColor(0.0f, 0.0f, 0.0f, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        int frameBufferWidth, frameBufferHeight;
        glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

        float aspect = (float)frameBufferHeight / (float)frameBufferWidth;
        int aspectLoc = glGetUniformLocation(shaderProgram, "uAspect");
        glUniform1f(aspectLoc, aspect);

        int zoomLoc = glGetUniformLocation(shaderProgram, "uZoom");
        glUniform1f(zoomLoc, zoom);

        velocity.x += acceleration.x;
        velocity.y += acceleration.y;

        for (int i = 0; i < body_count; i++)
        {
            if (bodies[i].isDynamic)
            {
                // Update velocity with acceleration and gravity
                bodies[i].velocity.x += bodies[i].acceleration.x * deltaTime;
                bodies[i].velocity.y += (bodies[i].acceleration.y - gravity) * deltaTime;

                if (bodies[i].type == SHAPE_CIRCLE)
                {
                    // Update position based on velocity
                    bodies[i].data.circle.x += bodies[i].velocity.x * deltaTime;
                    bodies[i].data.circle.y += bodies[i].velocity.y * deltaTime;
                }
                if (bodies[i].type == SHAPE_LINE)
                {
                    bodies[i].data.line.vertices[0].x += bodies[i].velocity.x * deltaTime;
                    bodies[i].data.line.vertices[1].x += bodies[i].velocity.x * deltaTime;
                    bodies[i].data.line.vertices[0].y += bodies[i].velocity.y * deltaTime;
                    bodies[i].data.line.vertices[1].y += bodies[i].velocity.y * deltaTime;
                }
            }

            if (bodies[i].type == SHAPE_CIRCLE)
            {
                drawCircle(bodies[i]);
            }
            else if (bodies[i].type == SHAPE_LINE)
            {
                drawLine(bodies[i]);
            }
        }

        static bool prevLeftDown = false;

        if (leftButtonDown && !prevLeftDown)
        {
            init_circle(mousePos, 0.05f, false, COLOR_RED, 0.0f, true);
        }
        prevLeftDown = leftButtonDown;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}