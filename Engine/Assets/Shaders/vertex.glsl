#version 460 core
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_shader_draw_parameters : require

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec3 aTan;
layout(location = 4) in vec3 aBitan;

out vec3 norm;
out vec2 uv;
out vec3 FragPos;

out mat3 TBN;

flat out int drawID;
uniform mat4 mvp;

void main()
{
    FragPos = aPos;
    gl_Position = mvp * vec4(aPos, 1.0);
    drawID = gl_DrawIDARB;

    vec3 T = normalize(mat3(1.0) * aTan);
    vec3 B = normalize(mat3(1.0) * aBitan);
    vec3 N = normalize(mat3(1.0) * aNorm);

	TBN = mat3(T,B,N);

    norm = aNorm;
    uv = aUV;
}