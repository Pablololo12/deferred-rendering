#version 330 core
/*
 * Fragment shader for deferred
 */

uniform sampler2D textur[1];
in vec3 pos;
in vec3 norm;
in vec2 UV;

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

void main()
{
	gPosition = pos;
	gNormal = normalize(norm);
	gAlbedoSpec = vec4(texture(textur[0],UV).rgb,2.0);
}