#pragma once
#include <vector>
#include <exception>
#include "opengl.h"

namespace ogl {
	class ArrayBuffer {
		GLuint m_vertexArrayId = 0;
		std::vector<GLuint> attributes;
		bool bDataUploaded = false;
	public:
		ArrayBuffer* create() {
			glGenVertexArrays(1, &m_vertexArrayId);
			if(m_vertexArrayId == 0) 
				throw ogl::ResourceNotAllocate(GL_VERTEX_ARRAY);
			return this;
		}
		void release() {
			if(m_vertexArrayId != 0) glDeleteVertexArrays(1, &m_vertexArrayId);
			bDataUploaded = false;
		}
		ArrayBuffer* bind() {
			if (m_vertexArrayId == 0) 
				throw ogl::ResourceNotAllocate(GL_VERTEX_ARRAY);
			glBindVertexArray(m_vertexArrayId);
			for each (auto attr in attributes) glEnableVertexAttribArray(attr);
			bDataUploaded = true;
			return this;
		}
		void unbind() {
			for each (auto attr in attributes) glDisableVertexAttribArray(attr);
			glBindVertexArray(0); 
		}
		ArrayBuffer* addAttrib(GLuint index, GLint size, GLenum type, GLboolean normalized, GLuint stride, unsigned int pointer) {
			attributes.push_back(index);
			glVertexAttribPointer(index, size, type, normalized, stride, (GLvoid*)pointer);
			return this;
		}
		GLuint get() { return m_vertexArrayId; }
	};
};
