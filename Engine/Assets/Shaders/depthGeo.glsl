#version 460 core
layout(triangles, invocations = 4) in;
layout(triangle_strip, max_vertices = 3) out;

struct CascadeMatrixSet
{
	mat4 matrix[4];
};

layout(std140) uniform LightMatrix
{
	CascadeMatrixSet lightMatix;
};

void main()
{
	for(int i = 0; i<3;i++)
	{
		gl_Position = lightMatix.matrix[gl_InvocationID] * gl_in[i].gl_Position;
		gl_Layer = gl_InvocationID;
		EmitVertex();
	}
	EndPrimitive();
}