#include "world.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <png.h>
#include <cstdlib>
#include <iostream>

using namespace std;

int width = 500;
int height = 500;
bool rotate_o=false;
GLFWwindow* win;

void ogl_info(GLFWwindow* win)
{

	cout << "GL: " << glGetString(GL_VERSION) << endl;
	cout << "SL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	cout << flush;
}

int init_openGL()
{
	cout << "Initializing OpenGL ";
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API,GLFW_OPENGL_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);

	vector<int> vers = { 46,45,44,43,42,41,40,33 };
	for (auto v : vers)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,v/10);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,v%10);
		win = glfwCreateWindow(width,height,"OpenGL",NULL,NULL);
		if (win!=nullptr)
			break;
	}
	if (win==nullptr) {
		cout << "[\033[1;31mError\033[0m]" << endl;
		return 0;
	}
	glfwMakeContextCurrent(win);
	cout << "[\033[1;32mDone\033[0m]" << endl;

	ogl_info(win);
}

int main(int argc, char **argv)
{
	
	if(init_openGL()==0) return 0;

	world_init(width, height);
	
	glfwMakeContextCurrent(win);
	world_display(width,height);

	return 0;
}