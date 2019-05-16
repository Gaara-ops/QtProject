#ifndef ICALLBACK_H
#define ICALLBACK_H

class ICallbacks
{
public:

    virtual void KeyboardCB(int key, int keyState = 0) {};

    virtual void PassiveMouseCB(int x, int y) {};

    virtual void RenderSceneCB() {};

    virtual void MouseCB(int button, int state, int x, int y) {};
};

#endif // ICALLBACK_H
