#pragma once

#include <gl/glm/vec3.hpp>
#include <gl/glm/vec4.hpp>
#include <gl/glm/matrix.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <gl/glm/gtc/quaternion.hpp>

namespace ogl {
	/* Универсальный класс описывающий положение модели */
	class Transformation3D {
	private:
		glm::vec3 m_scale = glm::vec3(1.0f);							// Вектор масштибирования
		glm::quat m_orientation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);	// Квантернион ориентации
		glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);				// Вектор положения
	public:
		Transformation3D() { }
		Transformation3D(const glm::vec3 &scale, float angle, const glm::vec3 &normal, const glm::vec3 &position)
			: m_scale(scale)
			, m_orientation(glm::angleAxis(angle, normal))
			, m_position(position)
		{ }
		
		/* Матрица преобразования */
		glm::mat4 toMat4() const {
			const glm::mat4 scaleMatrix = glm::scale(glm::mat4(), m_scale);
			const glm::mat4 rotationMatrix = glm::mat4_cast(m_orientation);
			const glm::mat4 translateMatrix = glm::translate(glm::mat4(), m_position);
			return translateMatrix * rotationMatrix * scaleMatrix;
		}
		
		/* Масштабирование*/
		inline glm::vec3 Scale() const { return m_scale; }
		inline Transformation3D* Scale(const glm::vec3 &scale) { m_scale = scale; return this; }
		inline Transformation3D* Scale(GLfloat scale) { m_scale = glm::vec3(scale); return this; }

		/* Позиция */
		inline glm::vec3 Position() const { return m_position; }
		inline Transformation3D* Position(const glm::vec3 &position) { m_position = position; return this; }

		/* Ориентация */
		inline glm::quat Orientation() const { return m_orientation; }
		inline glm::vec3 OrientationVec3() const { return glm::eulerAngles(m_orientation); }
		inline Transformation3D* Orientation(const glm::quat &orientation) { m_orientation = orientation; return this; }
		inline Transformation3D* OrientationVec3(float angle, const glm::vec3 &normal) { m_orientation = glm::angleAxis(angle, normal); return this; }
	};
}