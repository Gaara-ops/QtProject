#version 330  //告诉编译器我们的目标GLSL编译器版本是3.3

layout (location = 0) in vec3 Position;
// 绑定定点属性名和属性，方式二缓冲属性和shader属性对应映射

uniform float gScale;//一致变量(缩放)
// 平移变换矩阵一致变量
uniform mat4 gWorld;

void main()
{
    //对引入的顶点位置做固定编码转换
    //gl_Position = vec4(gScale * Position.x, gScale * Position.y, Position.z, 1.0);
    // 为glVertexAttributePointer提供返回值

    // 用平移变换矩阵乘以图形顶点位置对应的4X4矩阵相乘，完成平移变换
    gl_Position = gWorld * vec4(Position, gScale*1.0);
}
