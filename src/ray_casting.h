#include "obj.h"
#include <glm/glm.hpp>

unsigned char* ray_cast(OBJ, float*, float*, unsigned char*, int, int, glm::vec3);
glm::vec3 apply_color(glm::vec3, glm::vec3, glm::vec3, char, glm::vec3);
float toca_triangulo(glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3);
int get_triangle(glm::vec3, glm::vec3, float *);