#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\transform2.hpp>
#include <vector>

typedef std::vector<glm::vec3> vector3;
typedef std::vector<glm::vec2> vector2;

class torus {
private:
	unsigned m_stacks;
	unsigned m_slices;
	float    m_innerR;
	float    m_outerR;

	vector3  m_vertex;
	vector3  m_normal;
	vector2  m_texture;
	unsigned* m_index;    /* index for vertices */

	vector3  slice0();
public:
	torus(unsigned stacks, unsigned slices, float innerR, float outerR)
		: m_stacks(stacks), m_slices(slices), m_innerR(innerR), m_outerR(outerR)
	{
		m_index = (unsigned*) NULL;
		std::cout << "done" << std::endl;
	}
	~torus() 
	{
		if(m_index != NULL)  delete[] m_index;
	}
	void create();
	glm::vec3* vertexData() {
		if (m_vertex.empty()) return (glm::vec3*) NULL;
		return &m_vertex[0];
	}
	glm::vec3* normalData() {
		if (m_normal.empty()) return (glm::vec3*) NULL;
		return &m_normal[0];
	}
	glm::vec2* textureData() {
		if (m_texture.empty()) return (glm::vec2*) NULL;
		return &m_texture[0];
	}
	unsigned size() {
		return m_vertex.size();
	}
};

#endif