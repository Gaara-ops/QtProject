#ifndef GLUT_BACKEND_H
#define GLUT_BACKEND_H

#include "icallback.h"

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;


void GLUTBackendInit(int argc, char** argv, bool WithDepth, bool WithStencil);


bool GLUTBackendCreateWindow(uint Width, uint Height, bool isFullScreen, const char* pTitle);

void GLUTBackendRun(ICallbacks* pCallbacks);

void GLUTBackendSwapBuffers();

void GLUTBackendLeaveMainLoop();


#endif // GLUT_BACKEND_H
