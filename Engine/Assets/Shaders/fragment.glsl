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
flat in int mtlID;

struct AllTexture
{
    uvec2 _albedo;
    uvec2 _normal;
    uvec2 _ao;
    uvec2 _roughness;
    uvec2 _metallic;

    uvec2 _pad1;

    vec4 _color;
    vec4 _scale;
};

layout(std430, binding = 4) readonly buffer AllTextureMap{
    AllTexture allTextures[];
};

void main()
{
    aAlbedo = texture(sampler2D(allTextures[mtlID]._albedo),uv) * allTextures[mtlID]._color;
    
    aPosition = vec4(FragPos,1.0f);

    vec3 normTextureValue = vec3(texture(sampler2D(allTextures[mtlID]._normal),uv).rgb);
    normTextureValue = normalize(normTextureValue * 2.0f -1.0f);
    normTextureValue = normalize(TBN* normTextureValue);
    aNormals = vec4(normTextureValue, 1.0f);

    aAO = texture(sampler2D(allTextures[mtlID]._ao),uv).r;

    aRoughness = texture(sampler2D(allTextures[mtlID]._roughness),uv).r;

    aMetallic  = texture(sampler2D(allTextures[mtlID]._metallic),uv).r;
}