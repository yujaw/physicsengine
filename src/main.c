#include <stdio.h>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "draw_shapes.h"
#include "init_shapes.h"

#define pi 3.1415926535897932384626433f
#define MAX_SHAPES 100
#define restitution 0.8f
#define friction 0.9f
#define gravity 0.00005f

GLuint VAO, VBO;
int colorLocation;

GLFWwindow *window;

int screenHeight = 800;
int screenWidth = 1600;

int circle_count = 0;
int line_count = 0;

Circle circles[MAX_SHAPES];
Line lines[MAX_SHAPES];

typedef struct
{
    float x;
    float y;
} Vec2;

Vec2 velocity = {0.0f, 0.0f};
Vec2 acceleration = {0.0f, 0.0f};

void addLine(float x1, float y1, float x2, float y2);

const char *vertexShaderSource =
    "#version 330 core\n"
    "uniform float uAspect;\n"
    "layout (location = 0) in vec2 aPos;\n"
    "void main() {\n"
    "    gl_Position = vec4(aPos.x, aPos.y / uAspect, 0.0, 1.0);\n" // Fixed!
    "}\0";

const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 uColor;\n"
    "void main() {\n"
    "    FragColor = uColor;\n"
    "}\0";

Vec2 findMidpoints(Line line)
{
    // midpoint of line
    float mx = (line.x1 + line.x2) / 2.0f;
    float my = (line.y1 + line.y2) / 2.0f;

    Vec2 midpoint = (Vec2){mx, my};

    return midpoint;
}

Vec2 findNormal(Line line)
{
    Vec2 direction = {line.x2 - line.x1, line.y2 - line.y1};

    float mag = sqrtf(pow(direction.x, 2) + pow(direction.y, 2));

    if (mag < 0.0001f)
        return (Vec2){0, 0};

    direction.x = direction.x / mag;
    direction.y = direction.y / mag;

    Vec2 midpoint = findMidpoints(line);

    Vec2 normal = {-direction.y * 0.1f, direction.x * 0.1f};

    float dotP = (normal.x * (-midpoint.x) + normal.y * (-midpoint.y));

    if (dotP < 0)
    {
        normal.x = -normal.x;
        normal.y = -normal.y;
    }

    return normal;
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

void game(void);

void addCircle(float x, float y, float r)
{
    circles[circle_count] = init_circle(circle_count, x, y, r);
    circle_count++;
}

void addLine(float x1, float y1, float x2, float y2)
{
    if (line_count >= MAX_SHAPES)
        return;

    lines[line_count] = init_line(line_count, x1, y1, x2, y2);
    line_count++;
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

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    addCircle(0.0f, 0.2f, 0.05f);

    // addLine(-0.0f, 0.4f, 0.8f, 0.4f);   // top
    // addLine(-0.8f, -0.4f, 0.0f, -0.4f); // bottom
    // addLine(-0.8f, -0.4f, 0.0f, 0.4f);  // left
    // addLine(0.0f, -0.4f, 0.8f, 0.4f);   // right

    // addLine(-0.5f, 0.5f, 0.5f, 0.5f);   // top
    // addLine(0.5f, -0.5f, 0.5f, 0.5f);   // right
    // addLine(-0.5f, -0.5f, 0.5f, -0.5f); // bottom
    // addLine(-0.5f, -0.5f, -0.5f, 0.5f); // left

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        int frameBufferWidth, frameBufferHeight;
        glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

        float aspect = (float)frameBufferHeight / (float)frameBufferWidth;
        int aspectLoc = glGetUniformLocation(shaderProgram, "uAspect");
        glUniform1f(aspectLoc, aspect);

        setColor(1.0f, 1.0f, 0.0f, 1.0f);

        velocity.x += acceleration.x;
        velocity.y += acceleration.y;

        circles[0].y += velocity.y;
        circles[0].x += velocity.x;

        for (int i = 0; i < line_count; i++)
        {
            // Line direction vector
            Vec2 direction = {lines[i].x2 - lines[i].x1, lines[i].y2 - lines[i].y1};

            // Direction vector of circle to line
            Vec2 toCircle = {circles[0].x - lines[i].x1, circles[0].y - lines[i].y1};

            // The code below finds the length of the line and distance from the circle to the starting of the line
            // The formula to find distance between two points is sqrt(x1 * x2 + y1 * y2)
            // But sqrt is cpu heavy and wastes a lot of cpu computation
            // So instead we use the square of it (no sqrt)

            // Length squared of line segment
            // Same here as well using squared to reduce computation
            float lineLengthSq = direction.x * direction.x + direction.y * direction.y;

            // Length from circle to the line
            float circlineLengthSq = toCircle.x * direction.x + toCircle.y * direction.y;

            // Project circle center onto line (0 to 1 range)
            // 0 being the starting of the line
            // 1 being the ending of the line
            float t = circlineLengthSq / lineLengthSq;

            // Clamp t to line segment
            if (t < 0.0f)
            {
                t = 0.0f;
            }
            if (t > 1.0f)
            {
                t = 1.0f;
            }

            // Find closest point on line segment
            Vec2 closest = {lines[i].x1 + t * direction.x, lines[i].y1 + t * direction.y};

            // Distance from circle center to closest point
            float dx = circles[0].x - closest.x;
            float dy = circles[0].y - closest.y;
            float distance = sqrtf(dx * dx + dy * dy);

            velocity.y -= gravity;

            // Check collision
            if (distance <= circles[0].r)
            {
                // Collision normal (from line to circle)
                Vec2 collisionNormal = {dx / distance, dy / distance};

                // Push circle out of the line
                float penetration = circles[0].r - distance;
                circles[0].x += collisionNormal.x * penetration;
                circles[0].y += collisionNormal.y * penetration;


                // Reflect velocity
                float dot = velocity.x * collisionNormal.x + velocity.y * collisionNormal.y;

                Vec2 normalVel = {dot * collisionNormal.x, dot * collisionNormal.y};

                normalVel.x *= -restitution;
                normalVel.y *= -restitution;

                Vec2 tangentVel = {velocity.x - normalVel.x, velocity.y - normalVel.y};

                tangentVel.x *= (1.0f - friction);
                tangentVel.y *= (1.0f - friction);

                velocity.x = tangentVel.x + normalVel.x;
                velocity.y = tangentVel.y + normalVel.y;
            }

            // Draw normals
            Vec2 normal = findNormal(lines[i]);
            Vec2 midpoint = findMidpoints(lines[i]);

            setColor((float)i * 0.8f, 0.0f, 1.0f, 1.0f);

            Line line = {-1, midpoint.x, midpoint.y, midpoint.x + normal.x, midpoint.y + normal.y};
            drawLine(line);
        }
        game();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void game(void)
{
    setColor(1.0f, 1.0f, 0.0f, 1.0f);
    drawCircle(circles[0]);
    setColor(1.0f, 1.0f, 1.0f, 1.0f);
    for (int i = 0; i < line_count; i++)
    {
        drawLine(lines[i]);
    }
}