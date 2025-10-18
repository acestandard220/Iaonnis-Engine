#version 460 core
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_shader_draw_parameters : require

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec3 aTan;
layout(location = 4) in vec3 aBitan;
layout(location = 5) in uint aId; 

out vec3 norm;
out vec2 uv;
out vec3 FragPos;

out mat3 TBN;

flat out int drawID;
flat out int mtlID;

uniform mat4 mvp;

struct CommandData
{
    int offset;

    int nMeshes;
};

layout (std430, binding = 5) readonly buffer CommandDataBuffer
{
    CommandData commandData[];
};

layout(std430, binding = 6) readonly buffer TransformBuffer
{
    mat4 transformData[];
};

layout(std430, binding = 7)readonly buffer MaterialMapBuffer
{
    int materialMap[];
};

void main()
{
    drawID = gl_DrawIDARB;
    CommandData cmdData = commandData[drawID];
    mat4 model = transformData[drawID]; //+aId which is what it should be does not work...chechking...
    mtlID = materialMap[cmdData.offset + aId];

    FragPos = aPos;
    gl_Position = mvp * model * vec4(aPos, 1.0);

    vec3 T = normalize(mat3(model) * aTan);
    vec3 B = normalize(mat3(model) * aBitan);
    vec3 N = normalize(mat3(model) * aNorm);

	TBN = mat3(T,B,N);

    norm = aNorm;
    uv = aUV;
}