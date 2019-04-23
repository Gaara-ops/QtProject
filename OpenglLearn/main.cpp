#include <QCoreApplication>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "GL/glew.h"//一定要在GLUT引入之前引入,其他OpenGL头文件放在这个之后
#include "GL/freeglut.h"
#include "glm.hpp"
#include "math3d.h"
#include "opengl_util.h"
//#include "ogldev_math_3d.h"
//#include "ogldev_util.h"

//全局的GLuint引用变量,来操作顶点缓冲器对象,绝大多数OpenGL对象都是通过GLuint类型的变量来引用的.
GLuint VBO;
// 定义要读取的顶点着色器脚本和片断着色器脚本的文件名，作为文件读取路径
const char* pVSFileName = "E:/workspace/MyQtProject/QtProject/OpenglLearn/shader.vs";
const char* pFSFileName = "E:/workspace/MyQtProject/QtProject/OpenglLearn/shader.fs";

/**
 * 渲染回调函数
 */
static void RenderSceneCB(){
    // 清空颜色缓存
    glClear(GL_COLOR_BUFFER_BIT);

    // 开启顶点属性
    glEnableVertexAttribArray(0);
    // 绑定GL_ARRAY_BUFFER缓冲器
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    /**
第1个参定义了属性的索引，再这个例子中我们知道这个索引默认是0，
但是当我们开始使用shader着色器的时候，我们既要明确的设置着色器中的属性索引同时也要检索它；
第2个参数指的是属性中的元素个数（3个表示的是：X,Y,Z坐标）；
第3个参数指的是每个元素的数据类型；
第4个参数指明我们是否想让我们的属性在被管线使用之前被单位化，
我们这个例子中希望我们的数据保持不变的被传送；
第5个参数（称作’stride‘）指的是缓冲中那个属性的两个实例之间的比特数距离。
当只有一个属性（例如：buffer只含有一个顶点的位置数据）并且数据被紧密排布的时候将该参数值设置为0。
如果我们有一个包含位置和法向量（都是有三个浮点数的vector向量，一共6个浮点数）
两个属性的数据结构的数组的时候，我们将设置参数值为这个数据结构的比特大小（6*4=24）；
最后一个参数在前一个例子中非常有用。我们需要在管线发现我们的属性的地方定义数据结构中的内存偏移值。
在有位置数据和法向量数据的结构中，位置的偏移量为0，而法向量的偏移量则为12。
      */
    // 告诉管线怎样解析bufer中的数据
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    /**
      这个指令才是GPU真正开始工作的地方
      第一个参数我们定义拓扑结构为每一个顶点只表示一个点;
      下一个参数是第一个要绘制的顶点的索引
      最后一个参数是要绘制的顶点数
      */
    // 开始绘制几何图形(绘制一个点)
    //glDrawArrays(GL_POINTS, 0, 1);//绘制点
    glDrawArrays(GL_TRIANGLES, 0, 3);

    //  禁用顶点数据
    glDisableVertexAttribArray(0);

    // 交换前后缓存
    glutSwapBuffers();
}
static void InitializeGlutCallbacks()
{
    glutDisplayFunc(RenderSceneCB);
}
/**
 * 创建顶点缓冲器
 */
static void CreateVertexBuffer()
{
    // 创建含有一个顶点的顶点数组
    Vector3f Vertices[3];
    // 将点置于屏幕中央
    Vertices[0] = Vector3f(-1.0f, -1.0f, 0.0f);
    Vertices[1] = Vector3f(1.0f, -1.0f, 0.0f);
    Vertices[2] = Vector3f(0.0f, 1.0f, 0.0f);

    /**
      glGen*前缀的函数来产生不同类型的对象。它们通常有两个参数：
    第一个参数用来定义你想创建的对象的数量;
    第二个参数是一个GLuint变量的数组的地址，来存储分配给你的引用变量handles
        （要确保这个数组足够大来处理你的请求！）
    以后对这个函数的调用将不会重复产生相同的handle对象，
    除非你先使用glDeleteBuffers删除他们
      */
    // 创建缓冲器
    glGenBuffers(1, &VBO);
    /**
      将handle与一个目标的名称进行绑定，然后在该目标上执行命令.
    这些指令只会在与handle绑定的目标上生效直到另外有其他的对象
    跟这个handle绑定或者这个handle被置空。
    目标名GL_ARRAY_BUFFER意思是这个buffer将存储一个顶点的数组。
    另外一个是GL_ELEMENT_ARRAY_BUFFER,意思是这个buffer存储的是另一个buffer中顶点的标记
      */
    // 绑定GL_ARRAY_BUFFER缓冲器
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    /**
      往里面添加数据。这个回调函数取得我们之前绑定的目标名参数GL_ARRAY_BUFFER，
    还有数据的比特数参数，顶点数组的地址，还有一个表示这个数据模式的标志变量。
    因为我们不会去改变这个buffer的内容所以这里用了GL_STATIC_DRAW标志，
    相反的标志是GL_DYNAMIIC_DRAW, 这个只是给OpenGL的一个提示来给一些觉得合理的标志量使用，
    驱动程序可以通过它来进行启发式的优化（比如：内存中哪个位置最合适存储这个buffer缓冲）
      */
    // 绑定顶点数据
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

// 使用shader文本编译shader对象，并绑定shader都想到着色器程序中
static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
    // 根据shader类型参数定义两个shader对象
    GLuint ShaderObj = glCreateShader(ShaderType);
    // 检查是否定义成功
    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    // 定义shader的代码源
    const GLchar* p[1];
    p[0] = pShaderText;
    GLint Lengths[1];
    Lengths[0]= strlen(pShaderText);
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
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    // 将编译好的shader对象绑定到program object程序对象上
    glAttachShader(ShaderProgram, ShaderObj);
}

// 编译着色器函数
static void CompileShaders()
{
    // 创建着色器程序,我们将把所有的着色器连接到这个对象上
    GLuint ShaderProgram = glCreateProgram();
    // 检查是否创建成功
    if (ShaderProgram == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    // 存储着色器文本的字符串缓冲
    string vs, fs;
    // 分别读取着色器文件中的文本到字符串缓冲区
    if (!ReadFile(pVSFileName, vs)) {
        exit(1);
    };
    if (!ReadFile(pFSFileName, fs)) {
        exit(1);
    };

    // 添加顶点着色器和片段着色器
    AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);
    AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

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
    glLinkProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    // 检查验证在当前的管线状态程序是否可以被执行
    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }
/**
这个程序将在所有的draw call中一直生效直到你用另一个替换掉它或者使用glUseProgram指令
将其置NULL明确地禁用它。如果你创建的shader程序只包含一种类型的shader（只是为某一个
阶段添加的自定义shader），那么在其他阶段的该操作将会使用它们默认的固定功能操作
  */
    // 设置到管线声明中来使用上面成功建立的shader程序
    glUseProgram(ShaderProgram);
}

int main(int argc, char *argv[])
{
    // 初始化GLUT
    glutInit(&argc, argv);

    // 显示模式：双缓冲、RGBA
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    // 窗口设置
    glutInitWindowSize(480, 320);      // 窗口尺寸
    glutInitWindowPosition(100, 100);  // 窗口位置
    glutCreateWindow("Tutorial 02");   // 窗口标题

    // 开始渲染
    InitializeGlutCallbacks();

    // 检查GLEW是否就绪，必须要在GLUT初始化之后
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }
    printf("GL version: %s\n", glGetString(GL_VERSION));

    // 缓存清空后的颜色值
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // 创建顶点缓冲器
    CreateVertexBuffer();
    // 编译着色器
    CompileShaders();

    // 通知开始GLUT的内部循环
    glutMainLoop();


    return 0;
    //QCoreApplication a(argc, argv);
    //return a.exec();
}
