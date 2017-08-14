#pragma once
#include <exception>
#include "../repository/tstring.h"
#include "opengl.h"

namespace ogl {
	class FileNotFound : public std::runtime_error {
	public:
		FileNotFound(tstring filename);
		virtual const char* what() const throw();
		tstring getFilename();
	private:
		tstring m_filename;
		static tstringstream cnvt;
	};

	class ResourceNotAllocate : public std::runtime_error {
	public:
		ResourceNotAllocate(GLenum type);
		virtual const char* what() const throw();
		static tstring getMsg(GLenum type) {
			switch (type) {
			case GL_TEXTURE_2D: return _T("GL_TEXTURE_2D");
			case GL_SAMPLER_2D: return _T("GL_SAMPLER_2D");
			case GL_TEXTURE_CUBE_MAP: return _T("GL_TEXTURE_CUBE_MAP");
			case GL_SAMPLER_CUBE: return _T("GL_SAMPLER_CUBE");
			case GL_PROGRAM: return _T("GL_PROGRAM");
			case GL_ARRAY_BUFFER: return _T("GL_ARRAY_BUFFER");
			case GL_ELEMENT_ARRAY_BUFFER: return _T("GL_ELEMENT_ARRAY_BUFFER");
			case GL_VERTEX_ARRAY: return _T("GL_VERTEX_ARRAY");
			case GL_FRAMEBUFFER: return _T("GL_FRAMEBUFFER");
			case GL_RENDERBUFFER: return _T("GL_RENDERBUFFER");
			case GL_DEPTH_COMPONENT: return _T("GL_DEPTH_COMPONENT");
			case GL_STENCIL_COMPONENTS: return _T("GL_STENCIL_COMPONENTS");
			case GL_DEPTH_STENCIL: return _T("GL_DEPTH_STENCIL");
			case GL_COLOR_ATTACHMENT0: return _T("GL_COLOR_ATTACHMENT");
			default: return to_tstring(type);
			}
		}
		GLenum getType();
	private:
		GLenum m_type;
		static tstringstream cnvt;
	};
}