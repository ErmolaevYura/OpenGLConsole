#pragma once
#include "stdafx.h"
#include "repository/core.h"
#include "ogl/glm.h"
#include "ogl/ogl.h"

static const tstring APPNAME = _T("Test OpenGl");
static const int WIDTH = 1024;
static const int HEIGHT = 768;
static const bool FULLSCREEN = false;

namespace ogl {
	class App {
	private:
		int m_majorVersion = 4, m_minorVersion = 4, m_samples = 4;
		tstring m_title = APPNAME;
		int m_width = WIDTH, m_height = HEIGHT;
		bool m_fullscreen = false;
		bool m_focus = true;
		struct {
			float m_lastX = 0.0f;
			float m_lastY = 0.0f;
			bool m_firstMouse = true;
		} m_mouse;

		clock_t m_lastFrame;
		float m_frameInterval;

		int m_FPSCount = 0, m_currentFPS = 0;
		clock_t m_lastSecond = 0;

		glm::mat4 m_projection;
		glm::mat4 m_ortho;
	private:
		App(App const&) = delete;
		App& operator= (App const&) = delete;
		void processKey();
		void movement();
		void render();
	public:
		GLFWwindow* m_window;
		bool keys[1024] = { false };

		bool m_poligonMode = true;	//Полигоны или сетка
		bool m_spotOn = false;		//Фанарь
		bool m_fogOn = true;		//Туман
		bool m_depthOn = false;		//Глубина
		bool m_shadowOn = false;	//Тень
		float m_tessLevel = 64;		//Уровень теселяции
		float m_value = 1.0f;

		void(*m_initScene)(), (*m_renderScene)(), (*m_releaseScene)();
	public:
		App() {};
		~App() {};

		int Init(tstring title = APPNAME,
			int width = WIDTH, int height = HEIGHT, bool fullscreen = FULLSCREEN,
			int majorVersion = 3, int minorVersion = 3, int samples = 4);
		void Deinit();

		void resize(int width, int height);

		int Width() { return m_width; }
		int Height() { return m_height; }

		void switchFullscreen(bool fullScreen);
		bool Fullscreen() { return m_fullscreen; }

		void Focus(bool focus) { m_focus = focus; }
		bool Focus() { return m_focus; }

		void Title(tstring title) { m_title = title; glfwSetWindowTitle(m_window, GetC(m_title.data())); }
		tstring Title() { return m_title; };

		void Projection(float fov, float aspectRation, float _Near, float _Far) {
			m_projection = glm::perspective(fov, aspectRation, _Near, _Far);
		}
		glm::mat4 Projection() { return m_projection; }

		void Ortho(int width, int height) {
			if (width == 0) width = m_width;
			if (height == 0) height = m_height;
			m_ortho = glm::ortho(0.0f, float(width), 0.0f, float(height));
		}
		glm::mat4 Ortho() { return m_ortho; }
		
		bool setVerticalSynchronization(bool enable) { return true; }
		
		void resetTimer() { m_lastFrame = clock(); m_frameInterval = 0.0f; }
		void updateTimer() {
			clock_t tCur = clock();
			m_frameInterval = float(tCur - m_lastFrame) / float(CLOCKS_PER_SEC);
			m_lastFrame = tCur;
		}
		void updateFPS() {
			static double prevision_seconds = glfwGetTime();
			static int frame_count;
			double current_seconds = glfwGetTime();
			double elapsed_seconds = current_seconds - prevision_seconds;
			if (elapsed_seconds > 0.25) {
				prevision_seconds = current_seconds;
				m_FPSCount = (double)frame_count / elapsed_seconds;
				m_title = APPNAME;
				frame_count = 0;
			}
			frame_count++;
		}
		float sof(float fVal) { return fVal * m_frameInterval; }
		int getFPS() { return m_FPSCount; }

		void mouseMovement(int xpos, int ypos);
		void mouseScroll(float yoffset);

		void appBoby();
		void swapBuffers();
	};
	extern App app;
	void initScene();
	void renderScene();
	void releaseScene();
	extern GLfloat rotat;
}