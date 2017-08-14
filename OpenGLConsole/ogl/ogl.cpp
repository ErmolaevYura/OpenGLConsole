#pragma once
#include "stdafx.h"
#include "ogl.h"
#include <document.h>

const tstring RESOURCE_DIR = _T("./resource/");
const tstring TEXTURES_DIR = RESOURCE_DIR + _T("textures/");
const tstring SKYBOX_DIR = RESOURCE_DIR + _T("textures/skybox/");
const tstring MODELS_DIR = RESOURCE_DIR + _T("3dmodels/");
const tstring MODELS_TEXTURES_DIR = _T("/textures/");
const tstring FONTS_DIR = RESOURCE_DIR + _T("fonts/");
const tstring SHADERS_DIR = RESOURCE_DIR + _T("shaders/");

#include "ShaderManager.hpp"
#include "TextureManager.hpp"
#include "MaterialManager.hpp"
#include "ModelManager.hpp"
#include "Context.hpp"

using namespace ogl;
FlyCamera* ogl::getFlyCamera() { return Context::getInstance().getFlyCamera(); }
TextureManager* ogl::getTextureManager() { return &TextureManager::getInstance(); }
MaterialManager* ogl::getMaterialManager() { return &MaterialManager::getInstance(); }
ShaderManager* ogl::getShaderManager() { return &ShaderManager::getInstance(); }
ModelManager* ogl::getModelManager() { return &ModelManager::getInstance(); }

using namespace rapidjson;
bool ogl::loadResource(tstring filename) {
	std::ifstream file(filename);
	Document document;
	if (file.is_open()) {
		std::stringstream buffer;
		buffer << file.rdbuf();
		document.Parse(buffer.str().c_str());
		file.close();
	}
	else return false;
	/* Текстуры */
	if (!document.IsObject()) return false;
	if (document.HasMember("texture") && document["texture"].IsArray()) {
		const Value& a = document["texture"];
		tstring name, filename;
		for (auto& obj : a.GetArray()) {
			if (obj.IsObject() && obj.HasMember("name") && obj.HasMember("filename")) {
				if (obj["name"].IsString()) name = GetString(obj["name"].GetString());
				if (obj["filename"].IsString()) filename = GetString(obj["filename"].GetString());
			}
			try {
				getTextureManager()->loadTexture(name, TEXTURES_DIR + filename);
			}
			catch (const std::runtime_error &ex) {
				core::AddError(ex.what());
			}
		}
	}
	/* Скайбоксы */
	if (document.HasMember("skybox") && document["skybox"].IsArray()) {
		const Value& a = document["skybox"];
		tstring name;
		std::vector<tstring> files;
		for (auto& obj : a.GetArray()) {
			if (obj.IsObject() && obj.HasMember("name") && obj.HasMember("filenames")) {
				if (obj["name"].IsString()) name = GetString(obj["name"].GetString());
				if (obj["filenames"].IsArray())
					for (auto& fname : obj["filenames"].GetArray())
						if (fname.IsString()) files.push_back(GetString(fname.GetString()));
			}
			try {
				getTextureManager()->loadCubemap(SKYBOX_DIR, name, files);
			}
			catch (const std::runtime_error &ex) {
				core::AddError(ex.what());
			}
			files.clear();
		}
	}
	/* Материалы */
	if (document.HasMember("material") && document["material"].IsArray()) {
		const Value& a = document["material"];
		tstring name, filename, type;
		std::map<Texture::TYPE, tstring> files;
		for (auto& obj : a.GetArray()) {
			if (obj.IsObject() && obj.HasMember("name") && obj.HasMember("filenames")) {
				if (obj["name"].IsString()) name = GetString(obj["name"].GetString());
				if (obj["filenames"].IsArray())
					for (auto& fname : obj["filenames"].GetArray()) {
						if (fname.IsObject() && fname.HasMember("filename") && fname.HasMember("type")) {
							if (fname["filename"].IsString()) filename = GetString(fname["filename"].GetString());
							if (fname["type"].IsString()) type = GetString(fname["type"].GetString());
						}
						files.insert(std::pair<Texture::TYPE, tstring>(Texture::typeFromString(type), TEXTURES_DIR + filename));
					}
			}
			try {
				getMaterialManager()->loadMaterial(name, files);
			}
			catch (const std::runtime_error &ex) {
				core::AddError(ex.what());
			}
			files.clear();
		}
	}
	/* Модели */
	if (document.HasMember("model") && document["model"].IsArray()) {
		const Value& a = document["model"];
		tstring name, filename;
		for (auto& obj : a.GetArray()) {
			if (obj.IsObject() && obj.HasMember("name") && obj.HasMember("filename")) {
				if (obj["name"].IsString()) name = GetString(obj["name"].GetString());
				if (obj["filename"].IsString()) filename = GetString(obj["filename"].GetString());
			}
			try {
				getModelManager()->addModel(MODELS_DIR, name, filename);
			}
			catch (const std::runtime_error &ex) {
				core::AddError(ex.what());
			}
		}
	}
	/* Шейдры */
	if (document.HasMember("shader") && document["shader"].IsArray()) {
		const Value& a = document["shader"];
		tstring name, filename, type;
		std::vector<tstring> files;
		for (auto& obj : a.GetArray()) {
			if (obj.IsObject() && obj.HasMember("name") && obj.HasMember("filenames")) {
				if (obj["name"].IsString()) name = GetString(obj["name"].GetString());
				if (obj["filenames"].IsArray())
					for (auto& fname : obj["filenames"].GetArray()) files.push_back(GetString(fname.GetString()));
			}
			try {
				getShaderManager()->addShader(name, SHADERS_DIR, files);
			}
			catch (const std::runtime_error &ex) {
				core::AddError(ex.what());
			}
			files.clear();
		}
	}
	file.close();
	return true;
}