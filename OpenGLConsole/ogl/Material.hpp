#pragma once
#include "Shader.hpp"
#include "Texture.hpp"

namespace ogl {
	class ColorMaterial {
	protected:
		glm::vec3 m_color;			// Цвет материала
		glm::vec3 m_ambient;		// Окружающее освещение (постоянная света, при отсутствии его источников) - световой фон
		glm::vec3 m_diffuse;		// Дифузное освещение (направленное воздействие света)
		glm::vec3 m_specular;		// Зеркальное освещение (яркое пятно света на блестящей поверхности, склонна к цвету света)
		GLfloat m_shininess;		// Рассеянность света
	public:
		enum class MATERIAL : std::int8_t {
			EMERALD = 1, JADE = 2, OBSIDIAN = 3, PEARL = 4, RUBY = 5, TURQUOISE = 6, BRASS = 7,
			BRONZE = 8, CHROME = 9, COPPER = 10, GOLD = 11, SILVER = 12, BLACK_PLASTIC = 13,
			CYAN_PLASTIC = 14, GREEN_PLASTIC = 15, RED_PLASTIC = 16, WHITE_PLASTIC = 17,
			YELLOW_PLASTIC = 18, BLACK_RUBBER = 19, CYAN_RUBBER = 20, GREEN_RUBBER = 21,
			RED_RUBBER = 22, WHITE_RUBBER = 23, YELLOW_RUBBER = 24
		};
		ColorMaterial(glm::vec3 color = glm::vec3(1.0f, 0.5f, 0.31f),
			glm::vec3 ambient = glm::vec3(0.24725f, 0.1995f, 0.0745f),
			glm::vec3 diffuse = glm::vec3(0.75164f, 0.60648f, 0.22648f),
			glm::vec3 specular = glm::vec3(0.628281f, 0.555802f, 0.366065f),
			GLfloat shininess = 0.4f) : m_color(color), m_diffuse(diffuse), m_specular(specular), m_shininess(shininess) { }
		virtual ~ColorMaterial() {}

		void selectMaterial(MATERIAL material) {
			switch (material) {
			case MATERIAL::EMERALD:
				m_ambient = glm::vec3(0.0215f, 0.1745f, 0.0215f); 
				m_diffuse = glm::vec3(0.07568f, 0.61424f, 0.07568f);
				m_specular = glm::vec3(0.633f, 0.727811f, 0.633f); 
				m_shininess = 0.6f;
				break;
			case MATERIAL::JADE:
				m_ambient = glm::vec3(0.135f, 0.2225f, 0.1575f);
				m_diffuse = glm::vec3(0.54f, 0.89f, 0.63f);
				m_specular = glm::vec3(0.316228f, 0.316228f, 0.316228f);
				m_shininess = 0.1f;
				break;
			case MATERIAL::OBSIDIAN:
				m_ambient = glm::vec3(0.05375f, 0.05f, 0.06625f);
				m_diffuse = glm::vec3(0.18275f, 0.17f, 0.22525f);
				m_specular = glm::vec3(0.332741f, 0.328634f, 0.346435f);
				m_shininess = 0.3f;
				break;
			case MATERIAL::PEARL:
				m_ambient = glm::vec3(0.25f, 0.20725f, 0.20725f);
				m_diffuse = glm::vec3(1.0f, 0.829f, 0.829f);
				m_specular = glm::vec3(0.296648f, 0.296648f, 0.296648f);
				m_shininess = 0.088f;
				break;
			case MATERIAL::RUBY:
				m_ambient = glm::vec3(0.1745f, 0.01175f, 0.01175f);
				m_diffuse = glm::vec3(0.61424f, 0.04136f, 0.04136f);
				m_specular = glm::vec3(0.727811f, 0.626959f, 0.626959f);
				m_shininess = 0.6f;
				break;
			case MATERIAL::TURQUOISE:
				m_ambient = glm::vec3(0.1f, 0.18725f, 0.1745f);
				m_diffuse = glm::vec3(0.396f, 0.74151f, 0.69102f);
				m_specular = glm::vec3(0.297254f, 0.30829f, 0.306678f);
				m_shininess = 0.1f;
				break;						
			case MATERIAL::BRASS:
				m_ambient = glm::vec3(0.329412f, 0.223529f, 0.027451f);
				m_diffuse = glm::vec3(0.780392f, 0.568627f, 0.113725f);
				m_specular = glm::vec3(0.992157f, 0.941176f, 0.807843f);
				m_shininess = 0.21794872f;
				break;
			case MATERIAL::BRONZE:
				m_ambient = glm::vec3(0.2125f, 0.1275f, 0.054f);
				m_diffuse = glm::vec3(0.714f, 0.4284f, 0.18144f);
				m_specular = glm::vec3(0.393548f, 0.271906f, 0.166721f);
				m_shininess = 0.2f;
				break;
			case MATERIAL::CHROME:
				m_ambient = glm::vec3(0.25f, 0.25f, 0.25f);
				m_diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
				m_specular = glm::vec3(0.774597f, 0.774597f, 0.774597f);
				m_shininess = 0.6f;
				break;
			case MATERIAL::COPPER:
				m_ambient = glm::vec3(0.19125f, 0.0735f, 0.0225f);
				m_diffuse = glm::vec3(0.7038f, 0.27048f, 0.0828f);
				m_specular = glm::vec3(0.256777f, 0.137622f, 0.086014f);
				m_shininess = 0.1f;
				break;
			case MATERIAL::GOLD:
				m_ambient = glm::vec3(0.24725f, 0.1995f, 0.0745f);
				m_diffuse = glm::vec3(0.75164f, 0.60648f, 0.22648f);
				m_specular = glm::vec3(0.628281f, 0.555802f, 0.366065f);
				m_shininess = 0.4f;
				break;
			case MATERIAL::SILVER:
				m_ambient = glm::vec3(0.19225f, 0.19225f, 0.19225f);
				m_diffuse = glm::vec3(0.50754f, 0.50754f, 0.50754f);
				m_specular = glm::vec3(0.508273f, 0.508273f, 0.508273f);
				m_shininess = 0.4f;
				break;
			case MATERIAL::BLACK_PLASTIC:
				m_ambient = glm::vec3(0.0f, 0.0f, 0.0f);
				m_diffuse = glm::vec3(0.01f, 0.01f, 0.01f);
				m_specular = glm::vec3(0.50f, 0.50f, 0.50f);
				m_shininess = 0.25f;
				break;
			case MATERIAL::CYAN_PLASTIC:
				m_ambient = glm::vec3(0.0f, 0.1, 0.06f);
				m_diffuse = glm::vec3(0.0f, 0.50980392f, 0.50980392f);
				m_specular = glm::vec3(0.50196078f, 0.50196078f, 0.50196078f);
				m_shininess = 0.25f;
				break;
			case MATERIAL::GREEN_PLASTIC:
				m_ambient = glm::vec3(0.0f, 0.0f, 0.0f);
				m_diffuse = glm::vec3(0.1f, 0.35f, 0.1f);
				m_specular = glm::vec3(0.45f, 0.55f, 0.45f);
				m_shininess = 0.25f;
				break;
			case MATERIAL::RED_PLASTIC:
				m_ambient = glm::vec3(0.0f, 0.0f, 0.0f);
				m_diffuse = glm::vec3(0.5f, 0.0f, 0.0f);
				m_specular = glm::vec3(0.7f, 0.6f, 0.6f);
				m_shininess = 0.25f;
				break;
			case MATERIAL::WHITE_PLASTIC:
				m_ambient = glm::vec3(0.0f, 0.0f, 0.0f);
				m_diffuse = glm::vec3(0.55f, 0.55f, 0.55f);
				m_specular = glm::vec3(0.70f, 0.70f, 0.70f);
				m_shininess = 0.25f;
				break;
			case MATERIAL::YELLOW_PLASTIC:
				m_ambient = glm::vec3(0.0f, 0.0f, 0.0f);
				m_diffuse = glm::vec3(0.5f, 0.5f, 0.0f);
				m_specular = glm::vec3(0.60f, 0.60f, 0.50f);
				m_shininess = 0.25f;
				break;
			case MATERIAL::BLACK_RUBBER:
				m_ambient = glm::vec3(0.02f, 0.02f, 0.02f);
				m_diffuse = glm::vec3(0.01f, 0.01f, 0.01f);
				m_specular = glm::vec3(0.4f, 0.4f, 0.4f);
				m_shininess = 0.078125f;
				break;						
			case MATERIAL::CYAN_RUBBER:
				m_ambient = glm::vec3(0.0f, 0.05f, 0.05f);
				m_diffuse = glm::vec3(0.4f, 0.5f, 0.5f);
				m_specular = glm::vec3(0.04f, 0.7f, 0.7f);
				m_shininess = 0.078125f;
				break;
			case MATERIAL::GREEN_RUBBER:
				m_ambient = glm::vec3(0.0f, 0.05f, 0.0f);
				m_diffuse = glm::vec3(0.4f, 0.5f, 0.4f);
				m_specular = glm::vec3(0.04f, 0.7f, 0.04f);
				m_shininess = 0.078125f;
				break;
			case MATERIAL::RED_RUBBER:
				m_ambient = glm::vec3(0.05f, 0.0f, 0.0f);
				m_diffuse = glm::vec3(0.5f, 0.4f, 0.4f);
				m_specular = glm::vec3(0.7f, 0.04f, 0.04f);
				m_shininess = 0.078125f;
				break;
			case MATERIAL::WHITE_RUBBER:
				m_ambient = glm::vec3(0.05f, 0.05f, 0.05f);
				m_diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
				m_specular = glm::vec3(0.7f, 0.7f, 0.7f);
				m_shininess = 0.078125f;
				break;
			case MATERIAL::YELLOW_RUBBER:
				m_ambient = glm::vec3(0.05f, 0.05f, 0.0f);
				m_diffuse = glm::vec3(0.5f, 0.5f, 0.4f);
				m_specular = glm::vec3(0.7f, 0.7f, 0.04f);
				m_shininess = 0.078125f;
				break;
			default:
				m_ambient = glm::vec3(0.0215f, 0.1745f, 0.0215f);
				m_diffuse = glm::vec3(0.07568f, 0.61424f, 0.07568f);
				m_specular = glm::vec3(0.633f, 0.727811f, 0.633f);
				m_shininess = 0.6f;
				break;
			}
		}

		glm::vec3 getColor() { return m_color; }
		void setColor(glm::vec3 color) { m_color = color; }
		void setColor(GLfloat r, GLfloat g, GLfloat b) { setColor(glm::vec3(r, g, b)); }

		glm::vec3 getAmbient() { return m_ambient; }
		void setAmbient(glm::vec3 ambient) { m_ambient = ambient; }
		void setAmbient(GLfloat x, GLfloat y, GLfloat z) { setAmbient(glm::vec3(x, y, z)); }

		glm::vec3 getDiffuse() { return m_diffuse; }
		void setDiffuse(glm::vec3 diffuse) { m_diffuse = diffuse; }
		void setDiffuse(GLfloat x, GLfloat y, GLfloat z) { setDiffuse(glm::vec3(x, y, z)); }

		glm::vec3 getSpecular() { return m_specular; }
		void setSpecular(glm::vec3 specular) { m_specular = specular; }
		void setSpecular(GLfloat x, GLfloat y, GLfloat z) { setSpecular(glm::vec3(x, y, z)); }

		GLfloat getShininess() { return m_shininess; }
		void setShininess(GLfloat shininess) { m_shininess = shininess; }

		std::shared_ptr<Shader> bind(std::shared_ptr<Shader> shader, tstring name = _T("colorMaterial")) {
			shader
				->bind(name + _T(".color"), m_color)
				->bind(name + _T(".ambient"), m_ambient)
				->bind(name + _T(".diffuse"), m_diffuse)
				->bind(name + _T(".specular"), m_specular)
				->bind(name + _T(".shininess"), m_shininess);
			return shader;
		}
	};
	class TexMaterial {
	protected:
		static const int DIFFUSE_MAX_COUNT = 3;
		static const int SPECULAR_MAX_COUNT = 3;
		static const int AMBIENT_MAX_COUNT = 3;
		static const int EMISSIVE_MAX_COUNT = 3;
		std::vector<std::shared_ptr<Texture>> m_diffuse;
		std::vector<std::shared_ptr<Texture>> m_specular;
		std::vector<std::shared_ptr<Texture>> m_reflection;
		std::vector<std::shared_ptr<Texture>> m_emission;
		float m_shininess;
	public:
		TexMaterial(float shininess = 32.0f) : m_shininess(shininess) { }
		virtual ~TexMaterial() { }
		bool addTexture(std::shared_ptr<Texture> texture) {
			bool result = true;
			switch (texture->Type()) {
			case Texture::TYPE::DIFFUSE: 
				if (m_diffuse.size() < DIFFUSE_MAX_COUNT)
					m_diffuse.push_back(texture); 
				else result = false;
				break;
			case Texture::TYPE::SPECULAR:
				if (m_specular.size() < SPECULAR_MAX_COUNT)
					m_specular.push_back(texture); 
				else result = false;
				break;
			case Texture::TYPE::AMBIENT:
				if (m_specular.size() < AMBIENT_MAX_COUNT)
					m_reflection.push_back(texture); 
				else result = false;
				break;
			case Texture::TYPE::EMISSIVE:
				if (m_specular.size() < EMISSIVE_MAX_COUNT)
					m_emission.push_back(texture);
				else result = false;
				break;
			default: result = false;
			}
			return result;
		}
		std::shared_ptr<Shader> bind(std::shared_ptr<Shader> shader, tstring name = _T("texMaterial")) {
			try {
				GLuint j = 0;
				//shader->bind("diffuseCount", m_diffuse.size());
				for (size_t i = 0; i < m_diffuse.size(); i++, j++)
					m_diffuse[i]->bind(shader, name + _T(".diffuse"), i, j);
				//shader->bind("specularCount", m_diffuse.size());
				for (size_t i = 0; i < m_specular.size(); i++, j++)
					m_specular[i]->bind(shader, name + _T(".specular"), i, j);
				//shader->bind("reflectionCount", m_diffuse.size());
				for (size_t i = 0; i < m_reflection.size(); i++, j++)
					m_reflection[i]->bind(shader, name + _T(".reflection"), i, j);
				//shader->bind("emissionCount", m_diffuse.size());
				for (size_t i = 0; i < m_emission.size(); i++, j++)
					m_emission[i]->bind(shader, name + _T(".emission"), i, j);
				shader->bind(name + _T(".shininess"), m_shininess);
			}
			catch (std::runtime_error &ex) {
				throw ex;
			}
			return shader;
		}
		void unbind() {
			for (size_t i = 0; i < m_diffuse.size(); i++) m_diffuse[i]->unbind();
			for (size_t i = 0; i < m_specular.size(); i++) m_specular[i]->unbind();
			for (size_t i = 0; i < m_reflection.size(); i++) m_reflection[i]->unbind();
			for (size_t i = 0; i < m_emission.size(); i++) m_emission[i]->unbind();
		}
	};
};
