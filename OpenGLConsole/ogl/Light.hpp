#pragma once
#include <memory>
#include "Shader.hpp"
#include "FrameBuffer.hpp"
#include "Primitives.hpp"

namespace ogl {
	/* Абстрактный класс */
	class BaseLight
	{
	protected:
		glm::vec3 m_diffuse;		// Мощность рассеянного освещения (diffuse)
		glm::vec3 m_color = glm::vec3(1.0f, 1.0f, 1.0f);	// Цвет
		GLuint m_depthMap = 0;		// Карта теней
	public:
		BaseLight(const glm::vec3 &diffuse = glm::vec3(0.5f, 0.5f, 0.5f)) : m_diffuse(diffuse) { };
		virtual ~BaseLight() { }

		inline glm::vec3 Diffuse() const { return m_diffuse; }
		inline void Diffuse(const glm::vec3 &diffuse) { m_diffuse = diffuse; }
		inline void Diffuse(GLfloat x, GLfloat y, GLfloat z) { Diffuse(glm::vec3(x, y, z)); }

		inline glm::vec3 Color() const { return m_color; }
		inline void Color(const glm::vec3 &color) { m_color = color; }
		inline void Color(GLfloat r, GLfloat g, GLfloat b) { Color(glm::vec3(r, g, b)); }

		virtual std::shared_ptr<Shader> bind(std::shared_ptr<Shader> shader, const std::string &name = "light") const {
			shader
				->bind(name + ".diffuse", m_diffuse)
				->bind(name + ".color", m_color)
				;
			return shader;
		}
		virtual glm::mat4 getLightMVP() const = 0;
		
		/* Рендер в карту теней */
		void RenderDepth(std::shared_ptr<Shader> shader, std::list<Primitives*> lst) {
			static FrameBuffer fbo(app.Width(), app.Height(), FrameBuffer::FRAMETYPE::DEPTH);
			if (m_depthMap != 0) fbo.attachDepthTexture(m_depthMap);
			else if (fbo.DepthTexture() == 0) m_depthMap = fbo.initDephtTexture();
			else m_depthMap = fbo.DepthTexture();
			fbo.bind();
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			shader->activate()->bind("lightMVP", getLightMVP());
			for each (auto obj in lst)
				obj->Draw(shader);
			glDisable(GL_CULL_FACE);
			fbo.attachDepthTexture();
			fbo.unbind();
		}
		
		/* Рендер на экран */
		void RenderDepthToScreen(std::shared_ptr<Shader> shader, std::list<Primitives*> lst, GLuint x, GLuint y, GLuint width, GLuint height) {
			int curViewport[4];
			glGetIntegerv(GL_VIEWPORT, curViewport);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			glViewport(x, y, width, height);
			shader->activate()->bind("lightMVP", getLightMVP());
			for each (auto obj in lst)
				obj->Draw(shader);
			glDisable(GL_CULL_FACE);
			glViewport(curViewport[0], curViewport[1], curViewport[2], curViewport[3]);
		}
	};
	/* Направленный источник света (directional light) */
	class DirLight : public BaseLight
	{
	protected:
		glm::vec3 m_ambient;	//< Мощность фонового освещения (ambient)
		glm::vec3 m_direction;	//< Направление света
	public:
		DirLight(
			const glm::vec3 &direction, 
			const glm::vec3 &ambient = glm::vec3(0.15f, 0.15f, 0.15f), 
			const glm::vec3 &diffuse = glm::vec3(0.1f, 0.1f, 0.1f)
		) : BaseLight(diffuse), m_ambient(ambient), m_direction(direction) { };
		virtual ~DirLight() { }

		inline glm::vec3 Ambient() const { return m_ambient; }
		inline void Ambient(const glm::vec3 &ambient) { m_ambient = ambient; }
		inline void Ambient(GLfloat x, GLfloat y, GLfloat z) { Ambient(glm::vec3(x, y, z)); }

		inline glm::vec3 Direction() const { return m_direction; }
		inline void Direction(const glm::vec3 &direction) { m_direction = direction; }
		inline void Direction(GLfloat x, GLfloat y, GLfloat z) { Direction(glm::vec3(x, y, z)); }
		
		/*Матрица координат пространства источника света*/
		glm::mat4 getLightMVP() const {
			glm::mat4 projection = glm::ortho(-20.0f, 20.0f, -10.0f, 20.0f, -20.0f, 20.0f);
			glm::vec3 right = glm::cross(m_direction, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 view = glm::lookAt(
				m_direction * -5.0f,
				m_direction,
				glm::cross(right, m_direction)
			);
			return projection * view; // *glm::mat4(1.0f);
		}
		
		/* Привязка источника к шейдру без привязки MVP и теневой карты*/
		std::shared_ptr<Shader> bind(std::shared_ptr<Shader> shader, const std::string &name = "dirLight") const {
			BaseLight::bind(shader, name)
				->bind(name + ".ambient", m_ambient)
				->bind(name + ".direction", m_direction)
				;
			return shader;
		}

		/* Привязка источника к шейдру с привязкой MVP и теневой карты*/
		std::shared_ptr<Shader> bindWithShadow(std::shared_ptr<Shader> shader, GLuint shadowTexture, const std::string &name = "dirLight") const {
			BaseLight::bind(shader, name)
				->bind(name + ".ambient", m_ambient)
				->bind(name + ".direction", m_direction)
				->bind(name + ".lightMVP", getLightMVP())
				->bind(name + ".shadowMap", shadowTexture)
				;
			glActiveTexture(GL_TEXTURE0 + shadowTexture);
			glBindTexture(GL_TEXTURE_2D, m_depthMap);
			return shader;
		}
	};
	/* Точечный источник света (point light) */
	class PointLight : public BaseLight
	{
	protected:
		glm::vec3 m_specular;		//< Мощность отраженного освещения (specular)
		glm::vec3 m_position;		//< Положение в пространстве (position)
		float m_constant = 1.0f;	//< Коэффициенты затухания (attenuation)
		float m_linear = 0.09f;		//< Коэффициенты затухания (attenuation)
		float m_quadratic = 0.032f;	//< Коэффициенты затухания (attenuation)
	public:
		enum class DISTANCE : std::int16_t {
			D7 = 7, D13 = 13, D20 = 20, D32 = 23, D50 = 50, D65 = 65, D100 = 100,
			D160 = 160, D200 = 200, D325 = 325, D600 = 600, D3250 = 3250
		};
		enum class DIRECTION : std::int8_t { UP, DOWN, LEFT, RIGHT, FORWARD, BACK };
		PointLight(const glm::vec3 &position,
			const glm::vec3 &diffuse = glm::vec3(0.65f, 0.65f, 0.05f),
			const glm::vec3 &specular = glm::vec3(0.8f, 0.8f, 0.8f)
		) : BaseLight(diffuse), m_specular(specular), m_position(position) { };
		virtual ~PointLight() { }

		inline glm::vec3 Specular() const { return m_specular; }
		inline void Specular(const glm::vec3 &specular) { m_specular = specular; }
		inline void Specular(GLfloat x, GLfloat y, GLfloat z) { Specular(glm::vec3(x, y, z)); }

		inline glm::vec3 Position() const { return m_position; }
		inline void Position(const glm::vec3 &position) { m_position = position; }
		inline void Position(GLfloat x, GLfloat y, GLfloat z) { Position(glm::vec3(x, y, z)); }

		inline float Constant() const { return m_constant; }
		inline void Constant(float constant) { m_constant = constant; }

		inline float Linear() const { return m_linear; }
		inline void Linear(float linear) { m_linear = linear; }

		inline float Quadratic() const { return m_quadratic; }
		inline void Quadratic(float quadratic) { m_quadratic = quadratic; }

		inline glm::mat4 getLightMVP() const { return	getLightMVP(DIRECTION::DOWN); }
		glm::mat4 getLightMVP(DIRECTION direct) const {
			glm::mat4 projection = glm::perspective<float>(glm::radians(45.0f), app.Width() / app.Height(), 1.0f, 30.0f);
			glm::vec3 direction, up;
			switch(direct) {
			case DIRECTION::UP: direction = glm::vec3(0.0f, -1.0f, 0.0f); up = glm::vec3(0.0f, 0.0f, -1.0f); break;
			case DIRECTION::DOWN: direction = glm::vec3(0.0f, 1.0f, 0.0f); up = glm::vec3(0.0f, 0.0f, 1.0f); break;
			case DIRECTION::FORWARD: direction = glm::vec3(0.0f, 0.0f, -1.0f); up = glm::vec3(0.0f, 1.0f, 0.0f); break;
			case DIRECTION::BACK: direction = glm::vec3(0.0f, 0.0f, 1.0f); up = glm::vec3(0.0f, 1.0f, 0.0f); break;
			case DIRECTION::LEFT: direction = glm::vec3(-1.0f, 0.0f, 0.0f); up = glm::vec3(0.0f, 1.0f, 0.0f); break;
			case DIRECTION::RIGHT: direction = glm::vec3(1.0f, 0.0f, 0.0f); up = glm::vec3(0.0f, 1.0f, 0.0f); break;
			default: direction = glm::vec3(0.0f, 1.0f, 0.0f); up = glm::vec3(0.0f, 0.0f, -1.0f); break;
			}
			glm::mat4 view = glm::lookAt(m_position, m_position - direction, up);
			return projection * view; // *glm::mat4(1.0f);
		}
		glm::mat4 getLightMVP(glm::vec3 direction) const {
			glm::mat4 projection = glm::perspective<float>(glm::radians(45.0f), 1.0f, 2.0f, 50.0f);
			glm::vec3 right = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
			glm::vec3 up = glm::normalize(glm::cross(direction, right));
			glm::mat4 view = glm::lookAt(m_position, m_position - direction, up);
			return projection * view; // *glm::mat4(1.0f);
		}
		
		std::shared_ptr<Shader> bind(std::shared_ptr<Shader> shader, const std::string &name = "pointLight") const {
			BaseLight::bind(shader, name)
				->bind(name + ".specular", m_specular)
				->bind(name + ".position", m_position)
				->bind(name + ".constant", m_constant)
				->bind(name + ".linear", m_linear)
				->bind(name + ".quadratic", m_quadratic)
				;
			return shader;
		}
		std::shared_ptr<Shader> bindWithShadow(std::shared_ptr<Shader> shader, GLuint shadowTexture, const std::string &name = "pointLight") const {
			BaseLight::bind(shader, name)
				->bind(name + ".specular", m_specular)
				->bind(name + ".position", m_position)
				->bind(name + ".constant", m_constant)
				->bind(name + ".linear", m_linear)
				->bind(name + ".quadratic", m_quadratic)
				->bind(name + ".lightMVP", getLightMVP())
				->bind(name + ".shadowMap", shadowTexture)
				;
			glActiveTexture(GL_TEXTURE0 + shadowTexture);
			glBindTexture(GL_TEXTURE_2D, m_depthMap);
			return shader;
		}

		void CreatePointLight(DISTANCE distance) {
			m_constant = 1.0f;
			switch (distance) {
			case DISTANCE::D7:		m_linear = 0.7f;	m_quadratic = 1.8f;		break;
			case DISTANCE::D13:		m_linear = 0.35f;	m_quadratic = 0.44f;	break;
			case DISTANCE::D20:		m_linear = 0.22f;	m_quadratic = 0.20f;	break;
			case DISTANCE::D32:		m_linear = 0.14f;	m_quadratic = 0.07f;	break;
			case DISTANCE::D50:		m_linear = 0.09f;	m_quadratic = 0.032f;	break;
			case DISTANCE::D65:		m_linear = 0.07f;	m_quadratic = 0.017f;	break;
			case DISTANCE::D100:	m_linear = 0.045f;	m_quadratic = 0.0075f;	break;
			case DISTANCE::D160:	m_linear = 0.027f;	m_quadratic = 0.0028f;	break;
			case DISTANCE::D200:	m_linear = 0.022f;	m_quadratic = 0.0019f;	break;
			case DISTANCE::D325:	m_linear = 0.014f;	m_quadratic = 0.0007f;	break;
			case DISTANCE::D600:	m_linear = 0.007f;	m_quadratic = 0.0002f;	break;
			case DISTANCE::D3250:	m_linear = 0.0014f; m_quadratic = 0.000007; break;
			default:				m_linear = 0.7f;	m_quadratic = 1.8f;		break;
			}
		}
	};
	/* Прожектор (spot light) */
	class SpotLight : public PointLight
	{
	protected:
		glm::vec3 m_direction;	//< Направление света

		float m_cutOff = 10.5f;
		float m_outerCutOff = 11.5f;
		float m_rcutOff = glm::cos(glm::radians(10.5f));
		float m_routerCutOff = glm::cos(glm::radians(11.5f));
	public:
		SpotLight(
			const glm::vec3 &direction, glm::vec3 position,
			const glm::vec3 &diffuse = glm::vec3(0.35f, 0.35f, 0.35f),
			const glm::vec3 &specular = glm::vec3(0.45f, 0.45f, 0.45f)
		) : PointLight(position, diffuse, specular), m_direction(direction)  { };
		virtual ~SpotLight() { }

		inline glm::vec3 Direction() const { return m_direction; }
		inline void Direction(const glm::vec3 &direction) { m_direction = direction; }
		inline void Direction(GLfloat x, GLfloat y, GLfloat z) { Direction(glm::vec3(x, y, z)); }

		inline float CutOff() const { return m_cutOff; }
		inline void CutOff(float cutOff) { m_cutOff = cutOff; m_rcutOff = glm::cos(glm::radians(m_cutOff)); }

		inline float OuterCutOff() const { return m_outerCutOff; }
		inline void OuterCutOff(float outherCutOff) { m_outerCutOff = outherCutOff;  m_routerCutOff = glm::cos(glm::radians(m_outerCutOff)); }

		/*Матрица координат пространства источника света*/
		glm::mat4 getLightMVP() const {
			glm::mat4 projection = glm::perspective<float>(glm::radians(45.0f), app.Width() / app.Height(),  0.1f, 50.0f);
			auto right = glm::normalize(glm::cross(-m_direction, glm::vec3(0.0f, 1.0f, 0.0f)));
			auto up = glm::normalize(glm::cross(right, m_direction));
			glm::mat4 view = glm::lookAt(m_position, m_position - m_direction, up);
			return projection * view; // *glm::mat4(1.0f);
		}

		/* Привязка источника к шейдру без привязки MVP и теневой карты*/
		std::shared_ptr<Shader> bind(std::shared_ptr<Shader> shader, const std::string &name = "spotLight") const {
			PointLight::bind(shader, name)
				->bind(name + ".direction", m_direction)
				->bind(name + ".cutOff", m_rcutOff)
				->bind(name + ".outerCutOff", m_routerCutOff)
				;
			return shader;
		}
		
		/* Привязка источника к шейдру с привязкой MVP и теневой карты*/
		std::shared_ptr<Shader> bindWithShadow(std::shared_ptr<Shader> shader, GLuint shadowTexture, const std::string &name = "spotLight") const {
			PointLight::bind(shader, name)
				->bind(name + ".direction", m_direction)
				->bind(name + ".cutOff", m_rcutOff)
				->bind(name + ".outerCutOff", m_routerCutOff)
				->bind(name + ".lightMVP", getLightMVP())
				->bind(name + ".shadowMap", shadowTexture)
				;
			glActiveTexture(GL_TEXTURE0 + shadowTexture);
			glBindTexture(GL_TEXTURE_2D, m_depthMap);
			return shader;
		}
	};
};
