#version 330  //告诉编译器我们的目标GLSL编译器版本是3.3

//这个变量经过了光栅器的插值，所以每一个被执行的片断着色器FS可能都会是不同的颜色。
// 接受vs传来的插值后的颜色值
in vec4 Color;
in vec2 TexCoord0;

out vec4 FragColor;  // 片段着色器的输出颜色变量,4个分量代表R,G,B和A（透明度）

uniform sampler2D gSampler;

// 着色器的唯一入口函数,片段着色器的工作就是定义每个片段（像素）的颜色
void main()
{
    //FragColor = vec4(1.0, 1.0, 0.0, 1.0);
    // 使用插值后得到的颜色作为片断着色器的颜色输出
    //FragColor = Color;

    FragColor = texture2D(gSampler, TexCoord0.xy);
}
