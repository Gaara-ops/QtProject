#version 330  //告诉编译器我们的目标GLSL编译器版本是3.3

layout (location = 0) in vec3 Position;
// 绑定定点属性名和属性，方式二缓冲属性和shader属性对应映射

uniform float gScale;//一致变量(缩放)
// 平移,旋转,缩放变换矩阵一致变量
uniform mat4 gWVP;

//在各渲染管线之间进行传递的参数必须在shader中使用‘out’保留字来全局的进行声明定义
// 顶点着色器输出的颜色值
out vec4 Color;

void main()
{
    //对引入的顶点位置做固定编码转换
    //gl_Position = vec4(gScale * Position.x, gScale * Position.y, Position.z, 1.0);
    // 为glVertexAttributePointer提供返回值

    vec4 scalePos = vec4(gScale * Position.x, gScale * Position.y, gScale * Position.z, 1.0);

    // 用平移变换矩阵乘以图形顶点位置对应的4X4矩阵相乘，完成平移变换
    gl_Position = gWVP * scalePos;

    // 颜色插值,在图形管线中颜色值通常使用一个在[0.0,1.0]范围内的浮点数来表示
    //clamp()函数保证值不会超出0.0-1.0的范围
    Color = vec4(clamp(Position, 0.0, 1.0), 1.0);
}
