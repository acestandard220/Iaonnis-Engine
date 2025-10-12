#version 460 core
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_shader_draw_parameters : require

layout(location = 0) out vec4 aAlbedo;
layout(location = 1) out vec4 aPosition;
layout(location = 2) out vec4 aNormals;
layout(location = 3) out float aAO; 
layout(location = 4) out float aRoughness; 
layout(location = 5) out float aMetallic;

in vec3 FragPos;
in vec3 norm;
in vec2 uv;

in mat3 TBN;


flat in int drawID;

struct Material {
    vec4 color;
    vec4 scale; // +normStrenght +flipY
};

layout(std430, binding = 0) readonly buffer DiffuseMaps {
    uvec2 diffuseMap[];
};
layout(std430, binding = 1) readonly buffer NormalMaps {
    uvec2 normalMap[];
};
layout(std430, binding = 7) readonly buffer AOMaps{
    uvec2 aoMap[];
};

layout(std430, binding = 8) readonly buffer MetallicMap{
    uvec2 metallicMap[];
};

layout(std430, binding = 9) readonly buffer RoughnessMap{
    uvec2 roughnessMap[];
};

layout(std140)uniform Materials
{
    Material mtl[];
};

void main()
{
    aAlbedo = texture(sampler2D(diffuseMap[drawID]),uv) * mtl[drawID].color;

    aPosition = vec4(FragPos,1.0f);

    vec3 normTextureValue = vec3(texture(sampler2D(normalMap[drawID]),uv).rgb);
    normTextureValue = normalize(normTextureValue * 2.0f -1.0f);
    normTextureValue = normalize(TBN* normTextureValue);

    aNormals = vec4(normTextureValue, 1.0f);

    aAO = texture(sampler2D(aoMap[drawID]),uv).r;

    aRoughness = texture(sampler2D(roughnessMap[drawID]),uv).r;

    aMetallic  = texture(sampler2D(metallicMap[drawID]),uv).r;
}