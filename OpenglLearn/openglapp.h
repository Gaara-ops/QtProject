#ifndef OPENGLAPP_H
#define OPENGLAPP_H

#include "opengl_util.h"
class OpenglApp
{
public:
    OpenglApp();
    void CalcFPS();

    void RenderFPS();

    float GetRunningTime();
protected:

private:
    long long m_frameTime;
    long long m_startTime;
    int m_frameCount;
    int m_fps;
};

#endif // OPENGLAPP_H
