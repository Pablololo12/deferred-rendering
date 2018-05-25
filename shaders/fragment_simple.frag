#version 330 core
uniform sampler2D textur[1];

in vec3 pos;
in vec2 UV;
out vec4 color;

void main()
{
	color = vec4(texture(textur[0],UV).rgb,1.0);
	//color = vec4(1.0,1.0,1.0,1.0);
}