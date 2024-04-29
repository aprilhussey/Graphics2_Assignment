#ifndef GLSL_H
#define GLSL_H

typedef unsigned long ulong;
struct point {
	float x, y, z;
};
struct myMesh {
	GLuint vao;
	int    numFaces;
};
void     mouse_click(int button, int state, int x, int y);
void     mouse_motion(int x, int y);
void     torus(float r1, float r2, int stacks, int slices, float vertArray[], float vertNormal[], float texData[]);
int      plyLoader(const char* filename, float *&vertArray, float *&vertNormal);
int      objLoader(const char* filename, float *&vertArray, float *&vertNormal);
bool     import3DFromFile(const std::string& filename);
void     stack(const point& c, float r, int stacks, point* p);
void     crossProduct(const point& v1, const point& v2, point& v);
void     display();
void     init();
void     idle();
void     reshape(int w, int h);
ulong    getFileLength(std::ifstream& file);
GLchar*  shaderSourceToArray(const char* filename);
void     shaderSource (const GLuint &shaderHandle, const char* filename);
void     compileShader(const GLuint &shaderHandle, const char* msg);
#endif