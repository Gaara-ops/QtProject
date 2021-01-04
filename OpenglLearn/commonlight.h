#ifndef COMMONLIGHT_H
#define COMMONLIGHT_H
#include "math3d.h"
#include <iostream>

class BaseLight
{
public:
    std::string Name;
    Vector3f Color;//灯光颜色
    float AmbientIntensity;//环境光强度
    float DiffuseIntensity;//漫反射光强度

    BaseLight()
    {
        Color = Vector3f(0.0f, 0.0f, 0.0f);
        AmbientIntensity = 0.0f;
        DiffuseIntensity = 0.0f;
    }

};

//方向光(平行光)
class DirectionalLight : public BaseLight
{
public:
    Vector3f Direction;//灯光投射方向

    DirectionalLight()
    {
        Direction = Vector3f(0.0f, 0.0f, 0.0f);
    }

};


struct LightAttenuation
{
    float Constant;
    float Linear;
    float Exp;

    LightAttenuation()
    {
        Constant = 1.0f;
        Linear = 0.0f;
        Exp = 0.0f;
    }
};

//点光源
class PointLight : public BaseLight
{
public:
    Vector3f Position;
    LightAttenuation Attenuation;

    PointLight()
    {
        Position = Vector3f(0.0f, 0.0f, 0.0f);
    }

};


class SpotLight : public PointLight
{
public:
    Vector3f Direction;
    float Cutoff;

    SpotLight()
    {
        Direction = Vector3f(0.0f, 0.0f, 0.0f);
        Cutoff = 0.0f;
    }

};


#define COLOR_WHITE Vector3f(1.0f, 1.0f, 1.0f)
#define COLOR_RED Vector3f(1.0f, 0.0f, 0.0f)
#define COLOR_GREEN Vector3f(0.0f, 1.0f, 0.0f)
#define COLOR_CYAN Vector3f(0.0f, 1.0f, 1.0f)
#define COLOR_BLUE Vector3f(0.0f, 0.0f, 1.0f)
#endif // COMMONLIGHT_H
