#include "fps.h"
double lastTime = 0.0;
int frameCount = 0;
double currentFPS = 0.0;
double deltaTime = 0.0;
double lastFrameTime = 0.0;

int calculateFPS(double currentTime)
{
    // double currentTime = glfwGetTime();
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    frameCount++;
    if (currentTime - lastTime >= 1.0)
    {
        currentFPS = frameCount / (currentTime - lastTime);

        return (int)currentFPS;
        
        frameCount = 0;
        lastTime = currentTime;

    }
    return 0;
}
