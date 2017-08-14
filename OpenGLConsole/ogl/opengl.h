#pragma once

#include "glew.h"
#include "glfw.h"
#include "glm.h"
#include "../repository/tstring.h"

namespace ogl {
	/* Класс для описания 3d объекта */
	class Vertex {
	public:
		glm::vec3 m_position;
		glm::vec3 m_normal;
		glm::vec2 m_texCoords;
	};
	/* Вывод инфрмации о OpenGL */
	static void PrintOpenGLString(tstring msg, GLenum target) {
		if (glGetString(target))
			core::AddInfo(msg, GetString(reinterpret_cast<const char *>(glGetString(target))));
	}
	static void PrintOpenGlInteger(tstring msg, GLenum target) {
		GLint val;
		glGetIntegerv(target, &val);
		core::AddInfo(msg, val);
	}
	static void PrintOpenGLExtensionList() {
		tstringstream ss;
		ss << _T("OpenGL Extensions:") << std::endl;
		GLint n;
		glGetIntegerv(GL_NUM_EXTENSIONS, &n);
		for(int i = 0; i < n-1; i++) 
			ss << _T("\t") << GetString((char*)glGetStringi(GL_EXTENSIONS, i)) << std::endl;
		ss << _T("\t") << GetString((char*)glGetStringi(GL_EXTENSIONS, n - 1));
		ConsoleLogger::AddInfo(ss.str());
	}
	static void PrintFramebufferLimits() {
		tstringstream ss;
		int val;
		ss << _T("Framebuffer limits:") << std::endl;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &val);
		ss << _T("\tMax Color Attachments: ") << val << std::endl;
		glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &val);
		ss << _T("\tMax Framebuffer Width: ") << val << std::endl;
		glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &val);
		ss << _T("\tMax Framebuffer Height: ") << val << std::endl;
		glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &val);
		ss << _T("\tMax Framebuffer Samples: ") << val << std::endl;
		glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &val);
		ss << _T("\tMax Framebuffer Layers: ") << val;
		ConsoleLogger::AddInfo(ss.str());
	}
	static void PrintShaderLimits() {
		tstringstream ss;
		int val;
		ss << _T("Shader limits:") << std::endl;
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &val);
		ss << _T("\tMax vertex uniform blocks: ") << val << std::endl;
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &val);
		ss << _T("\tMax fragment uniform blocks: ") << val << std::endl;
		glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &val);
		ss << _T("\tMax geometry uniform blocks: ") << val << std::endl;
		glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &val);
		ss << _T("\tMax combined uniform blocks: ") << val << std::endl;
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &val);
		ss << _T("\tMax uniform buffer bindings: ") << val << std::endl;
		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &val);
		ss << _T("\tMax uniform block size: ") << val << std::endl;
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &val);
		ss << _T("\tMax vertex uniform components: ") << val << std::endl;
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &val);
		ss << _T("\tMax fragment uniform components: ") << val << std::endl;
		glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, &val);
		ss << _T("\tMax geometry uniform components: ") << val << std::endl;
		glGetIntegerv(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS, &val);
		ss << _T("\tMax combined vertex uniform components: ") << val << std::endl;
		glGetIntegerv(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, &val);
		ss << _T("\tMax combined fragment uniform components: ") << val << std::endl;
		glGetIntegerv(GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS, &val);
		ss << _T("\tMax combined geometry uniform components: ") << val << std::endl;
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &val);
		ss << _T("\tUniform buffer offset aligment: ") << val << std::endl;
		ConsoleLogger::AddInfo(ss.str());
	}
	static void PrintOpenGLInfo() {
		core::AddInfo(_T("GLU Version {0}"), GetString((char*)gluGetString(GLU_VERSION)));
		core::AddInfo(_T("GLU Extension: {0}"), GetString((char*)gluGetString(GLU_EXTENSIONS)));
		PrintOpenGLString(_T("OpenGL Vendor: {0}"), GL_VENDOR);
		PrintOpenGLString(_T("OpenGL Rendere: {0}"), GL_RENDERER);
		PrintOpenGLString(_T("OpenGL Version {0}"), GL_VERSION);
		PrintOpenGLString(_T("GLSL Version {0}"), GL_SHADING_LANGUAGE_VERSION);
		PrintOpenGLExtensionList();
		PrintFramebufferLimits();
		PrintShaderLimits();
	}
	/* Инициализация OpenGL */
	static int initOpenGL() {
		// Настройка OpenGL Контекста
		glClearColor(0.4f, 0.0f, 0.4f, 0.0f);	// Цвет очистки фона

		glEnable(GL_DEPTH_TEST);				// Тест глубины(Z-Buffer)
		glDepthFunc(GL_LESS);

		// Отсечение полигонов по направлению нормали
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);		// Выбор направления: (GL_FRONT, GL_BACK, GL_FRONT_AND_BACK)

		// Несколько фрагментов для определения цвета (Смешивание)
		glEnable(GL_MULTISAMPLE);

		// Настройка трафарета
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_GEQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF);

		// Прозрачность
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		return 0;
	}
	/* Создаем окно GLFW */
	static GLFWwindow* Init(tstring title = _T("Test OpenGl"),
		int width = 1024, int height = 768, bool fullscreen = false,
		int majorVersion = 4, int minorVersion = 5, int samples = 4) {
		GLFWwindow *window = GlfwInit(title, width, height, fullscreen, majorVersion, minorVersion, samples);
		if (!window) return window;
		if (initGlew()) { GlfwRelease(window); return nullptr; }
		if (initOpenGL()) { GlfwRelease(window); return nullptr; }
		PrintOpenGLInfo();
		return window;
	}
	/* Разрушаем окно GLFW */
	static void Deinit(GLFWwindow *window) {
		GlfwRelease(window);
	}
	
	static tstring convertInternalFormatToString(GLenum format) {
		// convert OpenGL internal format enum to string
		tstring formatName;
		switch (format) {
		case GL_STENCIL_INDEX:      // 0x1901
			formatName = _T("GL_STENCIL_INDEX");
			break;
		case GL_DEPTH_COMPONENT:    // 0x1902
			formatName = _T("GL_DEPTH_COMPONENT");
			break;
		case GL_ALPHA:              // 0x1906
			formatName = _T("GL_ALPHA");
			break;
		case GL_RGB:                // 0x1907
			formatName = _T("GL_RGB");
			break;
		case GL_RGBA:               // 0x1908
			formatName = _T("GL_RGBA");
			break;
		case GL_LUMINANCE:          // 0x1909
			formatName = _T("GL_LUMINANCE");
			break;
		case GL_LUMINANCE_ALPHA:    // 0x190A
			formatName = _T("GL_LUMINANCE_ALPHA");
			break;
		case GL_R3_G3_B2:           // 0x2A10
			formatName = _T("GL_R3_G3_B2");
			break;
		case GL_ALPHA4:             // 0x803B
			formatName = _T("GL_ALPHA4");
			break;
		case GL_ALPHA8:             // 0x803C
			formatName = _T("GL_ALPHA8");
			break;
		case GL_ALPHA12:            // 0x803D
			formatName = _T("GL_ALPHA12");
			break;
		case GL_ALPHA16:            // 0x803E
			formatName = _T("GL_ALPHA16");
			break;
		case GL_LUMINANCE4:         // 0x803F
			formatName = _T("GL_LUMINANCE4");
			break;
		case GL_LUMINANCE8:         // 0x8040
			formatName = _T("GL_LUMINANCE8");
			break;
		case GL_LUMINANCE12:        // 0x8041
			formatName = _T("GL_LUMINANCE12");
			break;
		case GL_LUMINANCE16:        // 0x8042
			formatName = _T("GL_LUMINANCE16");
			break;
		case GL_LUMINANCE4_ALPHA4:  // 0x8043
			formatName = _T("GL_LUMINANCE4_ALPHA4");
			break;
		case GL_LUMINANCE6_ALPHA2:  // 0x8044
			formatName = _T("GL_LUMINANCE6_ALPHA2");
			break;
		case GL_LUMINANCE8_ALPHA8:  // 0x8045
			formatName = _T("GL_LUMINANCE8_ALPHA8");
			break;
		case GL_LUMINANCE12_ALPHA4: // 0x8046
			formatName = _T("GL_LUMINANCE12_ALPHA4");
			break;
		case GL_LUMINANCE12_ALPHA12:// 0x8047
			formatName = _T("GL_LUMINANCE12_ALPHA12");
			break;
		case GL_LUMINANCE16_ALPHA16:// 0x8048
			formatName = _T("GL_LUMINANCE16_ALPHA16");
			break;
		case GL_INTENSITY:          // 0x8049
			formatName = _T("GL_INTENSITY");
			break;
		case GL_INTENSITY4:         // 0x804A
			formatName = _T("GL_INTENSITY4");
			break;
		case GL_INTENSITY8:         // 0x804B
			formatName = _T("GL_INTENSITY8");
			break;
		case GL_INTENSITY12:        // 0x804C
			formatName = _T("GL_INTENSITY12");
			break;
		case GL_INTENSITY16:        // 0x804D
			formatName = _T("GL_INTENSITY16");
			break;
		case GL_RGB4:               // 0x804F
			formatName = _T("GL_RGB4");
			break;
		case GL_RGB5:               // 0x8050
			formatName = _T("GL_RGB5");
			break;
		case GL_RGB8:               // 0x8051
			formatName = _T("GL_RGB8");
			break;
		case GL_RGB10:              // 0x8052
			formatName = _T("GL_RGB10");
			break;
		case GL_RGB12:              // 0x8053
			formatName = _T("GL_RGB12");
			break;
		case GL_RGB16:              // 0x8054
			formatName = _T("GL_RGB16");
			break;
		case GL_RGBA2:              // 0x8055
			formatName = _T("GL_RGBA2");
			break;
		case GL_RGBA4:              // 0x8056
			formatName = _T("GL_RGBA4");
			break;
		case GL_RGB5_A1:            // 0x8057
			formatName = _T("GL_RGB5_A1");
			break;
		case GL_RGBA8:              // 0x8058
			formatName = _T("GL_RGBA8");
			break;
		case GL_RGB10_A2:           // 0x8059
			formatName = _T("GL_RGB10_A2");
			break;
		case GL_RGBA12:             // 0x805A
			formatName = _T("GL_RGBA12");
			break;
		case GL_RGBA16:             // 0x805B
			formatName = _T("GL_RGBA16");
			break;
		case GL_DEPTH_COMPONENT16:  // 0x81A5
			formatName = _T("GL_DEPTH_COMPONENT16");
			break;
		case GL_DEPTH_COMPONENT24:  // 0x81A6
			formatName = _T("GL_DEPTH_COMPONENT24");
			break;
		case GL_DEPTH_COMPONENT32:  // 0x81A7
			formatName = _T("GL_DEPTH_COMPONENT32");
			break;
		case GL_DEPTH_STENCIL:      // 0x84F9
			formatName = _T("GL_DEPTH_STENCIL");
			break;
		case GL_RGBA32F:            // 0x8814
			formatName = _T("GL_RGBA32F");
			break;
		case GL_RGB32F:             // 0x8815
			formatName = _T("GL_RGB32F");
			break;
		case GL_RGBA16F:            // 0x881A
			formatName = _T("GL_RGBA16F");
			break;
		case GL_RGB16F:             // 0x881B
			formatName = _T("GL_RGB16F");
			break;
		case GL_DEPTH24_STENCIL8:   // 0x88F0
			formatName = _T("GL_DEPTH24_STENCIL8");
			break;
		default:
			tstringstream ss;
			ss << _T("Unknown Format(0x") << std::hex << format << _T(")") << std::ends;
			formatName = ss.str();
		}
		return formatName;
	}
	static tstring getTextureParametres(GLuint id) {
		// return texture parameters as string using glGetTexLevelParameteriv()
		if (glIsTexture(id) == GL_FALSE)
			return _T("Not texture object");

		int width, height, format;
		tstring formatName;

		glBindTexture(GL_TEXTURE_2D, id);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
		glBindTexture(GL_TEXTURE_2D, 0);

		formatName = ogl::convertInternalFormatToString(format);
		tstringstream ss;
		ss << width << _T("x") << height << _T(", ") << formatName;
		return ss.str();
	}
	static tstring getRenderbufferParametres(GLuint rbo) {
		// return renderbuffer parameters as string using glGetRenderbufferParameteriv
		if (glIsRenderbuffer(rbo) == GL_FALSE)
			return to_tstring(rbo) + _T( "not Renderbuffer object");

		int width, height, format, samples;
		tstring formatName;

		GLint currbo;
			glGetIntegerv(GL_RENDERBUFFER_BINDING, &currbo);
		if (currbo != rbo) glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, &format);
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_SAMPLES, &samples);
		if (currbo != rbo) glBindRenderbuffer(GL_RENDERBUFFER, currbo);

		formatName = convertInternalFormatToString(format);
		tstringstream ss;
		ss << width << _T("x") << height << _T(", ") << formatName << _T(", MSAA(") << samples << _T(")");
		return ss.str();
	}
	static tstring getFramebufferInfo(GLuint fbo, GLenum target = GL_FRAMEBUFFER) {
		// print out the FBO infos
		if (glIsFramebuffer(fbo) == GL_FALSE)
			return to_tstring(fbo) + _T(" not Framebuffer object");
		tstringstream ss;
		GLint curfbo;
		switch (target) {
		case GL_FRAMEBUFFER:
		case GL_DRAW_FRAMEBUFFER:
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &curfbo);
			break;
		case GL_READ_FRAMEBUFFER:
			glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &curfbo);
			break;
		default: return to_tstring(fbo) + _T(" - Unsupported target");
		}
		if(curfbo != fbo) glBindFramebuffer(target, fbo);
		ss << _T("Framebuffer ") << fbo;
		int colorBufferCount = 0;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &colorBufferCount);
		ss << std::endl << _T("\tMax Number of Color Buffer Attachment Points: ") << colorBufferCount << std::endl;

		int multisampleCount = 0;
		glGetIntegerv(GL_MAX_SAMPLES, &multisampleCount);
		ss << _T("\tMax Number of Samples for MSAA: ") << multisampleCount;

		int objectType, objectId;
		for (auto i = 0; i < colorBufferCount; i++) {
			glGetFramebufferAttachmentParameteriv(target, GL_COLOR_ATTACHMENT0+i,
				GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objectType);
			if (objectType != GL_NONE) {
				glGetFramebufferAttachmentParameteriv(target, GL_COLOR_ATTACHMENT0 + i,
					GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &objectId);
				ss << std::endl << _T("\tColor Attachment ") << i << _T(": ");
				switch (objectType)
				{
				case GL_TEXTURE: ss << _T("GLTEXTURE ") << getTextureParametres(objectId); break;
				case GL_RENDERBUFFER: ss << _T("GL_RENDERBUFFER ") << getRenderbufferParametres(objectId); break;
				}
			}
		}

		glGetFramebufferAttachmentParameteriv(target, GL_DEPTH_ATTACHMENT,
			GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objectType);
		if (objectType != GL_NONE) {
			glGetFramebufferAttachmentParameteriv(target, GL_DEPTH_ATTACHMENT,
				GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &objectId);
			ss << std::endl << _T("\tDepth Attachment: ");
			switch (objectType) {
			case GL_TEXTURE: ss << _T("GLTEXTURE ") << getTextureParametres(objectId); break;
			case GL_RENDERBUFFER: ss << _T("GL_RENDERBUFFER ") << getRenderbufferParametres(objectId); break;
			}
		}

		glGetFramebufferAttachmentParameteriv(target, GL_STENCIL_ATTACHMENT,
			GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objectType);
		if (objectType != GL_NONE) {
			glGetFramebufferAttachmentParameteriv(target, GL_STENCIL_ATTACHMENT,
				GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &objectId);
			ss << std::endl << _T("\tStencil Attachment: ");
			switch (objectType) {
			case GL_TEXTURE: ss << _T("GL_TEXTURE ") << getTextureParametres(objectId); break;
			case GL_RENDERBUFFER: ss << _T("GL_RENDERBUFFER ") << getRenderbufferParametres(objectId); break;
			}
		}
		if(curfbo!= fbo) glBindFramebuffer(target, curfbo);
		return ss.str();
	}
	static int checkFramebufferStatus(GLuint fbo, GLenum target = GL_FRAMEBUFFER) {
		// check FBO completeness
		if (glIsFramebuffer(fbo) == GL_FALSE) {
			core::AddError(_T("{0}, not FrameBuffer object"), fbo); 
			return GL_FRAMEBUFFER_COMPLETE;
		}
		GLint curfbo;
		switch (target) {
		case GL_FRAMEBUFFER:
		case GL_DRAW_FRAMEBUFFER:
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &curfbo);
			break;
		case GL_READ_FRAMEBUFFER:
			glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &curfbo);
			break;
		default: return GL_FRAMEBUFFER_COMPLETE;
		}
		if (curfbo != fbo) glBindFramebuffer(target, fbo);
		GLenum status = glCheckFramebufferStatus(target);
		switch (status) {
		case GL_FRAMEBUFFER_COMPLETE: break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: core::AddError(_T("Framebuffer incomplete: Attachment is NOT complete."));break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: core::AddError(_T("Framebuffer incomplete: No image is attached to FBO.")); break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: core::AddError(_T("FrameBuffer incomplete: Draw buffer.")); break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: core::AddError(_T("FrameBuffer incomplete: Read buffer.")); break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: core::AddError(_T("FrameBuffer incomplete: Multisample.")); break;
		case GL_FRAMEBUFFER_UNSUPPORTED: core::AddError(_T("FrameBuffer incomplete: Unsupported by FBO implementation.")); break;
		default: core::AddError(_T("FrameBuffer incomplete: Unknow error.")); break;
		}
		if (curfbo != fbo) glBindFramebuffer(target, curfbo);
		return status;
	}
	
	static std::string GetShaderInfoLog(GLuint shaderId) {
		GLsizei infoLogLength = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::string infoLog(size_t(infoLogLength), _T('\0'));
		glGetShaderInfoLog(shaderId, infoLogLength, &infoLogLength, &infoLog[0]);
		if (size_t(infoLogLength) < infoLog.length())
			infoLog.erase(infoLog.begin() + ptrdiff_t(infoLogLength), infoLog.end());
		return infoLog;
	}
	static std::string GetProgramInfoLog(GLuint programId) {
		GLsizei infoLogLength = 0;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::string infoLog(size_t(infoLogLength), _T('\0'));
		glGetProgramInfoLog(programId, infoLogLength, &infoLogLength, &infoLog[0]);
		if (size_t(infoLogLength) < infoLog.length())
			infoLog.erase(infoLog.begin() + ptrdiff_t(infoLogLength), infoLog.end());
		return infoLog;
	}
};
