#version 330  //告诉编译器我们的目标GLSL编译器版本是3.3

//这个变量经过了光栅器的插值，所以每一个被执行的片断着色器FS可能都会是不同的颜色。
// 接受vs传来的插值后的颜色值
in vec2 TexCoord0;
in vec3 Normal0;
in vec3 WorldPos0;

out vec4 FragColor;

struct DirectionalLight
{
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
    vec3 Direction;
};

uniform DirectionalLight gDirectionalLight;
uniform sampler2D gSampler;
uniform vec3 gEyeWorldPos;
uniform float gMatSpecularIntensity;
uniform float gSpecularPower;

// 着色器的唯一入口函数,片段着色器的工作就是定义每个片段（像素）的颜色
void main()
{
    //FragColor = vec4(1.0, 1.0, 0.0, 1.0);
    // 使用插值后得到的颜色作为片断着色器的颜色输出
    //FragColor = Color;

    vec4 AmbientColor = vec4(gDirectionalLight.Color * gDirectionalLight.AmbientIntensity, 1.0f);
    vec3 LightDirection = -gDirectionalLight.Direction;
    vec3 Normal = normalize(Normal0);

    float DiffuseFactor = dot(Normal, LightDirection);

    vec4 DiffuseColor  = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0) {
        DiffuseColor = vec4(gDirectionalLight.Color * gDirectionalLight.DiffuseIntensity * DiffuseFactor, 1.0f);

        vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos0);
        vec3 LightReflect = normalize(reflect(gDirectionalLight.Direction, Normal));
        float SpecularFactor = dot(VertexToEye, LightReflect);
        if (SpecularFactor > 0) {
            SpecularFactor = pow(SpecularFactor, gSpecularPower);
            SpecularColor = vec4(gDirectionalLight.Color * gMatSpecularIntensity * SpecularFactor, 1.0f);
        }
    }

    FragColor = texture2D(gSampler, TexCoord0.xy) *
                (AmbientColor + DiffuseColor + SpecularColor);
}
