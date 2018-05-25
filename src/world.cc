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
GLuint svtx2_frag;
GLuint sfrg_frag;
GLuint sfrg2_frag;
GLuint simple_frag;
GLuint simple_vert;

GLuint prog_frag;
GLuint prog_rast_frag;
GLuint prog_simple;

GLuint vao;
GLuint vao_sz;
GLuint vau;

GLuint tex;
GLuint ten;
GLuint tex2;

glm::mat4	view;

GLuint view_loc_frag;
GLuint textures_loc;
GLuint view_loc2_frag;
GLuint textures_loc2;
GLuint light_pos_frag;
GLuint camera_frag;
GLuint light_color_frag;

GLuint simple_texture_loc;

GLuint gBuffer;
GLuint gPosition;
GLuint gNormal;
GLuint gAlbedoSpec;
GLuint depthRenderBuffer;
// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
const GLint samplers[1] = {0};

static std::vector<glm::vec3> square {{-1.0,-1.0,0.0},
									{1.0,-1.0,0.0},
									{1.0,1.0,0.0},
									{-1.0,1.0,0.0},
									{1.0,1.0,0.0},
									{-1.0,-1.0,0.0}};

static std::vector<glm::vec3> squareUV {{0.0,0.0,0.0},
								{1.0,0.0,0.0},
								{1.0,1.0,0.0},
								{0.0,1.0,0.0},
								{1.0,1.0,0.0},
								{0.0,0.0,0.0}};

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
	
	glm::mat4 xf = glm::rotate(glm::radians(90.0f),glm::vec3(1.0f,0.0f,0.0f));

	obj.load("./model/teapot.obj",xf);
	//obj.load("./model/cube.obj",xf);

	cout << obj.faces().size()/3 << endl;
	cout << "Initializing Buffers ";
	
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
	glcheck("vpbo");
	// Normales
	GLuint vnbo = 0;
	glGenBuffers(1,&vnbo);
	glBindBuffer(GL_ARRAY_BUFFER,vnbo);
	glBufferData(GL_ARRAY_BUFFER,vao_sz*sizeof(glm::vec3),obj.normals().data(),GL_STATIC_DRAW);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),NULL);
	glcheck("vnbo");

	GLuint vtc = 0;
	glGenBuffers(1,&vtc);
	glBindBuffer(GL_ARRAY_BUFFER,vtc);
	glBufferData(GL_ARRAY_BUFFER,vao_sz*sizeof(glm::vec3),obj.texcoord().data(),GL_STATIC_DRAW);
	glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),NULL);
	glcheck("vtc");

	vau = 0;
	glGenVertexArrays(1,&vau);
	glBindVertexArray(vau);
	GLuint vau_sz = square.size();

	GLuint vpbu = 0;
	glGenBuffers(1,&vpbu);
	glBindBuffer(GL_ARRAY_BUFFER,vpbu);
	glBufferData(GL_ARRAY_BUFFER,vau_sz*sizeof(glm::vec3),square.data(),GL_STATIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),NULL);
	glcheck("vpbu");

	GLuint uvpbu = 0;
	glGenBuffers(1,&uvpbu);
	glBindBuffer(GL_ARRAY_BUFFER,uvpbu);
	glBufferData(GL_ARRAY_BUFFER,vau_sz*sizeof(glm::vec3),squareUV.data(),GL_STATIC_DRAW);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),NULL);
	glcheck("uvpbu");



//	PNG textura("./tex/checker.png");
//	PNG textura("./tex/paper.png");
//	PNG textura("./tex/lava.png");
	PNG textura("./tex/wood.png");

	glGenTextures(1,&tex);
	glBindTexture(GL_TEXTURE_2D,tex);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB32F,textura.width(),textura.height(),0,GL_RGB,GL_FLOAT,textura.pixels().data());
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glcheck("Texture");

	glGenRenderbuffers( 1, &depthRenderBuffer );
	glBindRenderbuffer( GL_RENDERBUFFER, depthRenderBuffer );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT,width,height);
	glcheck("Depth");

	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	  
	// - position buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	glcheck("Position Buffer");
	  
	// - normal buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	glcheck("Normal Buffer");

	// - Albedo and specular
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
	glcheck("Albedo Buffer");
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
	glcheck("Atach");

	//glDrawBuffers(3, attachments);

	const char * tmp;
	// Shader en fragmento
	string f_vertex_src = get_shader("shaders/vertex_shader.vert");
	string f_fragment_src = get_shader("shaders/fragment_shader.frag");
	glcheck("Really?");

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

	string f_fragment_2_src = get_shader("shaders/fragment_raster_shader.frag");
	sfrg2_frag = glCreateShader(GL_FRAGMENT_SHADER);
	tmp = f_fragment_2_src.c_str();
	glShaderSource(sfrg2_frag,1,&tmp,NULL);
	glCompileShader(sfrg2_frag);

	svtx2_frag = glCreateShader(GL_VERTEX_SHADER);
	tmp = f_vertex_src.c_str();
	glShaderSource(svtx2_frag,1,&tmp,NULL);
	glCompileShader(svtx2_frag);

	prog_rast_frag = glCreateProgram();
	glAttachShader(prog_rast_frag,svtx2_frag);
	glAttachShader(prog_rast_frag,sfrg2_frag);
	glLinkProgram(prog_rast_frag);

	string simple_vertex_src = get_shader("shaders/vertex_simple.vert");
	string simple_fragment_src = get_shader("shaders/fragment_simple.frag");

	simple_vert = glCreateShader(GL_VERTEX_SHADER);
	tmp = simple_vertex_src.c_str();
	glShaderSource(simple_vert,1,&tmp,NULL);
	glCompileShader(simple_vert);

	simple_frag = glCreateShader(GL_FRAGMENT_SHADER);
	tmp = simple_fragment_src.c_str();
	glShaderSource(simple_frag,1,&tmp,NULL);
	glCompileShader(simple_frag);

	prog_simple = glCreateProgram();
	glAttachShader(prog_simple,simple_vert);
	glAttachShader(prog_simple,simple_frag);
	glLinkProgram(prog_simple);

	view_loc_frag = glGetUniformLocation(prog_frag, "view");
	textures_loc = glGetUniformLocation(prog_frag, "textur");

	view_loc2_frag = glGetUniformLocation(prog_rast_frag, "view");
	textures_loc2 = glGetUniformLocation(prog_rast_frag, "textur");
	light_pos_frag = glGetUniformLocation(prog_rast_frag, "light_pos");
	camera_frag = glGetUniformLocation(prog_rast_frag, "camera");
	light_color_frag = glGetUniformLocation(prog_rast_frag, "light_color");

	simple_texture_loc = glGetUniformLocation(prog_simple, "textur");
	glcheck("Location");


	glClearColor(0.0,0.0,0.0,0.0);
	//glClearDepth(0.0f);
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

glm::vec3 light_color = {3.5,3.5,3.5};

void world_display(int w,int h, int option, int take_screenshot)
{
	//cout << "Creating image ";
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
	cam.y = -cam.y;
	view = pers*camera;

	glPolygonMode(GL_FRONT_AND_BACK,(world_fill ? GL_FILL : GL_LINE));
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glEnable(GL_MULTISAMPLE);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	if (option == 0) {
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glUseProgram(prog_rast_frag);

		glUniform3fv(camera_frag,1, glm::value_ptr(cam));
		glUniform3fv(light_pos_frag,1, glm::value_ptr(cam));
		glUniform3fv(light_color_frag,1, glm::value_ptr(light_color));
		glUniformMatrix4fv(view_loc2_frag,1,GL_FALSE,glm::value_ptr(view));
		glUniform1iv(textures_loc2 , 1, samplers);
	} else {
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		glUseProgram(prog_frag);
		glDrawBuffers(3, attachments);
		glUniformMatrix4fv(view_loc_frag,1,GL_FALSE,glm::value_ptr(view));
		glUniform1iv(textures_loc , 1, samplers);
	}

	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,tex);
	glcheck("mano");


	glDrawArrays(GL_TRIANGLES,0,obj.faces().size());
	//cout << "[\033[1;32mDone\033[0m]" << endl;

	if (option == 1) {
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		float* positions = new float[3 * w * h];
		glReadPixels(0, 0, w, h, GL_RGB, GL_FLOAT, positions);

		glReadBuffer(GL_COLOR_ATTACHMENT1);
		float* normals = new float[3 * w * h];
		glReadPixels(0, 0, w, h, GL_RGB, GL_FLOAT, normals);

		glReadBuffer(GL_COLOR_ATTACHMENT2);
		unsigned char* albedo = new unsigned char[4 * w * h];
		glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, albedo);

		unsigned char * final = ray_cast(obj, positions, normals, albedo, w, h, cam);

		if (take_screenshot==1) {
			save_image_vector(w, h, "position.png", positions);
			save_image_vector(w, h, "normals.png", normals);
			save_image_vector_2(w, h, "albedo.png", albedo);
			save_image_vector_3(w, h, "final.png", final);
		}
		glGenTextures(1,&tex2);
		glBindTexture(GL_TEXTURE_2D,tex2);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,w,h,0,GL_RGB,GL_UNSIGNED_BYTE,final);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_LINEAR
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glcheck("Texture");

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glUseProgram(prog_simple);

		glBindVertexArray(vau);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,tex2);
		glUniform1iv(simple_texture_loc , 1, samplers);

		glDrawArrays(GL_TRIANGLES,0,square.size());
		glcheck("Draw Square");

		glDeleteTextures(1,&tex2);
		delete final;
	}
}

void world_clean()
{
}
