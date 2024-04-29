//glm library is needed
//
#include "stdafx.h"
#include "geometry.h"

const float PI = 3.1415;
vector3 torus::slice0() {
	// slice 0 is the cross-section on the x-y plane
	glm::vec3  centre((m_innerR + m_outerR) / 2.0, 0.0, 0.0); 
	const float radius= (m_outerR - m_innerR) / 2.0;
	const float theta = 2.0 * PI / m_stacks;
	vector3 vertex;
	for (unsigned i = 0; i < m_stacks; i++) {
		vertex.push_back(glm::vec3(radius * cos(theta * i) + centre.x, radius * sin(theta * i) + centre.y, centre.z));
	}
	return vertex;
}

void torus::create() {
	vector3 vertex  = slice0(); 
	vector3 vertex0 = vertex;
	vector3 vertex1;
	const float theta = 2.0 * PI / m_slices;
	for (unsigned slice = 1; slice <= m_slices; slice++) {
		if (slice == m_slices) vertex1 = vertex;
		else {
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), theta * slice, glm::vec3(0.0, 1.0, 0.0));
			for (unsigned v = 0; v < m_stacks; v++) {
				vertex1.push_back(glm::vec3(rotation * glm::vec4(vertex[v], 1.0)));
			}
		}
		for (unsigned v = 0; v < m_stacks; v++) {
			unsigned w = v + 1;
			if (w == m_stacks) w = 0;
			// 001
			m_vertex.push_back(vertex0[v]);
			m_vertex.push_back(vertex1[v]);
			m_vertex.push_back(vertex1[w]);
			// 110
			m_vertex.push_back(vertex1[w]);
			m_vertex.push_back(vertex0[w]);
			m_vertex.push_back(vertex0[v]);
		}
		vertex0 = vertex1;
		vertex1.clear();
	}
	unsigned numOfTriangles = 2 * m_stacks * m_slices;
	//unsigned size = 3 * numOfTriangles; 
	unsigned size = m_vertex.size();
	float s = 1.0 / (2.0 * m_outerR); // factor to scale the torus so that it could be fit into a unit square (the texture), 
			                          // the scaled coordinates of each vertex are the texture coordinate
	for (unsigned i = 0; i < size; i++) {
		m_texture.push_back(glm::vec2(s * m_vertex[i]));
	}
	// vectors along 2 edges of a triangle
	glm::vec3 v1, v2;
	for (unsigned i = 0; i < numOfTriangles; i++) {
		unsigned j = i * 3;
		v1 = m_vertex[j + 1] - m_vertex[j];
		v2 = m_vertex[j + 2] - m_vertex[j + 1];
		glm::vec3 normal = glm::cross(v1, v2);
		m_normal.push_back(normal);
		m_normal.push_back(normal);
		m_normal.push_back(normal);
	}
	/*
	m_index = new unsigned[size];
	unsigned j = 0;
	for (unsigned slice = 0; slice < m_slices - 1; slice++) {
		unsigned stride = 6; // 2 triangles, so takes 6 indices in the index array
		for (unsigned first = slice * stride; first < slice * stride + m_stacks - 1; first++) {
			m_index[j] = first; j++;
			m_index[j] = first + m_stacks; j++;
			m_index[j] = first + m_stacks + 1; j++;
			m_index[j] = first + m_stacks + 1; j++;
			m_index[j] = first + 1; j++;
			m_index[j] = first; j++;
		}
	}
	*/
}