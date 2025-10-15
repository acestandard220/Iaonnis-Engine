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


struct CommandData{
    vec2 values;
};

struct MeshMap{
    vec2 values;
};

layout (std430,binding = 10) readonly buffer CmdDataBuffer
{
    CommandData cmdData[];
};

layout (std430,binding = 11) readonly buffer MeshMapBuffer
{
    MeshMap meshMap[];
};

layout (std430,binding = 12) readonly buffer ModelMatBuffer
{
    mat4 modelMats[];
};

void main()
{

   drawID = gl_DrawIDARB;
    int drawOffset = int(cmdData[drawID].values.y);
    int modelIndex = 0;
    int accumulatedVertices = 0;
    
    for(int i = 0; i < cmdData[drawID].values.x; i++)
    {
        accumulatedVertices += int(meshMap[drawOffset + i].values.x);
        if(gl_VertexID < accumulatedVertices)
        {
            modelIndex = int(meshMap[drawOffset + i].values.y);
            break;
        }
    }

    FragPos = aPos;
    gl_Position = mvp * vec4(aPos, 1.0);

    vec3 T = normalize(mat3(1.0) * aTan);
    vec3 B = normalize(mat3(1.0) * aBitan);
    vec3 N = normalize(mat3(1.0) * aNorm);

	TBN = mat3(T,B,N);

    norm = aNorm;
    uv = aUV;
}