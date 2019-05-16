#include <QCoreApplication>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "GL/glew.h"//一定要在GLUT引入之前引入,其他OpenGL头文件放在这个之后
#include "GL/freeglut.h"
#include "glm.hpp"
#include "math3d.h"
#include "pipeline.h"
#include "camera.h"
#include "texture.h"
#include "technique.h"
#include "glut_backend.h"
#include "icallback.h"
#include "openglapp.h"
#include "lighting_technique.h"

// 屏幕宽高宏定义
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

struct Vertex
{
    Vector3f m_pos;
    Vector2f m_tex;

    Vertex() {}

    Vertex(Vector3f pos, Vector2f tex)
    {
        m_pos = pos;
        m_tex = tex;
    }
};

class HelloGL : public ICallbacks,public OpenglApp
{
public:
    HelloGL(){
        m_pGameCamera = NULL;
        m_pTexture = NULL;
        m_pEffect = NULL;
        m_scale = 0.0f;
        //平行光设置
        m_directionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
        m_directionalLight.AmbientIntensity = 0.5f;
        //透视变换参数设置
        m_persProjInfo.FOV = 60.0f;
        m_persProjInfo.Height = WINDOW_HEIGHT;
        m_persProjInfo.Width = WINDOW_WIDTH;
        m_persProjInfo.zNear = 1.0f;
        m_persProjInfo.zFar = 100.0f;
    }
    ~HelloGL(){
        delete m_pEffect;
        delete m_pGameCamera;
        delete m_pTexture;
    }
    bool Init()
    {
        m_pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT);

        CreateVertexBuffer();//创建顶点数据
        CreateIndexBuffer();//创建索引数据

        m_pEffect = new LightingTechnique();//管理着色器
        if (!m_pEffect->Init())
        {
            return false;
        }
        m_pEffect->Enable();
        m_pEffect->SetTextureUnit(0);//设置纹理单元索引

        std::string textureName = "F:/opengl/ogldev-source/Content/test.png";
        m_pTexture = new Texture(GL_TEXTURE_2D, textureName);//管理纹理
        if (!m_pTexture->Load()) {
            return false;
        }
        return true;
    }

    void Run()
    {
        GLUTBackendRun(this);
    }
    //渲染回调函数
    virtual void RenderSceneCB()
    {
        m_pGameCamera->OnRender();

        glClear(GL_COLOR_BUFFER_BIT);// 清空颜色缓存

        m_scale += 0.0002f;
        // 实例化一个pipeline管线类对象，初始化配置好之后传递给shader
        Pipeline p;
        p.Rotate(0.0f, m_scale, 0.0f);
        p.WorldPos(0.0f, 0.0f, 3.0f);
        p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
        p.SetPerspectiveProj(m_persProjInfo);
        m_pEffect->SetWVP(p.GetWVPTrans());
        m_pEffect->SetDirectionalLight(m_directionalLight);

        glEnableVertexAttribArray(0);//开启顶点属性
        glEnableVertexAttribArray(1);//启用纹理属性
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);// 绑定GL_ARRAY_BUFFER缓冲器
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        //定义顶点缓冲器中纹理坐标的位置
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
        ///索引绘制
        // 每次在绘制之前绑定索引缓冲
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
        m_pTexture->Bind(GL_TEXTURE0);
        /**
    第一个参数是要渲染的图元的类型
    第二个参数是索引缓冲中用于产生图元的索引个数
    第三个参数是每一个索引值的数据类型,必须要告诉GPU单个索引值的大小，否则GPU无法知道如何
    解析索引缓冲区。索引值可用的类型主要有：GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT,
    GL_UNSIGNED_INT。如果索引的范围很小应该选择小的数据类型来节省空间，
    如果索引范围很大自然就要根据需要选择更大的数据类型
    最后一个参数告诉GPU从索引缓冲区起始位置到到第一个需要扫描的索引值得偏移byte数，这个在使用
    同一个索引缓冲来保存多个物体的索引时很有用，通过定义偏移量和索引个数可以告诉GPU去渲染
    哪一个物体，在我们的例子中我们从一开始扫描所以定义偏移量为0。
    注意最后一个参数的类型是GLvoid*，所以如果不是0的话必须要转换成那个参数类型。
          */
        // 索引绘制图形
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

        glDisableVertexAttribArray(0);//禁用顶点数据
        glDisableVertexAttribArray(1);//禁用纹理数据

        glutSwapBuffers();// 交换前后缓存
    }

    virtual void KeyboardCB(int key, int keyState)
    {
        std::cout << "key:" << key << ",state:" << keyState << std::endl;
        switch (key) {
            case OGLDEV_KEY_ESCAPE:
            case OGLDEV_KEY_q:
                    GLUTBackendLeaveMainLoop();
                    break;

            case OGLDEV_KEY_a:
                m_directionalLight.AmbientIntensity += 0.05f;
                break;

            case OGLDEV_KEY_s:
                m_directionalLight.AmbientIntensity -= 0.05f;
                break;
        }
    }
    virtual void PassiveMouseCB(int x, int y)
    {
        //m_pGameCamera->OnMouse(x, y);
    }
private:
    GLuint m_VBO;//操作顶点缓冲器对象
    GLuint m_IBO;// 索引缓冲对象的句柄
    LightingTechnique* m_pEffect;
    Texture* m_pTexture;//纹理控制
    Camera* m_pGameCamera;//相机控制
    float m_scale;//控制图形周期性变化
    DirectionalLight m_directionalLight;//控制灯光
    PersProjInfo m_persProjInfo;// 透视变换配置参数数据结构
    /**
     * 创建顶点缓冲器
     */
    void CreateVertexBuffer()
    {
        // 创建含有一个顶点的顶点数组
        /*Vector3f Vertices[4];
        Vertices[0] = Vector3f(-1.0f, -1.0f, 0.5773f);
        Vertices[1] = Vector3f(0.0f, -1.0f, -1.15475f);
        Vertices[2] = Vector3f(1.0f, -1.0f, 0.5773f);
        Vertices[3] = Vector3f(0.0f, 1.0f, 0.0f);*/
        Vertex Vertices[4] = { Vertex(Vector3f(-1.0f, -1.0f, 0.5773f), Vector2f(0.0f, 0.0f)),
                               Vertex(Vector3f(0.0f, -1.0f, -1.15475f), Vector2f(1.0f, 0.0f)),
                               Vertex(Vector3f(1.0f, -1.0f, 0.5773f),  Vector2f(1.0f, 1.0f)),
                               Vertex(Vector3f(0.0f, 1.0f, 0.0f),      Vector2f(0.0f, 1.0f)) };


        /**
          glGen*前缀的函数来产生不同类型的对象。它们通常有两个参数：
        第一个参数用来定义你想创建的对象的数量;
        第二个参数是一个GLuint变量的数组的地址，来存储分配给你的引用变量handles
            （要确保这个数组足够大来处理你的请求！）
        以后对这个函数的调用将不会重复产生相同的handle对象，
        除非你先使用glDeleteBuffers删除他们
          */
        // 创建缓冲器
        glGenBuffers(1, &m_VBO);
        /**
          将handle与一个目标的名称进行绑定，然后在该目标上执行命令.
        这些指令只会在与handle绑定的目标上生效直到另外有其他的对象
        跟这个handle绑定或者这个handle被置空。
        目标名GL_ARRAY_BUFFER意思是这个buffer将存储一个顶点的数组。
        另外一个是GL_ELEMENT_ARRAY_BUFFER,意思是这个buffer存储的是另一个buffer中顶点的标记
          */
        // 绑定GL_ARRAY_BUFFER缓冲器
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
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
    // 创建索引缓冲器
    void CreateIndexBuffer()
    {
        // 四个三角形面的顶点索引集
        unsigned int Indices[] = { 0, 3, 1,
                                   1, 3, 2,
                                   2, 3, 0,
                                   0, 1, 2 };
        // 创建缓冲区
        glGenBuffers(1, &m_IBO);
        // 绑定缓冲区
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
        // 添加缓冲区数据
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
    }

};

int main(int argc, char *argv[])
{
    GLUTBackendInit(argc,argv,false,false);
    if(!GLUTBackendCreateWindow(WINDOW_WIDTH,WINDOW_HEIGHT,
                                false,"Hello,GL")){
        return 1;
    }

    HelloGL* pApp = new HelloGL;

    if (!pApp->Init()) {
        return 1;
    }

    pApp->Run();

    delete pApp;

    return 0;
}
