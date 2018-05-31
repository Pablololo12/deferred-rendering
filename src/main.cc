#include "world.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <png.h>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <sstream>

using namespace std;

int width = 500;
int height = 500;
bool rotate_o=false;
int ray_casting = 0;
int take_screenshot = 0;
int ray_reflected = 0;
GLFWwindow* win;

void ogl_info(GLFWwindow* win);
void ogl_reshape(GLFWwindow* win,int width,int height);
void ogl_display(GLFWwindow* win);
void setWindowFPS (GLFWwindow* win);

void keyboard(GLFWwindow* win,int key,int s,int act,int mod);
void scroll(GLFWwindow* win,double x,double y);
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

	vector<int> vers { 42,41,40,33 };
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

	glfwSetFramebufferSizeCallback(win,ogl_reshape);
	glfwSetWindowRefreshCallback(win,ogl_display);
	glfwMakeContextCurrent(win);
	glfwSetWindowRefreshCallback(win,ogl_display);
	glfwSetKeyCallback(win,keyboard);
	glfwSetScrollCallback(win,scroll);

	ogl_display(win);
	//world_display(width,height);
	while (!glfwWindowShouldClose(win))
	{
		if(!rotate_o){
			glfwWaitEvents();
		}
		else {
			glfwWaitEventsTimeout(0.016667); // 60 FPS
			world_ph += 0.01;
			ogl_display(win);
		}
		setWindowFPS(win);
	}

	glfwTerminate();


	return 0;
}

void setWindowFPS (GLFWwindow* win)
{
	static int nbFrames = 0;
	static int lastTime = 0;
	double currentTime = glfwGetTime();
	double delta = currentTime - lastTime;
	nbFrames++;
	if ( delta >= 1.0 ){

	 double fps = double(nbFrames) / delta;

	 std::stringstream ss;
	 if (ray_casting==0) {
	 	ss << "Raster";
	 } else {
	 	ss << "Deferred";
	 }
	 ss << " [" << fps << " FPS]";

	 glfwSetWindowTitle(win, ss.str().c_str());

	 nbFrames = 0;
	 lastTime = currentTime;
	}
}

void ogl_reshape(GLFWwindow* win,int w,int h)
{
	glfwMakeContextCurrent(win);
	width = w;
	height = h;

	world_reshape(w,h);
}

void ogl_display(GLFWwindow* win)
{
	int w,h;
	glfwGetWindowSize(win,&w,&h);

	glfwMakeContextCurrent(win);

	world_display(w,h,ray_casting,take_screenshot, ray_reflected);
	take_screenshot = 0;

	glfwSwapBuffers(win);
}

void keyboard(GLFWwindow* win,int key,int s,int act,int mod)
{
	if (act==GLFW_RELEASE)
		return;

	switch (key)
	{
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(win,GL_TRUE);
			break;
		case GLFW_KEY_LEFT:
			world_ph += 0.1;
			break;
		case GLFW_KEY_RIGHT:
			world_ph -= 0.1;
			break;
		case GLFW_KEY_DOWN:
			world_th += 0.1;
			if (world_th>0.49*M_PI)
				world_th = 0.49*M_PI;
			break;
		case GLFW_KEY_UP:
			world_th -= 0.1;
			if (world_th<-0.49*M_PI)
				world_th = -0.49*M_PI;
			break;
		case GLFW_KEY_R:
			world_ph = 0;
			world_th = 0;
			break;
		case GLFW_KEY_F:
			world_fill = !world_fill;
			break;
		case GLFW_KEY_P:
			rotate_o = !rotate_o;
			break;
		case GLFW_KEY_S:
			ray_casting = !ray_casting;
			break;
		case GLFW_KEY_W:
			take_screenshot = 1;
			break;
		case GLFW_KEY_E:
			ray_reflected = !ray_reflected;
			break;
		default:
			cout << "key " << key << "<" << char(key) << ">" << endl;
			break;
	}

	ogl_display(win);
}

void scroll(GLFWwindow* win,double,double z)
{
	if (z<0)
		world_ro *= 0.90;
	else
		world_ro *= 1.10;

	ogl_display(win);
}