#version 450 core
layout(location = 0) in vec3 aPos;

out vec3 UV;

uniform mat4 ivp;

void main()
{
	UV = vec3(aPos);
	gl_Position = ivp * vec4(aPos,1.0);
}