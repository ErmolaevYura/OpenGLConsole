#pragma once
#include "../repository/tstring.h"
#include "Material.hpp"
#include "TextureManager.hpp"

namespace ogl {
	class MaterialManager
	{
		std::map<tstring, std::shared_ptr<TexMaterial>> m_materials;
		MaterialManager() { }
		~MaterialManager() { }
		MaterialManager(MaterialManager const&) = delete;
		MaterialManager& operator= (MaterialManager const&) = delete;
	public:
		static MaterialManager& getInstance() {
			static MaterialManager instance;
			return instance;
		}
		std::shared_ptr<TexMaterial> getMaterial(tstring name) {
			return m_materials.find(name)->second;
		}
		std::shared_ptr<TexMaterial> loadMaterial(tstring name, std::map <Texture::TYPE, tstring> files) {
			std::shared_ptr<TexMaterial> material = std::make_shared<TexMaterial>();
			try {
				for each (auto file in files)
					material->addTexture(TextureManager::getInstance().loadTexture(file.second, file.second, file.first));
			}
			catch (const std::runtime_error &ex) {
				throw ex;
			}
			m_materials.insert(std::pair<tstring, std::shared_ptr<TexMaterial>>(name, material));
			return material;
		}
		bool unloadMaterial(tstring name) {
			if (m_materials.find(name)->second.use_count() > 1) return false;
			m_materials.erase(name);
			return true;
		}
		int unloadMaterials() {
			for each (auto material in m_materials) {
				if (material.second.use_count() > 1) continue;
				m_materials.erase(material.first);
			}
			return m_materials.size();
		}
	};
};
