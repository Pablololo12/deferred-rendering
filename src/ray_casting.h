#include "obj.h"
#include "png.h"
#include <glm/glm.hpp>

unsigned char* ray_cast(OBJ, PNG, float*, float*, unsigned char*, int, int, glm::vec3, int);
glm::vec3 apply_color(glm::vec3, glm::vec3, glm::vec3, char, glm::vec3, int);
float toca_triangulo(glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3);
int get_triangle(glm::vec3, glm::vec3, float *);
glm::vec3 trace_reflected(glm::vec3, glm::vec3);
glm::vec3 color(glm::vec3, glm::vec3, float, unsigned int);