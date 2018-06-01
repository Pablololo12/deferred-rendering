#version 330 core
/*
 * Vertex shader to show final image
 */
layout(location=0) in vec3 vpos;
layout(location=1) in vec3 vtc;

out vec3 pos;
out vec2 UV;

void main()
{
	UV = vtc.rg;
	pos = vpos;
	gl_Position = vec4(vpos,1.0);
}