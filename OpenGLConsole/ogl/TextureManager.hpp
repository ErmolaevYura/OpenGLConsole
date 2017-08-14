#pragma once
#include <map>
#include <memory>
#include "Texture.hpp"

namespace ogl 
{	
	class TextureManager
	{
		typedef
#ifdef SOIL
			TextureSOIL
#elif FREEIMAGE
			TextureFreeImage
#else 
			TextureOGL
#endif
			 Texture;
		std::map<tstring, std::shared_ptr<Texture>> m_textures;
		TextureManager() { }
		~TextureManager() { }
		TextureManager(TextureManager const&) = delete;
		TextureManager& operator= (TextureManager const&) = delete;
		std::pair<tstring, std::shared_ptr<Texture>> getPair(GLuint id) {
			for each (auto texture in m_textures)
				if (texture.second->Id() == id)
					return texture;
			return std::pair<tstring, std::shared_ptr<Texture>>();
		}
	public:
		static TextureManager& getInstance() {
			static TextureManager instance;
			return instance;
		}
		std::shared_ptr<Texture> getTexture(GLuint id) { return getPair(id).second; }
		std::shared_ptr<Texture> getTexture(tstring name) { return m_textures.find(name)->second; }
		GLuint getId(tstring name) { return getTexture(name)->Id(); }
		Texture::TYPE getType(tstring name)	{ return getTexture(name)->Type(); }
		Texture::TYPE getType(GLuint id) { return getPair(id).second->Type(); }
		tstring getName(GLuint id) { return getPair(id).first; }

		std::shared_ptr<Texture> loadModelTexture(tstring file, Texture::TYPE type = Texture::TYPE::NONE) {
			for each (auto texture in m_textures)
				if (!texture.first.compare(file)) 
					return getTexture(file);
			auto texture = std::make_shared<Texture>(Texture());
			try {
				texture->loadTexture(file, type);
			}
			catch (const std::runtime_error &ex) {
				throw ex;
			}
			m_textures.insert(std::pair<tstring, std::shared_ptr<Texture>>(file, texture));
			return texture;
		}
		std::shared_ptr<Texture> loadTexture(tstring name, tstring filepath, Texture::TYPE type = Texture::TYPE::NONE) {
			for each (auto texture in m_textures)
				if (!texture.first.compare(name)) 
					return getTexture(name);
			auto texture = std::make_shared<Texture>(Texture());
			try {
				texture->loadTexture(filepath, type);
			}
			catch (const std::runtime_error &ex) {
				throw ex;
			}
			m_textures.insert(std::pair<tstring, std::shared_ptr<Texture>>(name, texture));
			return texture;
		}
		std::shared_ptr<Texture> loadCubemap(tstring directory, tstring name, std::vector<tstring> files, bool mipMaps = true) {
			for each (auto texture in m_textures)
				if (!texture.first.compare(name)) 
					return getTexture(name);
			auto texture = std::make_shared<Texture>(Texture());
#ifndef SOIL
			throw std::runtime_error("Only SOIL Texture has loadCubemap function!!")
			return 0;
#endif
			texture->loadCubemap(directory, name, files, mipMaps);
			m_textures.insert(std::pair<tstring, std::shared_ptr<Texture>>(name, texture));
			return texture;
		}
		bool unloadTexture(GLuint id) {
			auto texture = getPair(id);
			if (texture.second.use_count() > 1) return false;
			m_textures.erase(texture.first);
			glDeleteTextures(1, &id);
			return true;
		}
		int unloadTextures() {
			GLuint id;
			for each (auto texture in m_textures) {
				id = texture.second->Id();
				if (texture.second.use_count() > 1) continue;
				m_textures.erase(texture.first);
				glDeleteTextures(1, &id);
			}
			return m_textures.size();
		}
	};
};