#pragma once
#include "../repository/core.h"

#define GLFW_DLL
#define GLFW_INCLUDE_GLU
//#define GLFW_INCLUDE_GLCOREARB
//#define GLFW_INCLUDE_ES1
//#define GLFW_INCLUDE_ES2
//#define GLFW_INCLUDE_ES3
//#define GLFW_INCLUDE_ES31
//#define GLFW_INCLUDE_VULKAN
//#define GLFW_INCLUDE_NONE
//#define GLFW_INCLUDE_GLEXT
#include <GL/glfw3.h>	//GLFW обрабатывать окно и клавиатуру
#include "../app.h"

namespace ogl 
{
	static void printError(int error, const char* description) {
		tstring str = _T("Error: ");
		str.append(GetString(description));
		core::AddError(str);
	}
	
	void monitorCallback(GLFWmonitor* monitor, int event);
	
	void resizeCallback(GLFWwindow* window, int width, int height);
	void focusCallback(GLFWwindow* window, int focus);
	void windowPosCallback(GLFWwindow* window, int xpos, int ypos);
	void windowIconifyCallback(GLFWwindow* window, int iconified);
	void windowRefreshCallback(GLFWwindow* window);
	void closeCallback(GLFWwindow* window);
	
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	void charCallback(GLFWwindow* window, unsigned int ch);
	void charModsCallback(GLFWwindow* window, unsigned int codepoint, int mods);

	void mouseMove(GLFWwindow* window, double xpos, double ypos);
	void cursorEnter(GLFWwindow * window, int intered);
	void mouseKey(GLFWwindow * window, int button, int action, int mode);
	void mouseScroll(GLFWwindow* window, double xoffset, double yoffset);
	void mouseClick(int b, int s, int mouse_x, int mouse_y);
	
	void joystickCallback(int joy, int event);

	void dropCallback(GLFWwindow * win, int count, const char ** paths);

	void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	
	static GLFWwindow* GlfwInit(tstring title, int width, int height, bool fullscreen,
		int majorVersion, int minorVersion, int samples) {
		core::AddInfo(_T("GLFW Version {0}"), GetString(glfwGetVersionString()));
		GLFWwindow *window;
		glfwSetErrorCallback(printError);
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

		glfwSetMonitorCallback(monitorCallback);
		
		glfwSetWindowSizeCallback(window, resizeCallback);
		glfwSetWindowFocusCallback(window, focusCallback);
		glfwSetWindowPosCallback(window, windowPosCallback);
		glfwSetWindowIconifyCallback(window, windowIconifyCallback);
		glfwSetWindowRefreshCallback(window, windowRefreshCallback);
		glfwSetWindowCloseCallback(window, closeCallback);

		glfwSetKeyCallback(window, keyCallback);
		glfwSetCharCallback(window, charCallback);
		glfwSetCharModsCallback(window, charModsCallback);

		glfwSetCursorPosCallback(window, mouseMove);
		glfwSetMouseButtonCallback(window, mouseKey);
		glfwSetCursorEnterCallback(window, cursorEnter);
		glfwSetScrollCallback(window, mouseScroll);

		glfwSetJoystickCallback(joystickCallback);

		glfwSetDropCallback(window, dropCallback);

		glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
		
		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glfwSwapInterval(1);

		return window;
	}
	static void GlfwRelease(GLFWwindow *window) {
		glfwDestroyWindow(window);
		glfwTerminate();
	}
}