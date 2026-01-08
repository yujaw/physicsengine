#ifndef FPS_H
#define FPS_H

extern double lastTime;
extern int frameCount;
extern double currentFPS;
extern double deltaTime;
extern double lastFrameTime;

int calculateFPS(double currentTime);

#endif