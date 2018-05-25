#include "ray_casting.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "obj.h"
#include "png.h"
#include <iostream>

using namespace std;

glm::vec3 light_pos;
glm::vec3 light_pw = {0.02,0.02,0.02};
static OBJ obj;
float EPSILON = 0.00001;

glm::vec3 apply_color(glm::vec3 position, glm::vec3 normal,
				glm::vec3 difuse, char specular, glm::vec3 cam)
{
	// A por la luz
	glm::vec3 vector_luz = glm::normalize(light_pos-position);
	glm::vec3 position2 = position + normal*EPSILON*2;
	float distance;
	int ind = get_triangle(vector_luz,position2,&distance);

	if (ind != -1) {
		return {0,0,0};
	}

	float light = vector_luz.x*vector_luz.x + vector_luz.y*vector_luz.y + vector_luz.z*vector_luz.z;
	float dist_luz = sqrt(light);

	glm::vec3 color_luz = light_pw/dist_luz;

	glm::vec3 omegao = glm::normalize(cam-position);

	//Obtenemos omega r
	float dotproduc = vector_luz.x*normal.x + vector_luz.y*normal.y + vector_luz.z*normal.z;
	glm::vec3 omegar;
	omegar.x = vector_luz.x - 2*(vector_luz.x - normal.x*dotproduc);
	omegar.y = vector_luz.y - 2*(vector_luz.y - normal.y*dotproduc);
	omegar.z = vector_luz.z - 2*(vector_luz.z - normal.z*dotproduc);
	omegar = glm::normalize(omegar);

	float dotproductIntegral = glm::dot(omegar, normal);
	if (dotproductIntegral < 0) dotproductIntegral = -dotproductIntegral;

	// Por ultimo aplicamos phong
	float dotproductPhong = glm::dot(omegao, omegar);
	if (dotproductPhong < 0) dotproductPhong = -dotproductPhong;
	dotproductPhong = pow(dotproductPhong, specular);

	float especular = (specular + 2) / 2 * dotproductPhong;

	glm::vec3 rgb={0.0,0.0,0.0};
	rgb.r = color_luz.r * (difuse.r + especular) / M_PI * dotproductIntegral;
	rgb.g = color_luz.g * (difuse.g + especular) / M_PI * dotproductIntegral;
	rgb.b = color_luz.b * (difuse.b + especular) / M_PI * dotproductIntegral;

	return glm::clamp(rgb)*255;
}


unsigned char* ray_cast(OBJ object, float* positions, float* normals, unsigned char* albedo,
				int width, int height, glm::vec3 cam)
{
	obj = object;
	light_pos = cam;
	unsigned char* final = new unsigned char[3 * width * height];

	for (int i=0; i<height; i++) {
		for (int d=0,j=0; d<width*3; d=d+3, j=j+4) {
			int x = i*width*3+d;
			int x2 = i*width*4+j;
			glm::vec3 color = {0,0,0};
			glm::vec3 position = {positions[x],positions[x+1],positions[x+2]};
			if(position.x != 0.0 || position.y != 0.0 || position.z != 0.0) {
				glm::vec3 normal = {normals[x],normals[x+1],normals[x+2]};
				glm::vec3 difuse = {albedo[x2],albedo[x2+1],albedo[x2+2]};
				char specular = albedo[x2+3];
				color = apply_color(position, normal, difuse, specular, cam);
			}
			final[x] = color.r;
			final[x+1] = color.g;
			final[x+2] = color.b;
		}
	}
	return final;
}

float toca_triangulo(glm::vec3 origen, glm::vec3 vec, glm::vec3 V1, glm::vec3 V2, glm::vec3 V3)
{
	glm::vec3 e1 = {V2.x-V1.x, V2.y-V1.y, V2.z-V1.z};
	glm::vec3 e2 = {V3.x-V1.x, V3.y-V1.y, V3.z-V1.z};

	glm::vec3 P;
	P = glm::cross(vec,e2);

	float det = glm::dot(e1, P);

	if(det > -EPSILON && det < EPSILON) return -1.0;
	float inv_det = 1.0 / det;

	glm::vec3 T = {origen.x-V1.x, origen.y-V1.y, origen.z-V1.z};

	float u = glm::dot(T, P) * inv_det;

	if(u < 0.0 || u > 1.0) return -1.0;

	glm::vec3 Q = {0.0, 0.0, 0.0};
	Q = glm::cross(T,e1);

	float v = glm::dot(vec, Q) * inv_det;

	if(v < 0.0 || u + v > 1.0 ) return -1.0;

	float t = glm::dot(e2, Q) * inv_det;

	if(t > EPSILON) {
		return t;
	}

	return -1.0;
}

int get_triangle(glm::vec3 ray, glm::vec3 camera, float *distance)
{
	int vertex_number = obj.faces().size();
	float max_distance = std::numeric_limits<double>::infinity();
	int max_index = 0;

	for (int i = 0; i<vertex_number; i=i+3) {
		float distance = toca_triangulo(camera,ray,obj.faces()[i],obj.faces()[i+1],obj.faces()[i+2]);
		if (distance > -1.0 && max_distance > distance) {
			max_distance = distance;
			max_index = i;
		}
	}

	if (max_distance == std::numeric_limits<double>::infinity()) {
		*distance = max_distance;
		return -1;
	} else {
		*distance = max_distance;
		return max_index;
	}
}