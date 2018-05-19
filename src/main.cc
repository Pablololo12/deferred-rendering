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

void save_image()
{
	int i,d;
	png_bytep row = NULL;
	cout << "Taking Snapshot ";
	unsigned char* pixels = new unsigned char[3 * width * height];
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	FILE *fp = fopen("snap.png", "wb");
	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info = png_create_info_struct(png);
	if (setjmp(png_jmpbuf(png))){
		cout << "Error taking snapshot" << endl;
	}
	png_init_io(png, fp);
	png_set_IHDR(
		png,
		info,
		width, height,
		8,
		PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);
	png_write_info(png, info);
	
	row = (png_bytep) malloc(3 * width * sizeof(png_byte));
	for (i=height-1; i>=0; i--) {
		for (d=0;d<width*3;d++) {
			row[d]=pixels[i*width*3+d];
		}
		png_write_row(png, row);
	}
	free(row);
	png_write_end(png, NULL);
	fclose(fp);
	cout << "[\033[1;32mDone\033[0m]" << endl;
}

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

	//ogl_info(win);
}

int main(int argc, char **argv)
{
	
	if(init_openGL()==0) return 0;

	world_init();
	
	glfwMakeContextCurrent(win);
	world_display(width,height);

	save_image();

	return 0;
}