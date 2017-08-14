#pragma once

#include <list>
#include <exception>
#include "Exception.h"
#include "../repository/tstring.h"
#include "opengl.h"
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>

namespace ogl {
	enum class ShaderType {
		Vertex, Fragment, Geometry, TessConrol, TessEvalution
	};
	class ShaderRaii : private boost::noncopyable {
	private: 
		GLuint m_id = 0;
		GLenum MapShaderType(ShaderType shaderType) const {
			switch (shaderType) {
				case ShaderType::Vertex: return GL_VERTEX_SHADER;
				case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
				case ShaderType::Geometry: return GL_GEOMETRY_SHADER;
				case ShaderType::TessConrol: return GL_TESS_CONTROL_SHADER;
				case ShaderType::TessEvalution: return GL_TESS_EVALUATION_SHADER;
			}
			throw std::logic_error("Unknow shader type");
		}
	public:
		ShaderRaii(ShaderType shaderType) {
			m_id = glCreateShader(MapShaderType(shaderType));
		}
		~ShaderRaii() { glDeleteShader(m_id); }
		operator GLuint() const { return m_id; }
		GLuint Release() { 
			const GLuint id = m_id; 
			m_id = 0; 
			return id; 
		}
	};
	class ShaderProgram : private boost::noncopyable {
	private:
		GLuint m_programId = 0;
		std::vector<GLuint> m_shaders;
		void freeShader() {
			for (GLuint shaderId : m_shaders) {
				glDetachShader(m_programId, shaderId);
				glDeleteShader(shaderId);
			}
			m_shaders.clear();
		}
	public: 
		struct fixed_pipeline_t {};
		ShaderProgram() : m_programId(glCreateProgram()) {}
		ShaderProgram(fixed_pipeline_t pipeline) : m_programId(0) {}
		~ShaderProgram() {
			freeShader(); 
			glDeleteProgram(m_programId);
		}

		void compileShader(const std::string &source, ShaderType type) {
			const char *sourceLines[] = { source.c_str() };
			const GLint sourceLengths[] = { GLint(source.length()) };
			ShaderRaii shader(type);
			glShaderSource(shader, 1, sourceLines, sourceLengths);
			glCompileShader(shader);
			GLint compileStatus = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
			if (compileStatus == GL_FALSE) {
				const auto log = GetShaderInfoLog(shader);
				throw std::runtime_error("Shader compiling failed: " + log);
			}
			m_shaders.emplace_back(shader.Release());
			glAttachShader(m_programId, m_shaders.back());
		}
		void link() {
			glLinkProgram(m_programId);
			GLint linkStatus = 0;
			glGetProgramiv(m_programId, GL_LINK_STATUS, &linkStatus);
			if (linkStatus == GL_FALSE) {
				const auto log = GetProgramInfoLog(m_programId);
				throw std::runtime_error("Program linked failed: " + log);
			}
			freeShader();
		}

		boost::optional<tstring> validate() const {
			glValidateProgram(m_programId);
			GLint status = 0;
			glGetProgramiv(m_programId, GL_VALIDATE_STATUS, &status);
			if (status == GL_FALSE) {
				const auto log = GetProgramInfoLog(m_programId);
				return log;
			}
			return boost::none;
		}
		void use() const { glUseProgram(m_programId);  }
	};
	class Shader
	{
		GLuint m_programId = 0;
		GLint m_status = 0;
		GLint m_length = 0;
		std::list<GLuint> m_shaders;
	public:
		GLuint create() {
			m_programId = glCreateProgram();
			if (m_programId == 0) throw ogl::ResourceNotAllocate(GL_PROGRAM);
		}

		GLuint get() { return m_programId; }
		
		Shader* activate() {
			if (m_programId == 0) throw ogl::ResourceNotAllocate(GL_PROGRAM);
			glUseProgram(m_programId); 
			return this;
		}
		Shader* attach(const tstring &filepath) {
			if (m_programId == 0) throw ogl::ResourceNotAllocate(GL_PROGRAM);
			std::ifstream fd(filepath.c_str());
			if (!fd.is_open()) throw ogl::FileNotFound(filepath);
			auto src = std::string(std::istreambuf_iterator<char>(fd),
				std::istreambuf_iterator<char>());

			const char* source = src.c_str();
			auto shader = create(filepath);
			glShaderSource(shader, 1, &source, nullptr);
			glCompileShader(shader);
			glGetShaderiv(shader, GL_COMPILE_STATUS, &m_status);

			if (!m_status) {
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &m_length);
				std::unique_ptr<char[]> buffer(new char[m_length]);
				glGetShaderInfoLog(shader, m_length, nullptr, buffer.get());
				tstring msg = filepath + _T("\n") + GetString(buffer.get());
				throw std::runtime_error(GetC(msg.c_str()));
			}

			glAttachShader(m_programId, shader);
			m_shaders.push_back(shader);
			glDeleteShader(shader);
			return this;
		}
		Shader* attach(const tstring &source, GLenum type) {
			if (m_programId == 0)  throw ogl::ResourceNotAllocate(GL_PROGRAM);
			const GLchar *shaderSource = GetC(source.data());
			auto shader = glCreateShader(type);
			glShaderSource(shader, 1, &shaderSource, nullptr);
			glCompileShader(shader);
			glGetShaderiv(shader, GL_COMPILE_STATUS, &m_status);

			if (!m_status) {
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &m_length);
				std::unique_ptr<char[]> buffer(new char[m_length]);
				glGetShaderInfoLog(shader, m_length, nullptr, buffer.get());
				throw std::runtime_error(buffer.get());
			}

			glAttachShader(m_programId, shader);
			m_shaders.push_back(shader);
			glDeleteShader(shader);
			return this;
		}
		void detach(GLuint shader) {
			if (m_programId == 0) glDetachShader(m_programId, shader);
		}
		void detach() {
			if (m_programId == 0)
				for each (auto shader in m_shaders)
					glDetachShader(m_programId, shader);
		}
		Shader* link() {
			if (m_programId == 0)  throw ogl::ResourceNotAllocate(GL_PROGRAM);
			glLinkProgram(m_programId);
			glGetProgramiv(m_programId, GL_LINK_STATUS, &m_status);
			if (!m_status) {
				glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &m_length);
				std::unique_ptr<char[]> buffer(new char[m_length]);
				glGetProgramInfoLog(m_programId, m_length, nullptr, buffer.get());
				throw std::runtime_error(buffer.get());
			}
			/*
			glValidateProgram(m_programId);
			glGetProgramiv(m_programId, GL_VALIDATE_STATUS, &m_status);
			if (!m_status) {
				glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &m_length);
				std::unique_ptr<char[]> buffer(new char[m_length]);
				glGetProgramInfoLog(m_programId, m_length, nullptr, buffer.get());
				throw std::runtime_error(buffer.get());
			}
			*/
			return this;
		}

		static void bind(unsigned int location, float value) {
			glUniform1f(location, value);
		}
		static void bind(unsigned int location, GLuint value) {
			glUniform1i(location, value);
		}
		static void bind(unsigned int location, GLint value) {
			glUniform1i(location, value);
		}
		static void bind(unsigned int location, const glm::vec2 &vector) {
			glUniform2f(location, vector.x, vector.y);
		}
		static void bind(unsigned int location, const glm::vec3 vector) {
			glUniform3f(location, vector.x, vector.y, vector.z);
		} 
		static void bind(unsigned int location, const glm::vec4 vector) {
			glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
		}
		static void bind(unsigned int location, const glm::mat4 &matrix) {
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
		}
		template<typename T> Shader* bind(const tstring &name, T&& value) {
			int location = glGetUniformLocation(m_programId, GetC(name.c_str()));
			if (location == -1) core::AddError(_T("Missing Uniform: ") + name);
			else bind(location, std::forward<T>(value));
			return this;
		}
		void clearShaders() {
			for each(auto shader in m_shaders) {
				glDeleteShader(shader);
				m_shaders.remove(shader);
			}
		}
		void releas() {
			detach();
			clearShaders();
			glDeleteProgram(m_programId);
			m_programId = 0;
		}
	public:
		static GLuint create(const tstring & filepath) {
			auto index = filepath.rfind(_T("."));
			auto ext = filepath.substr(index + 1);
			if (ext == _T("comp")) return glCreateShader(GL_COMPUTE_SHADER);
			else if (ext == _T("frag")) return glCreateShader(GL_FRAGMENT_SHADER);
			else if (ext == _T("geom")) return glCreateShader(GL_GEOMETRY_SHADER);
			else if (ext == _T("vert")) return glCreateShader(GL_VERTEX_SHADER);
			else if (ext == _T("tcon")) return glCreateShader(GL_TESS_CONTROL_SHADER);
			else if (ext == _T("teva")) return glCreateShader(GL_TESS_EVALUATION_SHADER);
			else return false;
		}
	};
};
