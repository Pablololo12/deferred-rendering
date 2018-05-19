#version 330 core
out vec4 color;
uniform vec3 light_pos;
uniform vec3 camera;
uniform vec3 obj_color;
uniform vec3 light_color;
uniform vec4 dasr;
in vec3 pos;
in vec3 norm;
void main()
{
	vec3 norm2 = normalize(norm);
	vec3 vec_light = normalize(light_pos-pos);
	vec3 vec_v = normalize(pos-camera);
	vec3 specular_dir = reflect(-light_pos,norm2);
	vec3 reflected_dir = reflect(vec_v,norm2);

	float l_power = 1/pow(distance(light_pos,pos),2);
	float dot_l = dot(vec_light,normalize(norm2));

	vec3 difs = dot_l*l_power*obj_color*light_color; //Kd(N*L)Cd
	vec3 amb = obj_color*light_color; //ka Cd Cl
	vec3 spe = dot_l*pow(dot(specular_dir,vec_v),3)*obj_color*light_color*l_power; //ks (N*L)(R*V)Cs Cl

	color = vec4(difs*dasr.x+amb*dasr.y+spe*dasr.z,1.0);
}