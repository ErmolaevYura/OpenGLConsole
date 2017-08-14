#pragma once

#include "Camera.hpp"

namespace ogl {
	class Context {
		Context(Context const&) = delete;
		Context& operator= (Context const&) = delete;
		bool m_ready = false;
		FlyCamera *flyCamera;
	public:
		Context() { flyCamera = new FlyCamera(); }
		~Context() { delete flyCamera; }
		static Context& getInstance() {
			static Context instance;
			return instance;
		}
		bool Ready() { return m_ready; }
		FlyCamera* getFlyCamera() { return flyCamera; }
	};
}
/*
using namespace ogl;

void Context::printError(int error, const char* description) {
	tstring str = _T("Error: ");
	str.append(GetString(description));
	core::AddError(str);
}
void Context::resizeCallback(GLFWwindow* window, int width, int height) {
	app.resize(width, height);
}
void Context::focusCallback(GLFWwindow* window, int focus) {
	//app.Focus(focus == GLFW_TRUE ? true : false);
}
void Context::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	else if (key == GLFW_KEY_F4 && action == GLFW_PRESS)
		app.switchFullscreen(!app.Fullscreen());
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS)
			app.keys[key] = true;
		else if (action == GLFW_RELEASE)
			app.keys[key] = false;
	}
}
void Context::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	double xoffset, yoffset;
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glfwGetCursorPos(window, &xoffset, &yoffset);
	glfwSetCursorPos(window, width / 2, height / 2);
	app.mouseMovement(xpos, ypos);
}
void Context::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	app.mouseScroll(yoffset);
}
void Context::clickCallback(int b, int s, int mouse_x, int mouse_y) {
	//пересечение луча с объектами от места клика в глубину сцены
	//нормирование координат положения курсора
	float x = (2.0f * mouse_x) / appMain.m_width - 1.0f;
	float y = 1.0f - (2.0f * mouse_y) / appMain.m_height;
	float z = 1.0f;
	vec3 ray_nds = vec3(x, y, z);
	//построение вектора луча в глубину
	vec4 ray_clip = vec4(ray_nds.xy, -1.0, 1.0);
	//координаты камеры??
	vec4 ray_eye = inverse(projection_matrix) * ray_clip;
	ray_eye = vec4(ray_eye.xy, -1.0, 0.0);
	//Мировые координаты
	vec3 = ray_world = (inverse(view_matrix) * ray_eye).xyz;
	ray_world = normalize(ray_world);
}

tstring Context::glewSupported() {
	if (glewIsSupported("GL_VERSION_4_5")) return _T("Glew supported OpenGL 4.5");
	else if (glewIsSupported("GL_VERSION_4_4")) return _T("Glew supported OpenGL 4.4");
	else if (glewIsSupported("GL_VERSION_4_3")) return _T("Glew supported OpenGL 4.3");
	else if (glewIsSupported("GL_VERSION_4_2")) return _T("Glew supported OpenGL 4.2");
	else if (glewIsSupported("GL_VERSION_4_1")) return _T("Glew supported OpenGL 4.1");
	else if (glewIsSupported("GL_VERSION_4_0")) return _T("Glew supported OpenGL 4.0");
	else if (glewIsSupported("GL_VERSION_3_3")) return _T("Glew supported OpenGL 3.3");
	else if (glewIsSupported("GL_VERSION_3_2")) return _T("Glew supported OpenGL 3.2");
	else if (glewIsSupported("GL_VERSION_3_1")) return _T("Glew supported OpenGL 3.1");
	else if (glewIsSupported("GL_VERSION_3_0")) return _T("Glew supported OpenGL 3.0");
	else if (glewIsSupported("GL_VERSION_2_1")) return _T("Glew supported OpenGL 2.1");
	else if (glewIsSupported("GL_VERSION_2_0")) return _T("Glew supported OpenGL 2.0");
	else if (glewIsSupported("GL_VERSION_1_5")) return _T("Glew supported OpenGL 1.5");
	else if (glewIsSupported("GL_VERSION_1_4")) return _T("Glew supported OpenGL 1.4");
	else if (glewIsSupported("GL_VERSION_1_3")) return _T("Glew supported OpenGL 1.3");
	else if (glewIsSupported("GL_VERSION_1_2_1")) return _T("Glew supported OpenGL 1.2.1");
	else if (glewIsSupported("GL_VERSION_1_2")) return _T("Glew supported OpenGL 1.2");
	else if (glewIsSupported("GL_VERSION_1_1")) return _T("Glew supported OpenGL 1.1");
	return "";
}

Context::Context() { flyCamera = new FlyCamera(); }
Context::~Context() { delete flyCamera; }
void Context::Deinit(GLFWwindow *window) {
	glfwDestroyWindow(window);
	glfwTerminate();
}

GLFWwindow* Context::Init(tstring title, int width, int height, bool fullscreen,
	int majorVersion, int minorVersion, int samples) {
	GLFWwindow *window =
		initGlfw(title, width, height, fullscreen, majorVersion, minorVersion, samples);
	if (!window) return window;
	int result = initGlew();
	if (result) { Deinit(window); return nullptr; }
	result = initOpenGL();
	if (result) { Deinit(window); return nullptr; }
	result = initFreetype();
	if (result) { Deinit(window); return nullptr; }
	result = loadShaders();
	if (result) { Deinit(window); return nullptr; }
	else m_ready = true;
	return window;
}
GLFWwindow* Context::initGlfw(tstring title, int width, int height, bool fullscreen,
	int majorVersion, int minorVersion, int samples) {
	GLFWwindow *window;
	glfwSetErrorCallback(Context::printError);
	core::AddInfo(GetString(glfwGetVersionString()));
	if (!glfwInit()) { core::AddError(_T("Failed to initialize GLFW")); return nullptr; }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#ifdef _DEBUG 
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_SAMPLES, samples);

	if (fullscreen) {
		GLFWmonitor *mon = glfwGetPrimaryMonitor();
		const GLFWvidmode *vmode = glfwGetVideoMode(mon);
		window = glfwCreateWindow(vmode->width, vmode->height, GetC(title.data()), mon, nullptr);
	}
	else window = glfwCreateWindow(width, height, GetC(title.data()), nullptr, nullptr);
	if (!window) {
		core::AddError(_T("Failed to open GLFW window."));
		glfwTerminate();
		return window;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, Context::keyCallback);
	glfwSetCursorPosCallback(window, Context::mouseCallback);
	glfwSetScrollCallback(window, Context::scrollCallback);
	glfwSetWindowSizeCallback(window, Context::resizeCallback);
	glfwSetWindowFocusCallback(window, Context::focusCallback);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSwapInterval(1);

	return window;
}
int Context::initGlew() {
	glewExperimental = GL_TRUE;
	int status = glewInit();
	if (status != GLEW_OK) {
		tstring err = _T("Failed to initialize GLEW. Errors: ");
		err.append(GetString((char*)glewGetErrorString(status)));
		core::AddError(err);
		glfwTerminate();
	}
	ConsoleLogger::AddInfo(glewSupported());
	if (glGetString(GL_VENDOR))	core::AddInfo(_T("OpenGL Vendor: ") + tstring((char*)glGetString(GL_VENDOR)));
	if (glGetString(GL_RENDER)) core::AddInfo(_T("OpenGL Render: ") + tstring((CHAR*)glGetString(GL_RENDER)));
	if (glGetString(GL_VERSION)) core::AddInfo(_T("OpenGL Version: ") + tstring((CHAR*)glGetString(GL_VERSION)));
	if (glGetString(GL_SHADING_LANGUAGE_VERSION)) core::AddInfo(_T("GLSL: ") + tstring((CHAR*)glGetString(GL_SHADING_LANGUAGE_VERSION)));
	return status;
}
int Context::initOpenGL() {
	// Настиройка OpenGL Контекста
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);	// Цвет очистки фона

	glEnable(GL_DEPTH_TEST);				// Тест глубины(Z-Buffer)
	glDepthFunc(GL_LESS);

	// Отсечение полигонов по направлению нормали
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);		// Выбор направления: (GL_FRONT, GL_BACK, GL_FRONT_AND_BACK)

	// Несколько фрагментов для определения цвета (Смешивание)
	glEnable(GL_MULTISAMPLE);

	// Настройка трафарета
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_GEQUAL, 2, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xFF);

	// Прозрачность
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return 0;
}
int Context::initFreetype() {
	//auto status = FT_Init_FreeType(&ft);
	//if (status)
	//	core::AddError(_T("Could not init freetype library. Error code: ") + to_tstring(status));
	return 0;//status;
}
int Context::loadShaders() {
	return 0;
}
*/