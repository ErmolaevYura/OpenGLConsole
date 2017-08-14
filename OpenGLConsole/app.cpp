#include "stdafx.h"
#include "app.h"
#include "ogl/Camera.hpp"
#include <Windows.h>
using namespace ogl;

App ogl::app;
GLfloat ogl::rotat = 0.75f;
int App::Init(tstring title, int width, int height, bool fullscreen,
	int majorVersion, int minorVersion, int samples) {
	m_title = title; m_width = width; m_height = height; m_fullscreen = fullscreen;
	m_majorVersion = majorVersion; m_minorVersion = minorVersion, m_samples = samples;
	core::Init();
	m_window = ogl::Init(m_title, m_width, m_height, m_fullscreen,
		m_majorVersion, m_minorVersion, m_samples);
	if (!m_window) return 1;
	Ortho(width, height);
	Projection(45.0f, float(width) / float(height), 0.1f, 200.0f);
	loadResource("resource.json");
	m_renderScene = renderScene;
	m_initScene = initScene;
	m_releaseScene = releaseScene;
	if (m_initScene != nullptr) m_initScene();
	return 0;
}
void App::Deinit() {
	if (m_releaseScene) m_releaseScene();
	core::logger->UninitThread();
	ConsoleLogger::UninitThread();
	core::Deinit();
}

void App::resize(int width, int height) {
	m_width = width;
	m_height = height;
	Ortho(width, height);
	Projection(45.0f, float(width) / float(height), 0.1f, 100.0f);
	glViewport(0, 0, m_width, m_height);
}
void App::switchFullscreen(bool fullScreen) {
	if (m_fullscreen == fullScreen) return;
	m_fullscreen = fullScreen;
	if (m_fullscreen) {
		GLFWmonitor* mon = glfwGetPrimaryMonitor();
		const GLFWvidmode* vmode = glfwGetVideoMode(mon);
		glfwSetWindowMonitor(m_window, mon, 0, 0, vmode->width, vmode->height, GLFW_DONT_CARE);
	}
	else glfwSetWindowMonitor(m_window, nullptr, 0, 0, m_width, m_height, GLFW_DONT_CARE);
}

void App::mouseMovement(int xpos, int ypos) {
	if (m_mouse.m_firstMouse) {
		m_mouse.m_lastX = xpos;
		m_mouse.m_lastY = ypos;
		m_mouse.m_firstMouse = false;
	}
	double xoffset = xpos - m_mouse.m_lastX;
	double yoffset = m_mouse.m_lastY - ypos;

	m_mouse.m_lastX = xpos;
	m_mouse.m_lastY = ypos;

	getFlyCamera()->ProcessMouseMovement(xoffset, yoffset);
}
void App::mouseScroll(float yoffset) { 
	ogl::rotat -= 0.01f;
	glfwSetWindowTitle(m_window, GetC(to_tstring(ogl::rotat).data()));
	getFlyCamera()->ProcessMouseScroll(yoffset);
}
void App::processKey() {
	if (keys[GLFW_KEY_P]) {
		m_poligonMode = !m_poligonMode;
		Sleep(200);
	}
	if (keys[GLFW_KEY_H]) {
		m_spotOn = !m_spotOn;
		Sleep(200);
	}
	if (keys[GLFW_KEY_F]) {
		m_fogOn = !m_fogOn;
		Sleep(200);
	}
	if (keys[GLFW_KEY_Q]) {
		m_depthOn = !m_depthOn;
		Sleep(200);
	}
	if (keys[GLFW_KEY_T]) {
		m_shadowOn = !m_shadowOn;
		Sleep(200);
	}
	if (keys[GLFW_KEY_COMMA]) {
		if (m_tessLevel > 0) {
			m_tessLevel--;
			Sleep(200);
		}
	}
	if (keys[GLFW_KEY_KP_ADD]) {
		m_value++;
		Sleep(200);
	}
	if (keys[GLFW_KEY_KP_SUBTRACT]) {
		if (m_value > 1) {
			m_value--;
			Sleep(200);
		}
	}
	if (keys[GLFW_KEY_PERIOD]) {
		m_tessLevel++;
		Sleep(200);
	}
}
void App::movement() {
	int direction = 0;
	if (keys[GLFW_KEY_W]) direction |= ogl::FlyCamera::FORWARD;
	else if (keys[GLFW_KEY_S]) direction |= ogl::FlyCamera::BACKWARD;
	if (keys[GLFW_KEY_A]) direction |= ogl::FlyCamera::LEFTWARD;
	else if (keys[GLFW_KEY_D]) direction |= ogl::FlyCamera::RIGHTWARD;
	getFlyCamera()->Movement(direction);
}

void App::render() {
	updateFPS();
	if (m_renderScene) m_renderScene();
}
void App::appBoby() {
	if (m_focus) {
		processKey();
		movement();
		render();
	}
	else Sleep(200);
}
void App::swapBuffers() {
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}