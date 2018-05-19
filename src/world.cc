#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#define __gl_h_
	#include <OpenGL/glu.h>
	//#include <GLUT/glut.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
	//#include <GL/freeglut.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/polar_coordinates.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "world.h"
#include "obj.h"
#include "png.h"

using namespace std;

GLuint svtx_frag;
GLuint sfrg_frag;

GLuint prog_frag;

GLuint vao;
GLuint vao_sz;

GLuint tex;
GLuint ten;

glm::mat4	view;
glm::vec3   light_pos;
glm::vec3   light_color;
glm::vec3	obj_color;
glm::vec4	dasr;

GLuint		view_loc_frag;
GLuint light_loc_frag;
GLuint light_color_frag;
GLuint camera_loc_frag;
GLuint obj_color_loc_frag;
GLuint das_frag;
GLuint textures_loc;
const GLint samplers[2] = {0,1};

void glcheck(const string& msg)
{
	GLenum err;
	err = glGetError();
	switch(err)
	{
		case GL_NO_ERROR:
		break;
		case GL_INVALID_ENUM:
		cout << msg << " error: Invalid enun" << endl;
		break;
		case GL_INVALID_VALUE:
		cout << msg << " error: Invalid ID value" << endl;
		break;
		case GL_INVALID_OPERATION:
		cout << msg << " error: Invalid operation" << endl;
		break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
		cout << msg << " error: Invalid framebuffer operation" << endl;
		break;
		case GL_OUT_OF_MEMORY:
		cout << msg << " error: Out of memory" << endl;
		break;
		default:
		cout << msg << " error: Undefined" << endl;
		break;
	}
}

string get_shader(string filename)
{
	ifstream is;

	is.open(filename);
	if (!is)
		cerr << "Can't open file " << filename << endl;
	stringstream buffer;
	buffer << is.rdbuf();
	is.close();
	return buffer.str();
}

OBJ obj;

void world_init()
{
	cout << "Initializing Buffers ";
	glm::mat4 xf = glm::rotate(glm::radians(90.0f),glm::vec3(1.0f,0.0f,0.0f));

//	obj.load("./model/cube.obj");
	obj.load("./model/teapot.obj",xf);
//	obj.load("./model/sphere.obj");
//	obj.load("./model/venus.obj",xf);
//	obj.load("./model/bunny.obj",xf);
//	obj.load("./model/dragon.obj",xf);
//	obj.load("./model/armadillo.obj",xf);
//	obj.load("./model/tyra.obj",xf);
//	obj.load("./model/nefertiti.obj");
//	obj.load("./model/Interceptor.obj",xf);
//	obj.load("./model/bb8.obj",xf);

	//cout << obj.faces().size()/3 << endl;

	vao = 0;
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);
	vao_sz = obj.faces().size();
	// Vertices y triangulos
	GLuint vpbo = 0;
	glGenBuffers(1,&vpbo);
	glBindBuffer(GL_ARRAY_BUFFER,vpbo);
	glBufferData(GL_ARRAY_BUFFER,vao_sz*sizeof(glm::vec3),obj.faces().data(),GL_STATIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),NULL);
	// Normales
	GLuint vnbo = 0;
	glGenBuffers(1,&vnbo);
	glBindBuffer(GL_ARRAY_BUFFER,vnbo);
	glBufferData(GL_ARRAY_BUFFER,vao_sz*sizeof(glm::vec3),obj.normals().data(),GL_STATIC_DRAW);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),NULL);

	const char * tmp;
	// Shader en fragmento
	string f_vertex_src = get_shader("shaders/vertex_shader.vert");
	string f_fragment_src = get_shader("shaders/fragment_shader.frag");
	svtx_frag = glCreateShader(GL_VERTEX_SHADER);
	tmp = f_vertex_src.c_str();
	glShaderSource(svtx_frag,1,&tmp,NULL);
	glCompileShader(svtx_frag);

	sfrg_frag = glCreateShader(GL_FRAGMENT_SHADER);
	tmp = f_fragment_src.c_str();
	glShaderSource(sfrg_frag,1,&tmp,NULL);
	glCompileShader(sfrg_frag);

	prog_frag = glCreateProgram();
	glAttachShader(prog_frag,svtx_frag);
	glAttachShader(prog_frag,sfrg_frag);
	glLinkProgram(prog_frag);

	view_loc_frag = glGetUniformLocation(prog_frag,"view");
	light_loc_frag = glGetUniformLocation(prog_frag,"light_pos");
	light_color_frag = glGetUniformLocation(prog_frag,"light_color");
	camera_loc_frag = glGetUniformLocation(prog_frag,"camera");
	obj_color_loc_frag = glGetUniformLocation(prog_frag,"obj_color");
	das_frag = glGetUniformLocation(prog_frag,"dasr");

	light_pos = {0.0,1.0,0.0};
	light_color = {2.0,2.0,2.0};
	obj_color = {1.0,1.0,1.0};
	dasr = {0.7,0.0,0.3,1.0};

	glClearColor(0,0,0,0);
	cout << "[\033[1;32mDone\033[0m]" << endl;
}

void world_reshape(int w,int h)
{
//	cout << w << " x " << h << endl;
	glViewport(0,0,w,h);
}


float world_ph=0;
float world_th=0;
float world_ro=2;

bool world_fill=true;
bool world_shading_frag=true;

void world_display(int w,int h)
{
	cout << "Creating image ";
	if (h<=0) return;
	if (w<=0) return;

	float aspect = float(w)/float(h);

	glm::mat4 pers = glm::perspective(glm::radians(45.0f),aspect,0.01f,1000.0f);

	glm::vec3 axis;
	axis.x = cos(world_ph)*cos(world_th);
	axis.y = sin(world_ph)*cos(world_th);
	axis.z = sin(world_th);

	glm::vec3 to(0,0,0);
	glm::mat4 camera = glm::lookAt(to+world_ro*axis,to,glm::vec3(0,0,1));
	glm::vec3 cam = to+world_ro*axis;
	view = pers*camera;

	glPolygonMode(GL_FRONT_AND_BACK,(world_fill ? GL_FILL : GL_LINE));
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


	glUseProgram(prog_frag);
	glUniform3fv(camera_loc_frag,1, glm::value_ptr(cam));
	glUniform3fv(light_loc_frag,1, glm::value_ptr(cam));
	glUniform3fv(light_color_frag,1, glm::value_ptr(light_color));
	glUniform3fv(obj_color_loc_frag,1, glm::value_ptr(obj_color));
	glUniform4fv(das_frag,1, glm::value_ptr(dasr));
	glUniformMatrix4fv(view_loc_frag,1,GL_FALSE,glm::value_ptr(view));


	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glDrawArrays(GL_TRIANGLES,0,obj.faces().size());
	cout << "[\033[1;32mDone\033[0m]" << endl;
}

void world_clean()
{
}
