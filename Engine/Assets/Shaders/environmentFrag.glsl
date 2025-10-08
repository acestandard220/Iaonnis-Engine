#version 450 core
out vec4 FragColor;

in vec3 UV;

uniform samplerCube environmentMap;

void main()
{
	FragColor = texture(environmentMap,(UV));
	//FragColor = vec4(1.0,1.0f,1.0f,1.0f);
}