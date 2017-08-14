#pragma once
#include "opengl.h"

namespace ogl {
	class FlyCamera
	{
		inline void updateCameraVectors() {
			glm::vec3 front;
			front.x = cos(glm::radians(m_angle.y)) * cos(glm::radians(m_angle.x));
			front.y = sin(glm::radians(m_angle.y));
			front.z = cos(glm::radians(m_angle.y)) * sin(glm::radians(m_angle.x));
			m_target = glm::normalize(front);
			m_right = glm::normalize(glm::cross(m_target, glm::vec3(0.0f, 1.0f, 0.0f)));
			m_up = glm::normalize(glm::cross(m_right, m_target));
		}
	public:
		enum DIRECTION { FORWARD = 1, BACKWARD = 2, RIGHTWARD = 4, LEFTWARD = 8 };
	public:
		glm::mat4 m_projection;

		glm::vec3 m_position;								//Позиция камеры
		glm::vec3 m_target = glm::vec3(0.0f, 0.0f, -1.0f);	//Цель камеры(инверсия от направления просмотра)
		glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);		//Верх камеры
		glm::vec3 m_right;									//Правая сторона камеры
		
		glm::fvec3 m_angle = glm::fvec3(0.0f, 0.0f, 0.0f); // m_yaw - x rotate; m_pitch - y rotate; m_roll - z rotate
		GLfloat m_speed = 3.0f;
		GLfloat m_mouseSensitivity = 0.05f;
		GLfloat m_zoom = 45.0f;

		FlyCamera(const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 3.0f),
			GLfloat yaw = -90.0f, 
			GLfloat pitch = 0.0f) {
			m_position = position;
			m_angle.x = yaw;
			m_angle.y = pitch;
			updateCameraVectors();
		}
		FlyCamera(GLfloat posX, GLfloat posY, GLfloat posZ,
				GLfloat yaw, GLfloat pitch) {
			m_position = glm::vec3(posX, posY, posZ);
			m_angle.x = yaw;
			m_angle.y = pitch;
			updateCameraVectors();
		}
		~FlyCamera() { }

		inline glm::vec3 Position() const { return m_position; }
		inline void Position(const glm::vec3 &position) { m_position = position; }
		
		inline glm::fvec3 Angle() const { return m_angle; }
		inline void Angle(const glm::fvec3 &angle) { m_angle = angle; updateCameraVectors(); }
		
		inline glm::mat4 View() const { return glm::lookAt(m_position, m_position + m_target, m_up); }
		
		inline glm::mat4 Projection() { return m_projection; }
		inline void Projection(const glm::mat4 &projection) { m_projection = projection; }
		inline glm::mat4 Projection(GLFWwindow *window) {
			int width, height;
			glfwGetWindowSize(window, &width, &height);
			m_projection = glm::perspective(m_zoom, float(width) / float(height), 0.1f, 100.0f);
			return m_projection;
		}
				
		inline void Movement(int direction) {
			static double lastTime = glfwGetTime();
			double currentTime = glfwGetTime();
			GLfloat deltaTime = float(currentTime - lastTime);

			GLfloat velocity = m_speed * deltaTime;
			if (direction & DIRECTION::FORWARD)
				m_position += m_target * velocity;
			else if (direction & DIRECTION::BACKWARD)
				m_position -= m_target * velocity;
			if (direction & DIRECTION::RIGHTWARD)
				m_position += glm::normalize(glm::cross(m_target, m_up)) * velocity;
			else if (direction & DIRECTION::LEFTWARD)
				m_position -= glm::normalize(glm::cross(m_target, m_up)) * velocity;

			lastTime = currentTime;
		}
		inline void ProcessMouseMovement( GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true) {
			xoffset *= m_mouseSensitivity;
			yoffset *= m_mouseSensitivity;
			m_angle.x += xoffset;
			m_angle.y += yoffset;
			if (constrainPitch) {
				if (m_angle.y > 89.0f) m_angle.y = 89.0f;
				else if (m_angle.y < -89.0f) m_angle.y = -89.0f;
			}
			updateCameraVectors();
		}
		inline void ProcessMouseScroll(GLfloat yoffset)  {
			if (m_zoom >= 1.0f && m_zoom <= 45.0f) m_zoom -= yoffset;
			if (m_zoom <= 1.0f) m_zoom = 1.0f;
			else if (m_zoom >= 45.0f) m_zoom = 45.0f;
		}
	};
	class WalkingCamera {
		enum DIRECTION { FORWARD = 1, BACKWARD = 2, RIGHTWARD = 4, LEFTWARD = 8 };
		glm::mat4 look() { return glm::lookAt(m_eye, m_view, m_up); }
		void update(int direction) {
			static double lastTime = glfwGetTime();
			double currentTime = glfwGetTime();
			GLfloat deltaTime = float(currentTime - lastTime);

			GLfloat velocity = m_speed * deltaTime;
			if (direction & DIRECTION::FORWARD)
				move(m_speed * deltaTime);
			else if (direction & DIRECTION::BACKWARD)
				move(-m_speed * deltaTime);
			if (direction & DIRECTION::RIGHTWARD)
				rotateViewY(90.0f * deltaTime);
			else if (direction & DIRECTION::LEFTWARD)
				rotateViewY(-90.0f * deltaTime);

			lastTime = currentTime;
		}
		void rotateViewY(float angle) {
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec3 dir = m_view - m_eye;
			glm::vec4 newView = rotation * glm::vec4(dir, 1.0f);
			m_view = glm::vec3(newView.x, newView.y, newView.x);
			m_view += m_eye;
		}
		void move(float by) {
			glm::vec3 dir = m_view - m_eye;
			dir *= by;
			m_eye += dir;
			m_view += dir;
		}
		WalkingCamera(const glm::vec3 &eye = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3 &view = glm::vec3(0.0f, 0.0f, -1.0f),
			const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f), float speed = 25.0f) 
		: m_eye(eye), m_view(view), m_up(up), m_speed(speed) { }
	private: 
		glm::vec3 m_eye, m_view, m_up;
		float m_speed;
	};
};
