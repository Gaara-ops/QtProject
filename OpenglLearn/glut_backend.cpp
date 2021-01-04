#ifndef WIN32
#include <unistd.h>
#endif
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <QDebug>

#include "glut_backend.h"

static ICallbacks* s_pCallbacks = NULL;

static bool sWithDepth = false;
static bool sWithStencil = false;

static int GLUTMouseToOGLDEVMouse(uint Button)
{
    switch (Button) {
    case GLUT_LEFT_BUTTON:
            return 0;
    case GLUT_RIGHT_BUTTON:
            return 2;
    case GLUT_MIDDLE_BUTTON:
            return 1;
    default:
            break;
    }
    return -1;
}

static void SpecialKeyboardCB(int Key, int x, int y)
{
    s_pCallbacks->KeyboardCB(Key);
}

static void KeyboardCB(unsigned char Key, int x, int y)
{
    if (
        ((Key >= '+') && (Key <= '9')) ||
        ((Key >= 'A') && (Key <= 'Z')) ||
        ((Key >= 'a') && (Key <= 'z'))
        ) {
        s_pCallbacks->KeyboardCB((int)Key);
    }
    else {
        printf("%d Unimplemented GLUT key\n", Key);
    }

}

static void PassiveMouseCB(int x, int y)
{
    s_pCallbacks->PassiveMouseCB(x, y);
}


static void RenderSceneCB()
{
    s_pCallbacks->RenderSceneCB();
}


static void IdleCB()
{
    s_pCallbacks->RenderSceneCB();
}

static void MouseCB(int Button, int State, int x, int y)
{
    s_pCallbacks->MouseCB(Button, State, x, y);
}


static void InitCallbacks()
{
    glutDisplayFunc(RenderSceneCB);
    glutIdleFunc(IdleCB);// 将渲染回调注册为全局闲置回调
    glutSpecialFunc(SpecialKeyboardCB);// 用于监听特殊键盘事件
    glutPassiveMotionFunc(PassiveMouseCB);//用于监听鼠标事件
    glutKeyboardFunc(KeyboardCB);//监听常规键盘按下的事件
    glutMouseFunc(MouseCB);
}


void GLUTBackendInit(int argc, char** argv, bool WithDepth, bool WithStencil){
    sWithDepth = WithDepth;
    sWithStencil = WithStencil;

    glutInit(&argc, argv);// 初始化GLUT
    // 显示模式：双缓冲、RGBA
    uint DisplayMode = GLUT_DOUBLE|GLUT_RGBA;

    if (WithDepth) {
        DisplayMode |= GLUT_DEPTH;
    }

    if (WithStencil) {
        DisplayMode |= GLUT_STENCIL;
    }

    glutInitDisplayMode(DisplayMode);//设置显示模式

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    qDebug() << "GLUTBackendInit end";
}


bool GLUTBackendCreateWindow(uint Width, uint Height, bool isFullScreen, const char* pTitle){
    if (isFullScreen) {
        char ModeString[64] = { 0 };
        int bpp = 32;
        _snprintf_s(ModeString, sizeof(ModeString), "%dx%d:%d@60", Width, Height, bpp);
        glutGameModeString(ModeString);
        glutEnterGameMode();//进入游戏模式
    }
    else {
        glutInitWindowSize(Width, Height);  // 窗口尺寸
        glutInitWindowPosition(100, 100);  // 窗口位置
        glutCreateWindow(pTitle);  // 窗口标题
    }

    //检查GLEW是否就绪，必须要在GLUT初始化之后
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return false;
    }
    qDebug() << "GLUTBackendCreateWindow end";

    return true;
}

void GLUTBackendRun(ICallbacks* pCallbacks){
    if (!pCallbacks) {
        fprintf(stderr, "%s : callbacks not specified!\n", __FUNCTION__);
        return;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);// 缓存清空后的颜色值
    glFrontFace(GL_CW);//告诉OpenGL三角形的顶点是按照顺时针顺序定义的
    glCullFace(GL_BACK);//告诉GPU剔除三角形的背面
    glEnable(GL_CULL_FACE);//开启面剔除本身（默认是关闭的）

    if (sWithDepth) {
        glEnable(GL_DEPTH_TEST);
    }

    s_pCallbacks = pCallbacks;
    InitCallbacks();
    glutMainLoop();
}

void GLUTBackendSwapBuffers(){
    glutSwapBuffers();
}

void GLUTBackendLeaveMainLoop(){
    glutLeaveMainLoop();
}
