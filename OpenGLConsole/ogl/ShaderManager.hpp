#pragma once
#include <map>
#include <memory>
#include <vector>
#include "Shader.hpp"

namespace ogl
{
	class ShaderManager
	{
		std::map<tstring, std::shared_ptr<Shader>> m_shaders;
		ShaderManager() { }
		~ShaderManager() { }
		ShaderManager(ShaderManager const&) = delete;
		ShaderManager& operator= (ShaderManager const&) = delete;
	public:
		static ShaderManager& getInstance() {
			static ShaderManager instance;
			return instance;
		}
		std::shared_ptr<Shader> getShader(tstring name) { return m_shaders.find(name)->second; }
		GLuint getId(tstring name) { return m_shaders.find(name)->second.get()->get(); }
		std::shared_ptr<Shader> getShader(GLuint id) {
			for each (auto shader in m_shaders)
				if (shader.second->get() == id)
					return shader.second;
			return nullptr;
		}
		GLuint addShader(tstring name, tstring directory, std::vector<tstring> filepaths) {
			for each (auto nameShader in m_shaders)
				if (!name.compare(nameShader.first))
					return nameShader.second->get();

			auto shader = std::make_shared<Shader>(Shader());
			try {
				shader->create();
				for each(auto filepath in filepaths) shader->attach(directory + filepath);
				shader->link();
			}
			catch (const std::runtime_error &ex) {
				shader->detach();
				throw ex;
			}
			return addShader(name, shader);
		}
		GLuint addShader(tstring name, std::shared_ptr<Shader> shader) {
			for each (auto nameShader in m_shaders)
				if (!name.compare(nameShader.first)) 
					return nameShader.second->get();
			
			m_shaders.insert(std::pair<tstring, std::shared_ptr<Shader>>(name, shader));
			return shader->get();
		}
		void unloadShader(tstring name) {
			if (m_shaders.find(name)->second.use_count() > 1) return;
			GLuint id = m_shaders.find(name)->second->get();
			m_shaders.erase(name);
			glDeleteProgram(id);
		}
		void unloadShaders() {
			for each (auto shader in m_shaders) {
				GLuint id = shader.second->get();
				if (shader.second.use_count() > 1) continue;
				m_shaders.erase(shader.first);
				glDeleteProgram(id);
			}
		}
	};
};
