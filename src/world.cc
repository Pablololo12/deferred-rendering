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
#include "ray_casting.h"

using namespace std;

GLuint svtx_frag;
GLuint sfrg_frag;

GLuint prog_frag;

GLuint vao;
GLuint vao_sz;

GLuint tex;
GLuint ten;

glm::mat4	view;

GLuint view_loc_frag;
GLuint textures_loc;

GLuint gBuffer;
GLuint gPosition;
GLuint gNormal;
GLuint gAlbedoSpec;
// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
const GLint samplers[1] = {0};

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

static OBJ obj;

void world_init(int width, int height)
{
	cout << "Initializing Buffers ";
	glm::mat4 xf = glm::rotate(glm::radians(90.0f),glm::vec3(5.0f,0.0f,0.0f));

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

	GLuint vtc = 0;
	glGenBuffers(1,&vtc);
	glBindBuffer(GL_ARRAY_BUFFER,vtc);
	glBufferData(GL_ARRAY_BUFFER,vao_sz*sizeof(glm::vec3),obj.texcoord().data(),GL_STATIC_DRAW);
	glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),NULL);

//	PNG textura("./tex/checker.png");
//	PNG textura("./tex/paper.png");
	PNG textura("./tex/lava.png");
	glGenTextures(1,&tex);
	glBindTexture(GL_TEXTURE_2D,tex);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB32F,textura.width(),textura.height(),0,GL_RGB,GL_FLOAT,textura.pixels().data());
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);//GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);//GL_LINEAR_MIPMAP_LINEAR


	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	  
	// - position buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	  
	// - normal buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// - Albedo and specular
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	glDrawBuffers(3, attachments);

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
	textures_loc = glGetUniformLocation(prog_frag, "textur");


	glClearColor(0,0,0,0);
	cout << "[\033[1;32mDone\033[0m]" << endl;
}

void world_reshape(int w,int h)
{
	glViewport(0,0,w,h);
}


//float world_ph=1.57;
float world_ph=1.0;
float world_th=0.5;
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
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_MULTISAMPLE);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


	glUseProgram(prog_frag);
	glUniformMatrix4fv(view_loc_frag,1,GL_FALSE,glm::value_ptr(view));

	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,tex);
	glUniform1iv(textures_loc , 1, samplers);

	glDrawArrays(GL_TRIANGLES,0,obj.faces().size());
	cout << "[\033[1;32mDone\033[0m]" << endl;

	glReadBuffer(GL_COLOR_ATTACHMENT0);
	float* positions = new float[3 * w * h];
	glReadPixels(0, 0, w, h, GL_RGB, GL_FLOAT, positions);
	save_image_vector(w, h, "position.png", positions);

	glReadBuffer(GL_COLOR_ATTACHMENT1);
	float* normals = new float[3 * w * h];
	glReadPixels(0, 0, w, h, GL_RGB, GL_FLOAT, normals);
	save_image_vector(w, h, "normals.png", normals);

	glReadBuffer(GL_COLOR_ATTACHMENT2);
	unsigned char* albedo = new unsigned char[4 * w * h];
	glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, albedo);
	save_image_vector_2(w, h, "albedo.png", albedo);

	ray_cast(obj, positions, normals, albedo, w, h, cam);
}

void world_clean()
{
}
