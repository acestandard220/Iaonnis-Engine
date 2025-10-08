#version 450 core
//out vec4 FragColor;
layout(location = 0) out vec4 FragColor;
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

layout(std430,binding = 4) buffer readonly Lights_directional
{
	DirectionalLight dLights[];
};

layout(std430,binding = 5) buffer readonly Lights_spot
{
	SpotLight sLights[];
};

layout(std430,binding = 6) buffer readonly Lights_point
{
	PointLight pLights[];
};

layout (std140) uniform LightMeta{	LightData lightMeta; };

vec3 CalculateDirectionalLights(vec3 _pos,vec3 _norm,vec3 _color, DirectionalLight dl);
vec3 CalculateSpotLights(vec3 _pos,vec3 _norm,vec3 _color, SpotLight sl);
vec3 CalculatePointLights(vec3 _pos,vec3 _norm,vec3 _color, PointLight pl);

vec3 gammaCorrection(vec3 col)
{
	const float gamma = 0.9f;
	return pow(col, vec3(1.0 / gamma));
}

const float ambientStrength = 0.2f;

void main()
{
	vec3 o= vec3(0.0);
	vec3 pos = texture(position,UV).rgb;
	vec3 norm = normalize(texture(normal,UV).rgb);
	vec3 color = texture(albedo,UV).rgb;

	for(int i = 0;i < lightMeta.nDirectionalLights; i++)
	{
		o += vec3(CalculateDirectionalLights(pos,norm,color,dLights[i]));
	}

	for(int i = 0;i<lightMeta.nSpotLights;i++)
	{
		o += vec3(CalculateSpotLights(pos,norm,color,sLights[i]));
	}

	for(int i = 0;i<lightMeta.nPointLights;i++)
	{
		o += vec3(CalculatePointLights(pos,norm,color,pLights[i]));
	}

	const float gamma = 0.5f;
    vec3 hdrColor = o;

    vec3 mapped = hdrColor / (hdrColor + vec3(1.0));

	mapped = gammaCorrection(mapped);

	FragColor = vec4(mapped,1.0f);
}


vec3 CalculateDirectionalLights(vec3 _pos,vec3 _norm,vec3 _color, DirectionalLight dl)
{
	vec3 ambient = ambientStrength * dl.color.rgb;

	vec3 norm = _norm;

	vec3 lightDir = normalize( dl.direction.rgb);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3  diffuse = diff * dl.color.rgb;

	return vec3(ambient + diffuse * _color);
}


vec3 CalculateSpotLights(vec3 _pos,vec3 _norm,vec3 _color, SpotLight sl)
{
	vec3 ambient = ambientStrength * sl.color.rgb;

	vec3 lightDir = normalize(sl.position.rgb - _pos);

	float theta = dot(lightDir, normalize(-sl.direction.rgb));
	float epsilon = sl.position.w - sl.color.w; 
	float intensity = clamp((theta - sl.color.w) / epsilon, 0.0, 1.0);   

	if(theta > sl.color.w)
	{
	    vec3 lightDir = normalize(sl.position.rgb - _pos);
	    float diff = max(dot(lightDir, _norm),0.0);
	    vec3 diffuse = diff * sl.color.rgb;
	    
	    float specular_strength = 0.05;
	    vec3 viewDir = normalize(lightMeta.viewPos.rgb - _pos);
	    vec3 ref   = reflect(-lightDir, _norm);
	    float spec = pow(max(dot(ref, viewDir), 0.0), 32);
	    vec3 specular = spec * sl.color.rgb * specular_strength;
	    
		diffuse  *= intensity;
		specular *=intensity;

	     return vec3(ambient + diffuse + specular) * _color.rgb;
	}
	else{
		return vec3(ambient * _color.rgb);
	}
}

vec3 CalculatePointLights(vec3 _pos,vec3 _norm,vec3 _color, PointLight pl)
{
	vec3 ambient = ambientStrength * pl.color.rgb;

	vec3 lightDir = normalize(pl.position.rgb - _pos);
	float diff = max(dot(lightDir,_norm),0.0);
	vec3 diffuse = diff * pl.color.rgb;

	float specular_strength = 0.2;
	vec3 viewDir = normalize(lightMeta.viewPos.rgb - _pos);
	vec3 ref   = reflect(-lightDir, _norm);
	float spec = pow(max(dot(ref, viewDir), 0.0), 64);
	vec3 specular = spec * pl.color.rgb * specular_strength;

    return (ambient + diffuse + specular) * _color.rgb;
}