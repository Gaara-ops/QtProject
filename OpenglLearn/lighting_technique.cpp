#include "lighting_technique.h"


LightingTechnique::LightingTechnique()
{   
}

bool LightingTechnique::Init()
{
    if (!Technique::Init()) {
        return false;
    }
    const char* pVSFileName =
            "E:/workspace/MyQtProject/QtProject/OpenglLearn/shader.vs";
    const char* pFSFileName =
            "E:/workspace/MyQtProject/QtProject/OpenglLearn/shader.fs";
    if (!AddShader(GL_VERTEX_SHADER, pVSFileName)) {
        return false;
    }

    if (!AddShader(GL_FRAGMENT_SHADER, pFSFileName)) {
        return false;
    }

    if (!Finalize()) {
        return false;
    }

    m_WVPLocation = GetUniformLocation("gWVP");
    m_samplerLocation = GetUniformLocation("gSampler");
    m_dirLightColorLocation = GetUniformLocation("gDirectionalLight.Color");
    m_dirLightAmbientIntensityLocation =
            GetUniformLocation("gDirectionalLight.AmbientIntensity");

    if (m_dirLightAmbientIntensityLocation == 0xFFFFFFFF ||
        m_WVPLocation == 0xFFFFFFFF ||
        m_samplerLocation == 0xFFFFFFFF ||
        m_dirLightColorLocation == 0xFFFFFFFF)
    {
        return false;
    }

    return true;
}

void LightingTechnique::SetWVP(const Matrix4f& WVP)
{
    glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, (const GLfloat*)WVP.m);
}


void LightingTechnique::SetTextureUnit(unsigned int TextureUnit)
{
    //将要使用的纹理单元的索引放到shader中的取样器一致变量里
    glUniform1i(m_samplerLocation, TextureUnit);
}


void LightingTechnique::SetDirectionalLight(const DirectionalLight& Light)
{
    glUniform3f(m_dirLightColorLocation, Light.Color.x, Light.Color.y, Light.Color.z);
    glUniform1f(m_dirLightAmbientIntensityLocation, Light.AmbientIntensity);
}
