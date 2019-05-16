#ifndef TECHNIQUE_H
#define TECHNIQUE_H

//对shader进行管理
#include <list>
#include <GL/glew.h>

#include "opengl_util.h"

class Technique
{
public:
    Technique();
    virtual ~Technique();

    virtual bool Init();

    void Enable();

public:

    bool AddShader(GLenum ShaderType, const char* pFilename);

    bool Finalize();

    GLint GetUniformLocation(const char* pUniformName);

    GLint GetProgramParam(GLint param);

    GLuint m_shaderProg;//着色器程序,管理所有着色器

private:

    typedef std::list<GLuint> ShaderObjList;
    ShaderObjList m_shaderObjList;
};

#endif // TECHNIQUE_H
