#pragma once
#include <map>
#include <memory>
#include <vector>
#include "opengl.h"

const extern tstring RESOURCE_DIR;
const extern tstring TEXTURES_DIR;
const extern tstring SKYBOX_DIR;
const extern tstring MODELS_DIR;
const extern tstring MODELS_TEXTURES_DIR;
const extern tstring FONTS_DIR;
const extern tstring SHADERS_DIR;

namespace ogl {
	enum class GlassType : uint8_t {
		AIR, WATER, ICE, GLASS, DIAMOND
	};
	static GLfloat getGlass(GlassType type) {
		switch (type) {
		case GlassType::AIR: return 1.0f; 
		case GlassType::WATER: return 1.33f;
		case GlassType::ICE: return 1.309f;
		case GlassType::GLASS: return 1.52f;
		case GlassType::DIAMOND: return 2.42f;
		default: return 1.0f;
		}
	}

	class FlyCamera;
	class TextureManager;
	class MaterialManager;
	class ShaderManager;
	class ModelManager;
	FlyCamera* getFlyCamera();
	TextureManager* getTextureManager();
	MaterialManager* getMaterialManager();
	ShaderManager* getShaderManager();
	ModelManager* getModelManager();
	/*
	* Загрузка ресурсов из json
	*/
	bool loadResource(tstring filename);
};
