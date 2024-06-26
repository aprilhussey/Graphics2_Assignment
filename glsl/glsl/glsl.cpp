// glsl.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <Windows.h>

#define _USE_MATH_DEFINES
#include <cmath>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/type_precision.hpp>
#include <glew/glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "glut.h"
#include "glsl.h"
#include "imageloader.h"

using namespace std;
using glm::mat4;
using glm::mat3;
using glm::vec3;
using glm::vec4;

// rotation, scaling
float  angle_y = 0.0;
float  angle_x = 0.0;
float  scale   = 1.0;
float  angle   = 0.0;

// VAO, shader program object
GLuint vertexArrayObj;
GLuint shaderProgramObj; 
GLuint vao;

// torus
int  slices = 500;
int  stacks = 1000;

// view, model, projection matrices
mat4 viewMatrix, projMatrix, modelMatrix, mvp;
// modelview 
mat4 mvMatrix;
// normal matrix
mat3 normalMatrix;

// texture ID
GLuint tid[2];

int size;

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowPosition(10, 10);
	glutInitWindowSize(600, 600);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("A simple glut window");	
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutMouseFunc(mouse_click);
	glutMotionFunc(mouse_motion);
	glutReshapeFunc(reshape);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cout << "Error initializing glew:  " << glewGetErrorString(err) << std::endl;
	}

	init();

	glutMainLoop();

	return 0;
}

void reshape(int w, int h) {
	if (h == 0) h = 1;
	glViewport(0, 0, w, h);
	projMatrix = glm::perspective(45.0f, (float)w / h, 3.0f, 100.0f);
}
void display() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	modelMatrix = glm::rotate(mat4(1.0f), angle, vec3(1.0, 0.0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, angle, vec3(0.0, 1.0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, angle_y, vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, angle_x, vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, vec3(scale, scale, scale));
	
	// mv, mvp, normalMatrix
	mvMatrix     = viewMatrix * modelMatrix;
	mvp          = projMatrix * mvMatrix;

	GLuint locationMV  = glGetUniformLocation(shaderProgramObj, "MV");
	GLuint locationMVP = glGetUniformLocation(shaderProgramObj, "MVP");
	glUniformMatrix4fv(locationMV,  1, GL_FALSE, &mvMatrix[0][0]);
	glUniformMatrix4fv(locationMVP, 1, GL_FALSE, &mvp[0][0]);
	
	int loc = glGetUniformLocation(shaderProgramObj, "Tex1");
	glUniform1i(loc, 0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tid[0]);

	glBindVertexArray(vertexArrayObj);
	glDrawArrays(GL_TRIANGLES, 0, 2 * 3 * stacks * slices);

	glutSwapBuffers();
}

void idle() {
	angle += 0.1;
	glutPostRedisplay();
}

// points on cross section on z = 0 plane
void stack(const point& c, float r, int stacks, point* p) {
	float theta = 2.0 * M_PI / stacks;
	for (int i = 0; i < stacks; i++) {
		p[i].x = r * cos(theta * i) + c.x;
		p[i].y = r * sin(theta * i) + c.y;
		p[i].z = 0.0 + c.z;
	}
}

void torus(float r1, float r2, int stacks, int slices, float vertices[], float vertNormal[], float texData[]) {
	// r1: inner radius, r2: outer radius
	// the circle centre on the x-y plane
	point  c;
	c.x = (r1 + r2) / 2.0; c.y = 0.0; c.z = 0.0;

	point* p  = new point[stacks];
	point* pn = new point[stacks];
	stack(c, (r2 - r1) / 2.0, stacks, p);  // the cross section, i.e, the circle on z=0 plane

	point v1, v2, v;
	int k = 0;
	int m = 0;
	int t = 0;
	float theta =  2.0 * 3.1415 / slices;
	for (int segment = 1; segment <= slices; segment++) {
		// create next cross section, by rotating the first cross section
		if (segment <= slices - 1)
			for (int i = 0; i < stacks; i++) {
				pn[i].x =  p[i].x * cos(theta) + p[i].z * sin(theta);
				pn[i].y =  p[i].y;
				pn[i].z = -p[i].x * sin(theta) + p[i].z * cos(theta);
			}
		else 
			stack(c, (r2 - r1) / 2.0, stacks, pn);
		int j = 0;
		for(int i = 0; i < stacks; i++) {
			j = i + 1;
			if (j == stacks) j = 0;
			float s = 1.0 / (2.0 * r2);
			// quad 0: 001, 101
			// triangle: 001
			// point 0
			vertices[k] = p[i].x; k++;
			vertices[k] = p[i].y; k++;
			vertices[k] = p[i].z; k++;
			texData[t]  = p[i].x * s; t++;
			texData[t]  = p[i].z * s; t++;
			// point 0
			vertices[k] = pn[i].x; k++;
			vertices[k] = pn[i].y; k++;
			vertices[k] = pn[i].z; k++;
			texData[t]  = pn[j].x * s; t++;
			texData[t]  = pn[j].z * s; t++;
			// point 1
			vertices[k] = pn[j].x; k++;
			vertices[k] = pn[j].y; k++;
			vertices[k] = pn[j].z; k++;
			texData[t]  = pn[j].x * s; t++;
			texData[t]  = pn[j].z * s; t++;
			// triangle: 101
			// point 1
			vertices[k] = pn[j].x; k++;
			vertices[k] = pn[j].y; k++;
			vertices[k] = pn[j].z; k++;
			texData[t]  = pn[j].x * s; t++;
			texData[t]  = pn[j].z * s; t++;
			// point 1
			vertices[k] = p[j].x; k++;
			vertices[k] = p[j].y; k++;
			vertices[k] = p[j].z; k++;
			texData[t]  = p[j].x * s; t++;
			texData[t]  = p[j].z * s; t++;
			// point 0
			vertices[k] = p[i].x; k++;
			vertices[k] = p[i].y; k++;
			vertices[k] = p[i].z; k++;
			texData[t]  = p[j].x * s; t++;
			texData[t]  = p[j].z * s; t++;
			// normal at vertex, triangle 001, 101 share the same normal
			v1.x = pn[i].x - p[i].x;
			v1.y = pn[i].y - p[i].y;
			v1.z = pn[i].z - p[i].z;
			v2.x = pn[j].x - pn[i].x;
			v2.y = pn[j].y - pn[i].y;
			v2.z = pn[j].z - pn[i].z;
			crossProduct(v1, v2, v);
			for (int n = 0; n < 6; n++) {
				vertNormal[m] = v.x; m++;
				vertNormal[m] = v.y; m++;
				vertNormal[m] = v.z; m++;
			}
		}
		for (int i = 0; i < stacks; i++) {
			p[i].x = pn[i].x;
			p[i].y = pn[i].y;
			p[i].z = pn[i].z;
		}
	}
	delete[] p;
	delete[] pn;
}

void crossProduct(const point& v1, const point& v2, point& v) {
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
}

void init() {
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// texture
	const char *texName[2] ={"checkerboard.bmp", "t.bmp"};
	Image *image0 = loadBMP(texName[0]);
	Image *image1 = loadBMP(texName[1]);

	glEnable(GL_TEXTURE_2D);
	glGenTextures(2, tid);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tid[0]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image0->width, image0->height, GL_RGB, GL_UNSIGNED_BYTE, image0->pixels);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tid[1]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image1->width, image1->height, GL_RGB, GL_UNSIGNED_BYTE, image1->pixels);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glDisable(GL_TEXTURE_2D);

	/*
	1. create shader programmes using glsl and save them as text files 
	-- glCreateShader()
	2. create corresponding shader objects, and load shader programmes into 
	shader objects (from text files) 
	-- glShaderSource()
	3. compile shader objects 
	-- glCompileShader() 
	4. create shader programme object 
	-- glCreateProgram();
	5. attach shader objects to shader programme object 
	-- glAttachShader()
	6. bind 'in' variables in vertex shader programme to generic vertex attribute indices, ie. 0, 1, etc
	-- glBindAttribLocation()
	7. link shader programme 
	--glLinkProgram()
	8. create vertex buffer names/objects (2 names in this case, as there are 2 'in' variables in vertex shader)
	-- glGenBuffers()
	9. bind vertex buffer names/objects
	*/

	//create vertex shader object
	glEnableClientState(GL_VERTEX_ARRAY);
	GLuint vertShaderObj = glCreateShader(GL_VERTEX_SHADER); // GL_VERTEX_SHADER: vertex shader
	if (0 == vertShaderObj) {
		std::cout << "error creating vertex shader. " << std::endl;
		exit(1);
	}
	//load shader from disk to shader object
	shaderSource(vertShaderObj, "vertshader.vert");	
	//compile shader
	compileShader(vertShaderObj, "vertex shader");
	//create fragment shader object
	GLuint fragShaderObj = glCreateShader(GL_FRAGMENT_SHADER); // GL_FRAGMENT_SHADER: fragment shader
	if (0 == fragShaderObj) {
		std::cout << "error creating fragment shader. " << std::endl;
		exit(1);
	}
	//load shader from disk to shader object
	shaderSource(fragShaderObj, "fragshader.vert");	
	//compile shader
	compileShader(fragShaderObj, "fragment shader");
	shaderProgramObj = glCreateProgram();  // shaders are organised in a shader program
	if (0 == shaderProgramObj) {
		std::cout << "error creating shader programme object" << std::endl;
		exit(1);
	}
	glAttachShader(shaderProgramObj, vertShaderObj);
	glAttachShader(shaderProgramObj, fragShaderObj);             // attach shaders to shader program
	// the indices of these generic vertex attributes can be obtained using glGetAttribLocation(shaderProgram, "generic attrib")
	glLinkProgram(shaderProgramObj);
	GLint status;
	glGetProgramiv(shaderProgramObj, GL_LINK_STATUS, &status);
	if (GL_FALSE == status) {
		std::cout << "failed to link shader programme" << std::endl;
		GLint logLen;
		glGetProgramiv(shaderProgramObj, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			char *log = (char*)malloc(logLen);
			GLsizei written;
			glGetShaderInfoLog(shaderProgramObj, logLen, &written, log);
			std::cout << "programme log: " << log << std::endl;
			free(log);
		}
	} else {
		// if shader program installed successfully, then use the shader program
		glUseProgram(shaderProgramObj);
		std::cout << "linking is successful, and shader programme object has been installed into OpenGL pipeline" << std::endl;
	}

	viewMatrix = glm::lookAt(vec3(0.0, 0.0, 5.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
	GLuint location = glGetUniformLocation(shaderProgramObj, "View");
	glUniform3fv(location, 1, &viewMatrix[0][0]);

	struct lightInfo {
		vec4 lightPosition;
		vec3 La;
		vec3 L;
	};
	lightInfo light;
	light.lightPosition = vec4(0.0, 9.0, 9.0, 1.0);
	light.La = vec3(0.3, 0.3, 0.3);
	light.L  = vec3(1.0, 1.0, 1.0);
	GLuint location0 = glGetUniformLocation(shaderProgramObj, "Light.LightPosition");
	GLuint location1 = glGetUniformLocation(shaderProgramObj, "Light.L");
	GLuint location2 = glGetUniformLocation(shaderProgramObj, "Light.La");
	glUniform4fv(location0, 1, &light.lightPosition[0]);
	glUniform3fv(location1, 1, &light.L[0]);
	glUniform3fv(location2, 1, &light.La[0]);

	struct materialInfo {
		vec3 Ka;
		vec3 Kd;
		vec3 Ks;
		GLfloat  Shininess;
	};
	materialInfo material;
	material.Ka = vec3(0.3, 0.3, 0.3);
	material.Kd = vec3(0.9, 0.0, 0.0);
	material.Ks = vec3(0.8, 0.8, 0.8);
	material.Shininess = 250; 
	GLuint location4 = glGetUniformLocation(shaderProgramObj, "Material.Ka");
	GLuint location5 = glGetUniformLocation(shaderProgramObj, "Material.Kd");
	GLuint location6 = glGetUniformLocation(shaderProgramObj, "Material.Ks");
	GLuint location7 = glGetUniformLocation(shaderProgramObj, "Material.Shininess");
	glUniform4fv(location4, 1, &material.Ka[0]);
	glUniform3fv(location5, 1, &material.Kd[0]);
	glUniform3fv(location6, 1, &material.Ks[0]);
	glUniform1f(location7, material.Shininess); // for passing single value

	// generate vertices data
	float * positionData = new float[2 * 3 * 3 * stacks * slices];
	float * normalData   = new float[2 * 3 * 3 * stacks * slices];  // 2 * stacks = number of triangles, 
	// 3 * (2 * stacks): each triangle has 3 vertecies, 
	// 3 * (3 * 2 * stacks): each vertex has 3 coordinates
	// slices: number of segments;
	float * texData      = new float[2 * 2 * 3 * stacks * slices];
	torus(0.4, 1.0, stacks, slices, positionData, normalData, texData);      // triangle mesh for torus
	
	// create 3 VBOs, one for texutre
	GLuint vertexBufferObjs[3];
	glGenBuffers(3, vertexBufferObjs);
	// active the first VBO for position
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjs[0]);
	// load data from 1-D array positionData[] of position to the first VBO
	glBufferData(GL_ARRAY_BUFFER, 2 * 3 * 3 * stacks * slices * sizeof(float), positionData, GL_STATIC_DRAW);

	// acive the second VBO for color
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjs[1]);
	// load data from 1-D array colorData[] of color to the second VBO 
	glBufferData(GL_ARRAY_BUFFER, 2 * 3 * 3 * stacks * slices * sizeof(float), normalData, GL_STATIC_DRAW);

	// active the 3rd VBO for texture
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjs[2]);
	// load data from 1-D array TexData[] to 3rd VBO
	glBufferData(GL_ARRAY_BUFFER, 2 * 2 * 3 * stacks * slices * sizeof(float), texData, GL_STATIC_DRAW);

	// create one vertex array object -- VAO, for 3 VBOs
	glGenVertexArrays(1, &vertexArrayObj);
	// make VAO active and current
	glBindVertexArray(vertexArrayObj);
	// enable generic attribute: position
	glEnableVertexAttribArray(0);
	// enable generic attribute: normal
	glEnableVertexAttribArray(1);
	// enable generic attribute: texture
	glEnableVertexAttribArray(2);

	// make the position VBO current
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjs[0]);
	// specify the location and format of the generic vertex attribute "VertexPosition" 
	// index 0, 3 components (x, y, z), and each of which are GL_FLOATs, not normalized, stride is 0, offset 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

	// make the color VBO current
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjs[1]);
	// specify the location and format of the generic vertex attribute "VertexColor"
	// index 1, 3 components (r, g, b), and each of which are GL_FLOATs, not normalized, stride is 0, offset 0
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

	// make the texture VBO current
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjs[2]);
	// index 2, 2 components
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
}

void shaderSource (const GLuint &shaderHandle, const char* filename) {
	const GLchar *shaderArray = shaderSourceToArray(filename);
	glShaderSource(shaderHandle, 1, &shaderArray, NULL);
}

void compileShader(const GLuint &shaderHandle, const char* msg) {
	glCompileShader(shaderHandle);
	GLint result;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &result);
	if(GL_FALSE == result) {
		std::cout << msg << " compilation failed" << std::endl;
		GLint logLen;
		glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			char *log = (char*)malloc(logLen);
			GLsizei written;
			glGetShaderInfoLog(shaderHandle, logLen, &written, log);
			std::cout << "shade log: " << log << std::endl;
			free(log);
		}
	}
}

unsigned long getFileLength(ifstream& file) {
	if(!file.good()) return 0;
	file.seekg(0, ios::end);
	unsigned long len = file.tellg();
	file.seekg(0, ios::beg);
	return len;
}

GLchar* shaderSourceToArray(const char* filename) {
	ifstream infile;
	infile.open(filename, ios::in); 
	if(!infile) {
		std::cout << "error opening shader source code" << std::endl;
		exit(1);
	}
	unsigned long len = getFileLength(infile);
	if (len == 0) {
		std::cout << "empty file" << std::endl;
		exit(1);
	}
	GLchar *ShaderSource = new char[len+1];
	if (ShaderSource == NULL) {
		std::cout << "failed to allocate memory" << std::endl;
		exit(1);
	}	
	ShaderSource[len] = 0; 
	unsigned int i=0;
	while (infile.good()) {
		ShaderSource[i] = infile.get(); 
		if (!infile.eof()) i++;
	}
	ShaderSource[i] = 0;  
	infile.close();
	return ShaderSource; 
}

// mouse click
int x_click, y_click;
int button;

void mouse_click(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		x_click = x;
		y_click = y;
		::button = button; // button = GLUT_LEFT_BUTTON, GLUT_RIGHT_BUTTON, GLUT_MIDDLE_BUTTON
	}else 
		::button = -1;
}

void mouse_motion(int x, int y) {
	switch (button){
	case GLUT_LEFT_BUTTON:
		angle_x  -= (y_click - y);
		angle_y -= (x_click - x);
		break;
	case GLUT_RIGHT_BUTTON:
		scale -= (x_click - x) * 0.02;
		break;
	}
	x_click = x;
	y_click = y;
}
