#include "stdafx.h"

/*
// BEGIN INITIALIZATION
// Define some vertex data 
struct Vertex {
  GLfloat position[3];
  GLfloat texcoord[2];
};
Vertex vertexdata[NUM_VERTS] = { ... };
GLubyte indexdata[NUM_INDICES] = { 0, 1, 2, ... };

// Create and bind a VAO
GLuint vao;
glGenVertexArrays(1, &vao);
glBindVertexArray(vao);

// Create and bind a BO for vertex data
GLuint vbuffer;
glGenBuffers(1, &vbuffer);
glBindBuffer(GL_ARRAY_BUFFER, vbuffer);

// copy data into the buffer object
glBufferData(GL_ARRAY_BUFFER, NUM_VERTS * sizeof(Vertex), vertexdata, GL_STATIC_DRAW);

// set up vertex attributes
glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
glEnableVertexAttribArray(1);
glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));

// Create and bind a BO for index data
GLuint ibuffer;
glGenBuffers(1, &ibuffer);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);

// copy data into the buffer object
glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_INDICES * sizeof(GLubyte), indexdata, GL_STATIC_DRAW);

// At this point the VAO is set up with two vertex attributes
// referencing the same buffer object, and another buffer object
// as source for index data. We can now unbind the VAO, go do
// something else, and bind it again later when we want to render
// with it.

glBindVertexArray(0);

// END INITIALIZATION

// BEGIN RENDER LOOP

// This is it. Binding the VAO again restores all buffer 
// bindings and attribute settings that were previously set up
glBindVertexArray(vao);
glDrawElements(GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_BYTE, (void*)0);

// END RENDER LOOP
*/

