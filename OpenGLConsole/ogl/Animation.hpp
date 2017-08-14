#pragma once
#include <memory>
#include "opengl.h"

namespace ogl {
	/* Анимация Масштабирования */
	class iScaleAnimation
	{
		GLboolean m_enable;
		float(*scaleX)() = []() { return 0.0f; };
		float(*scaleY)() = []() { return 0.0f; };
		float(*scaleZ)() = []() { return 0.0f; };
	public:
		iScaleAnimation(GLboolean enable = false) : m_enable(enable) { }
		~iScaleAnimation() { }
		inline void MovementX(float(*scaleX)()) { scaleX = scaleX; }
		inline void MovementY(float(*scaleY)()) { scaleY = scaleY; }
		inline void MovementZ(float(*scaleZ)()) { scaleZ = scaleZ; }
		inline void Enable(GLboolean enable) { m_enable = enable; }
		inline glm::vec3 ScaleNext() const {
			return m_enable ?
				glm::vec3(scaleX(), scaleY(), scaleZ())
				: glm::vec3(0.0f, 0.0f, 0.0f);
		}
		glm::mat4 Next(glm::mat4 &model) const {
			if (m_enable)
				model = glm::scale(model,
					glm::vec3(
						scaleX(),
						scaleY(),
						scaleZ()
					)
				);
			return model;
		}
	};
	/* Анимация вращения */
	class iRotateAnimation
	{
		glm::vec3 m_angle = glm::vec3(0.0f);
		GLboolean m_enable = false;										// Включение/Остоновка анимации
	public:
		iRotateAnimation(const iRotateAnimation &rotate) : m_angle(rotate.m_angle), m_enable(rotate.m_enable) { }
		iRotateAnimation(const iRotateAnimation *rotate) : m_angle(rotate->m_angle), m_enable(rotate->m_enable) { }
		iRotateAnimation(const glm::vec3 &angle, GLboolean enable) : m_angle(angle), m_enable(enable) { }
		iRotateAnimation() { }
		~iRotateAnimation() { }
		/* Получить/Установить ориентацию */
		inline glm::vec3 Angle() const { return m_angle; }
		inline void Angle(const glm::vec3 &angle) { m_angle = angle; }
		/* Включить/Выключить анимацию */
		inline void Enable(GLboolean enable) { m_enable = enable; }
		/* Следующий кадр */
		glm::mat4 Next(glm::mat4 &model) {
			if (m_enable) {
				float time = (GLfloat)glfwGetTime();
				glm::quat q(1.0f, 0.0f, 0.0f, 0.0f);
				if (m_angle.x != 0) q *= glm::angleAxis(glm::radians(time * m_angle.x), glm::vec3(1.0f, 0.0f, 0.0f));
				if (m_angle.y != 0) q *= glm::angleAxis(glm::radians(time * m_angle.y), glm::vec3(0.0f, 1.0f, 0.0f));
				if (m_angle.z != 0) q *= glm::angleAxis(glm::radians(time * m_angle.z), glm::vec3(0.0f, 0.0f, 1.0f));
				model *= mat4_cast(q);
			}
			return model;
		}
	};
	/* Анимация Движения */
	class iMovementAnimation
	{
		GLboolean m_enable;							// Включение/Остоновка анимации				
		float(*moveX)() = []() { return 0.0f; };	// Функция поведения на оси X
		float(*moveY)() = []() { return 0.0f; };	// Функция поведения на оси Y
		float(*moveZ)() = []() { return 0.0f; };	// Функция поведения на оси Z
	public:
		iMovementAnimation(const iMovementAnimation &move) : m_enable(move.m_enable) {
			moveX = move.moveX;
			moveY = move.moveY;
			moveZ = move.moveZ;
		}
		iMovementAnimation(iMovementAnimation *move) : m_enable(move->m_enable) {
			moveX = move->moveX;
			moveY = move->moveY;
			moveZ = move->moveZ;
		}
		iMovementAnimation(GLboolean enable = false) : m_enable(enable) { }
		~iMovementAnimation() { }
		inline void MovementX(float(*calculateX)()) { calculateX = calculateX; }
		inline void MovementY(float(*calculateY)()) { calculateY = calculateY; }
		inline void MovementZ(float(*calculateZ)()) { calculateZ = calculateZ; }
		inline void Enable(GLboolean enable) { m_enable = enable; }
		inline glm::vec3 MovementNext() const {
			return m_enable ?
				glm::vec3(moveX(), moveY(), moveZ())
				: glm::vec3(0.0f, 0.0f, 0.0f);
		}
		glm::mat4 Next(glm::mat4 &model) const {
			if (m_enable) model = glm::translate(model, MovementNext());
			return model;
		}
	};
	/* Общяя анимация */
	struct Animation
	{
		std::unique_ptr<ogl::iScaleAnimation> Scale = nullptr;
		std::unique_ptr<ogl::iRotateAnimation> Rotate = nullptr;
		std::unique_ptr<ogl::iMovementAnimation> Movement = nullptr;
		Animation() { }
		Animation(Animation *animate) {
			if (animate->Rotate != nullptr) Rotate = std::move(animate->Rotate);
			if (animate->Movement != nullptr) Movement = std::move(animate->Movement);
			if (animate->Scale != nullptr) Scale = std::move(animate->Scale);
		}
		glm::mat4 Next(glm::mat4 &model) const {
			if (Movement != nullptr) model = Movement->Next(model);
			if (Rotate != nullptr) model = Rotate->Next(model);
			if (Scale != nullptr) model = Scale->Next(model);
			return model;
		}
	};
}
