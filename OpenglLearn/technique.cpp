#include "technique.h"

#include <stdio.h>
#include <string.h>

#define GLCheckError() (glGetError() == GL_NO_ERROR)
#define INVALID_UNIFORM_LOCATION 0xffffffff

using namespace std;

Technique::Technique()
{
     m_shaderProg = 0;
}

Technique::~Technique()
{
    for (ShaderObjList::iterator it = m_shaderObjList.begin() ; it != m_shaderObjList.end() ; it++)
    {
        glDeleteShader(*it);
    }

    if (m_shaderProg != 0)
    {
        glDeleteProgram(m_shaderProg);
        m_shaderProg = 0;
    }
}

bool Technique::Init()
{
    //创建着色器程序,我们将把所有的着色器连接到这个对象上
    m_shaderProg = glCreateProgram();
    // 检查是否创建成功
    if (m_shaderProg == 0) {
        fprintf(stderr, "Error creating shader program\n");
        return false;
    }

    return true;
}

void Technique::Enable()
{
    /**
    这个程序将在所有的draw call中一直生效直到你用另一个替换掉它或者使用glUseProgram指令
    将其置NULL明确地禁用它。如果你创建的shader程序只包含一种类型的shader（只是为某一个
    阶段添加的自定义shader），那么在其他阶段的该操作将会使用它们默认的固定功能操作
      */
    // 设置到管线声明中来使用上面成功建立的shader程序
    glUseProgram(m_shaderProg);
}

bool Technique::AddShader(GLenum ShaderType, const char *pFilename)
{
    string s;//存储着色器文本的字符串缓冲
    //读取着色器文件中的文本到字符串缓冲区
    if (!ReadFile(pFilename, s)) {
        return false;
    }

    GLuint ShaderObj = glCreateShader(ShaderType);//创建shader对象
    // 检查是否创建成功
    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        return false;
    }

    //保存shader对象,析构的时候删除
    m_shaderObjList.push_back(ShaderObj);
    // 定义shader的代码源
    const GLchar* p[1];
    p[0] = s.c_str();
    GLint Lengths[1] = { (GLint)s.size() };
    /**
函数glShaderSource以shader对象为参数，使你可以灵活的定义代码来源。
shader源代码（也就是我们所常说的shader脚本）可以由多个字符串数组排布组合而成，
你需要提供一个指针数组来对应指向这些字符窜数组，同时要提供一个整型数组来对应表示每个数组的长度。
为了简单，我们这里只使用一个字符串数组来保存所有的shader源代码，并且分别用数组的一个元素来分
别指向这个字符串数组和表示数组的长度。
第二个参数表示的是这两个数组的元素个数（我们的例子中则只有1个）。
     */
    glShaderSource(ShaderObj, 1, p, Lengths);

    glCompileShader(ShaderObj);// 编译shader对象
    // 检查和shader相关的错误
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling '%s': '%s'\n", pFilename, InfoLog);
        return false;
    }
    // 将编译好的shader对象绑定到program object(着色器)程序对象上
    glAttachShader(m_shaderProg, ShaderObj);

    return true;
}
// 所有的着色器程序添加完成后调用这个进行链接和验证程序
bool Technique::Finalize()
{
    // 链接shader着色器程序，并检查程序相关错误
    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };
    /**
编译好所有的shader对象并将他们绑定到程序中后我就可以连接他们了。注意在完成程序的连接后你
可以通过调用函数glDetachShader和glDeleteShader来清除每个中介shader对象。
OpenGL保存着由它产生的多数对象的引用计数，如果一个shader对象被创建后又被删除的话驱动
程序也会同时清除掉它，但是如果他被绑定在程序上，只调用glDeleteShader函数只是会标记它等待
删除，只有等你调用glDetachShader后它的引用计数才会被置零然后被移除掉。
     */
    glLinkProgram(m_shaderProg);

    glGetProgramiv(m_shaderProg, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(m_shaderProg, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        return false;
    }
    // 检查验证在当前的管线状态程序是否可以被执行
    glValidateProgram(m_shaderProg);
    glGetProgramiv(m_shaderProg, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(m_shaderProg, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
     //   return false;
    }

    // Delete the intermediate shader objects that have been added to the program
    for (ShaderObjList::iterator it = m_shaderObjList.begin() ; it != m_shaderObjList.end() ; it++) {
        glDeleteShader(*it);
    }

    m_shaderObjList.clear();

    return GLCheckError();
}

GLint Technique::GetUniformLocation(const char *pUniformName)
{
    // 查询获取一致变量的位置,执行环境映射到shader着色器执行环境
    GLuint Location = glGetUniformLocation(m_shaderProg, pUniformName);
    // 检查错误
    if (Location == INVALID_UNIFORM_LOCATION) {
        fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", pUniformName);
    }

    return Location;
}

GLint Technique::GetProgramParam(GLint param)
{
    GLint ret;
    glGetProgramiv(m_shaderProg, param, &ret);
    return ret;
}
