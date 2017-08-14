#pragma once
#include "Model.hpp"

namespace ogl {
	class ModelManager
	{
		std::map<tstring, std::shared_ptr<Model>> m_models;
		ModelManager() { }
		~ModelManager() { }
		ModelManager(ModelManager const&) = delete;
		ModelManager& operator= (ModelManager const&) = delete;
		inline tstring parsePath(tstring filepath) {
			std::size_t div = filepath.find_last_of('/');
			if (div != tstring::npos)
				return filepath.substr(0, div);
			return filepath;
		}
	public:
		static ModelManager& getInstance() {
			static ModelManager instance;
			return instance;
		}
		std::shared_ptr<Model> getModel(tstring name) { return m_models.find(name)->second; }
		std::shared_ptr<Model> addModel(tstring directory, tstring name, tstring filepath) {
			for each (auto nameModel in m_models)
				if (!name.compare(nameModel.first)) return nameModel.second;

			auto model = std::make_shared<Model>(Model());
			try {
				model->loadModel(directory + name + _T("/") + filepath);
			}
			catch (std::runtime_error &ex) {
				throw ex;
			}
			return addModel(name, model);
		}
		std::shared_ptr<Model> addModel(tstring name, std::shared_ptr<Model> model) {
			for each (auto nameModel in m_models)
				if (!name.compare(nameModel.first)) return nameModel.second;

			m_models.insert(std::pair<tstring, std::shared_ptr<Model>>(name, model));
			return model;
		}
		void unloadModel(tstring name) {
			if (m_models.find(name)->second.use_count() > 1) return;
			m_models.erase(name);
		}
		void unloadModels() {
			for each (auto model in m_models) {
				if (model.second.use_count() > 1) continue;
				m_models.erase(model.first);
			}
		}
	};
}