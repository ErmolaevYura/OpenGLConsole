#pragma once
#include "../repository/core.h"

//OpenGL Extension Library Wrangler
#define GLEW_STATIC
#include <GL/glew.h>

namespace ogl {
	static tstring glewSupported() {
		if (glewIsSupported("GL_VERSION_4_5")) return _T("GLEW supported OpenGL 4.5");
		else if (glewIsSupported("GL_VERSION_4_4")) return _T("GLEW supported OpenGL 4.4");
		else if (glewIsSupported("GL_VERSION_4_3")) return _T("GLEW supported OpenGL 4.3");
		else if (glewIsSupported("GL_VERSION_4_2")) return _T("GLEW supported OpenGL 4.2");
		else if (glewIsSupported("GL_VERSION_4_1")) return _T("GLEW supported OpenGL 4.1");
		else if (glewIsSupported("GL_VERSION_4_0")) return _T("GLEW supported OpenGL 4.0");
		else if (glewIsSupported("GL_VERSION_3_3")) return _T("GLEW supported OpenGL 3.3");
		else if (glewIsSupported("GL_VERSION_3_2")) return _T("GLEW supported OpenGL 3.2");
		else if (glewIsSupported("GL_VERSION_3_1")) return _T("GLEW supported OpenGL 3.1");
		else if (glewIsSupported("GL_VERSION_3_0")) return _T("GLEW supported OpenGL 3.0");
		else if (glewIsSupported("GL_VERSION_2_1")) return _T("GLEW supported OpenGL 2.1");
		else if (glewIsSupported("GL_VERSION_2_0")) return _T("GLEW supported OpenGL 2.0");
		else if (glewIsSupported("GL_VERSION_1_5")) return _T("GLEW supported OpenGL 1.5");
		else if (glewIsSupported("GL_VERSION_1_4")) return _T("GLEW supported OpenGL 1.4");
		else if (glewIsSupported("GL_VERSION_1_3")) return _T("GLEW supported OpenGL 1.3");
		else if (glewIsSupported("GL_VERSION_1_2_1")) return _T("GLEW supported OpenGL 1.2.1");
		else if (glewIsSupported("GL_VERSION_1_2")) return _T("GLEW supported OpenGL 1.2");
		else if (glewIsSupported("GL_VERSION_1_1")) return _T("GLEW supported OpenGL 1.1");
		return _T("");
	}
	static int initGlew() {
		core::AddInfo(_T("GLEW Version {0}"), GetString((char*)glewGetString(GLEW_VERSION)));
		glewExperimental = GL_TRUE;
		int status = glewInit();
		if (status != GLEW_OK) {
			tstring err = _T("Failed to initialize GLEW. Errors: ");
			err.append(GetString((char*)glewGetErrorString(status)));
			core::AddError(err);
		}
		core::AddInfo(glewSupported());
		return status;
	}
}