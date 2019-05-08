#ifndef TEXTURE_H
#define TEXTURE_H
#include <string>

#include <GL/glew.h>
#include <QImage>

class Texture
{
public:
    Texture(GLenum TextureTarget, const std::string& FileName);

    bool Load();

    void Bind(GLenum TextureUnit);

private:
    std::string m_fileName;
    GLenum m_textureTarget;
    GLuint m_textureObj;
    QImage m_image;
};

#endif // TEXTURE_H
