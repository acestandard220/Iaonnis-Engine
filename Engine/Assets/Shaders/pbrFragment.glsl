#version 450 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BloomColor;

in vec2 UV;

struct DirectionalLight
{
	vec4 direction;
	vec4 color;
};

struct SpotLight
{
	vec4 position; //w = inner Radius
	vec4 color;    //w = outer Radius
	vec4 direction;
};

struct PointLight
{
	vec4 position;
	vec4 color;
};

struct LightData
{
	vec4 viewPos;
	int nDirectionalLights;
	int nSpotLights;
	int nPointLights;
};

uniform sampler2D albedo;
uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D iAo;
uniform sampler2D iRoughness;
uniform sampler2D iMetallic;

layout(std430,binding = 1) buffer readonly Lights_directional
{
	DirectionalLight dLights[];
};

layout(std430,binding = 2) buffer readonly Lights_spot
{
	SpotLight sLights[];
};

layout(std430,binding = 3) buffer readonly Lights_point
{
	PointLight pLights[];
};

layout (std140) uniform LightMeta{	LightData lightMeta; };

vec3 gammaCorrection(vec3 col)
{
	const float gamma = 0.9f;
	return pow(col, vec3(1.0 / gamma));
}

const float ambientStrength = 0.2f;
const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0- cosTheta, 5.0);
}

 float DistributionGGX(vec3 N,vec3 H,float roughness)
 {
	float a =roughness*roughness;
	float a2 =a*a;
	float NdotH =max(dot(N,H),0.0);
	float NdotH2=NdotH*NdotH;
	float num =a2;
	float denom=(NdotH2 * (a2-1.0)+1.0);
	denom = PI * denom * denom;
	return num/denom;
 }
 float GeometrySchlickGGX(float NdotV,float roughness)
 {
	float r=(roughness+1.0);
	float k=(r*r)/8.0;
	float num=NdotV;
	float denom=NdotV * (1.0-k)+k;					
	return num/denom;
 }

 float GeometrySmith(vec3 N,vec3 V,vec3 L,float roughness)
 {
	float NdotV = max(dot(N,V),0.0);
	float NdotL = max(dot(N,L),0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
 }

void main()
{
	vec3 o = vec3(0.0);
	vec3 pos = texture(position,UV).rgb;
	vec3 norm = normalize(texture(normal,UV).rgb);
	vec3 color = texture(albedo,UV).rgb;
	float ao = texture(iAo,UV).r;
	float roughness = texture(iRoughness,UV).r;
	float metallic = texture(iMetallic,UV).r;

	vec3 V = normalize(lightMeta.viewPos.rgb - pos.rgb);


	vec3 Lo = vec3(0.0f,0.0f,0.0f); //Final Output Var

	vec3 F0 = vec3(0.04);
    F0 = mix(F0, color, metallic);

	for(int i = 0; i < lightMeta.nPointLights; i++)
	{
		vec3 L = normalize(pLights[i].position.rgb - pos.rgb);
		vec3 H = normalize(V + L);
		float distance = length(pLights[i].position.rgb - pos.rgb);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = pLights[i].color.rgb * attenuation;

		float NDF = DistributionGGX(norm, H,roughness);
		float G = GeometrySmith(norm, V, L, roughness);
		vec3 F  = fresnelSchlick(max(dot(H, V),0.0),F0);
		
		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;
		
		vec3 numerator = NDF * G * F;
		float denominator=4.0 * max(dot(norm,V),0.0) * max(dot(norm,L),0.0);

		vec3 specular = numerator/max(denominator,0.001);
		float NdotL= max(dot(norm,L),0.0);
		Lo += (kD * color / PI + specular) * radiance * NdotL;
	}

	for(int i = 0; i < lightMeta.nDirectionalLights; i++) {
		vec3 L = normalize(-dLights[i].direction.rgb);
		vec3 H = normalize(V + L);
		vec3 radiance = dLights[i].color.rgb;

		float NDF = DistributionGGX(norm, H,roughness);
		float G = GeometrySmith(norm, V, L, roughness);
		vec3 F  = fresnelSchlick(max(dot(H, V),0.0),F0);
		vec3 kS = F;
		vec3 kD = vec3(1.0)-kS;
		kD *=1.0-metallic;
		vec3 numerator =NDF * G * F;
		float denominator=4.0 * max(dot(norm,V),0.0) * max(dot(norm,L),0.0);
		vec3 specular =numerator/max(denominator,0.001);
		float NdotL=max(dot(norm,L),0.0);
		Lo += (kD * color/PI+specular) * radiance * NdotL;
		}
	vec3 ambient=vec3(0.03) * color * ao;
	vec3 _color = ambient+Lo;
	_color = _color/(_color+vec3(1.0));
	_color = pow(_color,vec3(1.0/2.2));
	
	FragColor = vec4(_color,1.0);
    
	float brightness = dot(FragColor.rgb, vec3(0.5126, 0.7152, 0.5722));
	if(brightness>1.0f)
		BloomColor = vec4(_color,1.0);
	else
		BloomColor = vec4(0.0f,0.0f,0.0f,1.0f);

}


