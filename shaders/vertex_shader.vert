#version 330 core
layout(location=0) in vec3 vpos;
layout(location=1) in vec3 vnos;
uniform mat4 view;
out vec3 pos;
out vec3 norm;
void main()
{
   pos = vpos;
   norm = vnos;
   gl_Position = view*vec4(vpos,1.0);
}