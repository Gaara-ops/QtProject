#ifndef LIGHTING_TECHNIQUE_H
#define	LIGHTING_TECHNIQUE_H

#include "technique.h"
#include "math3d.h"
#include "commonlight.h"


class LightingTechnique : public Technique
{
public:

    LightingTechnique();

    virtual bool Init();

    void SetWVP(const Matrix4f& WVP);
    void SetTextureUnit(unsigned int TextureUnit);
    void SetDirectionalLight(const DirectionalLight& Light);

private:

    GLuint m_WVPLocation;
    GLuint m_samplerLocation;
    GLuint m_dirLightColorLocation;
    GLuint m_dirLightAmbientIntensityLocation;
};


#endif	/* LIGHTING_TECHNIQUE_H */
