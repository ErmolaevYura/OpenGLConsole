#include "stdafx.h"
#include "opengl.h"
using namespace ogl;

void ogl::monitorCallback(GLFWmonitor* monitor, int event) {
	if (event == GLFW_CONNECTED) {
		// The monitor was connected
	}
	else if (event == GLFW_DISCONNECTED) {
		// The monitor was disconnected
	}
}

void ogl::resizeCallback(GLFWwindow* window, int width, int height) {
	app.resize(width, height);
}
void ogl::focusCallback(GLFWwindow* window, int focus) {
	app.Focus(focus == GLFW_TRUE ? true : false);
}
void ogl::windowPosCallback(GLFWwindow* window, int xpos, int ypos) { }
void ogl::windowIconifyCallback(GLFWwindow* window, int iconified) {
	if (iconified) {
		// The window was iconified
	} 
	else {
		// The window was restored
	}
}
void ogl::windowRefreshCallback(GLFWwindow* window) {
	glfwSwapBuffers(window);
}
void ogl::closeCallback(GLFWwindow* window) {
	glfwSetWindowShouldClose(window, GLFW_FALSE);
}

void ogl::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
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
void ogl::charCallback(GLFWwindow * window, unsigned int ch) {
	// handle char
}
void ogl::charModsCallback(GLFWwindow* window, unsigned int codepoint, int mods) { }

void ogl::mouseMove(GLFWwindow* window, double xpos, double ypos) {
	double xoffset, yoffset;
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	app.mouseMovement(xpos, ypos);
}
void ogl::mouseKey(GLFWwindow * window, int button, int action, int mode) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS);
		// handle right mouse-click
}
void ogl::cursorEnter(GLFWwindow * window, int intered) {
	// handle cursor enter or leave
}
void ogl::mouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
	app.mouseScroll(yoffset);
}
void ogl::mouseClick(int b, int s, int mouse_x, int mouse_y) {
	/*
	//пересечение луча с объектами от места клика в глубину сцены
	//нормирование координат положени€ курсора
	float x = (2.0f * mouse_x) / appMain.m_width - 1.0f;
	float y = 1.0f - (2.0f * mouse_y) / appMain.m_height;
	float z = 1.0f;
	vec3 ray_nds = vec3(x, y, z);
	//построение вектора луча в глубину
	vec4 ray_clip = vec4(ray_nds.xy, -1.0, 1.0);
	//координаты камеры??
	vec4 ray_eye = inverse(projection_matrix) * ray_clip;
	ray_eye = vec4(ray_eye.xy, -1.0, 0.0);
	//ћировые координаты
	vec3 = ray_world = (inverse(view_matrix) * ray_eye).xyz;
	ray_world = normalize(ray_world);
	*/
}

void ogl::joystickCallback(int joy, int event) {
	if (event == GLFW_CONNECTED) {
		// The joystick was connected
	}
	else if (event == GLFW_DISCONNECTED) {
		// The joystick was disconnected
	}
}

void ogl::dropCallback(GLFWwindow * win, int count, const char ** paths) {
	for (int i = 0; i < count; i++)
		ConsoleLogger::AddInfo(GetString(paths[i]));
}

void ogl::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}