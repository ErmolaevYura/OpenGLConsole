#pragma once
#include "assimp.h"
#include "Mesh.hpp"

namespace ogl
{
	class Model {
	private:
		tstring m_directory;
		tstring m_filename;
		std::vector<Mesh*> m_meshes;
		inline void parsePath(tstring filepath) {
			std::size_t div = filepath.find_last_of('/');
			if (div != tstring::npos) {
				m_directory = filepath.substr(0, div);
				m_filename = filepath.substr(++div, filepath.length());
			}
			else m_filename = filepath;
		}
		inline void processNode(aiNode* node, const aiScene* scene) {
			for (GLuint i = 0; i < node->mNumMeshes; i++) {
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				try {
					m_meshes.push_back(processMesh(mesh, scene));
				}
				catch (const std::runtime_error &ex) {
					throw ex;
				}
			}
			for (GLuint i = 0; i < node->mNumChildren; i++)
				processNode(node->mChildren[i], scene);
		}
		Mesh* processMesh(aiMesh* mesh, const aiScene* scene)
		{
			std::vector<Vertex> vertices;
			std::vector<GLuint> indices;
			std::unique_ptr<TexMaterial> texMaterial;
			for (GLuint i = 0; i < mesh->mNumVertices; i++) {
				Vertex vertex;
				
				glm::vec3 vector;
				vector.x = mesh->mVertices[i].x;
				vector.y = mesh->mVertices[i].y;
				vector.z = mesh->mVertices[i].z;
				vertex.m_position = vector;

				vector.x = mesh->mVertices[i].x;
				vector.y = mesh->mVertices[i].y;
				vector.z = mesh->mVertices[i].z;
				vertex.m_normal = vector;

				if (mesh->mTextureCoords[0]) {
					glm::vec2 vec;
					vec.x = mesh->mTextureCoords[0][i].x;
					vec.y = mesh->mTextureCoords[0][i].y;
					vertex.m_texCoords = vec;
				}
				else vertex.m_texCoords = glm::vec2(0.0f, 0.0f);

				vertices.push_back(vertex);
			}
			for (GLuint i = 0; i < mesh->mNumFaces; i++) {
				aiFace face = mesh->mFaces[i];
				for (GLuint j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}
			if (mesh->mMaterialIndex >= 0) {
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
				try {
					texMaterial = loadMaterialTexures(material);
				}
				catch (const std::runtime_error &ex){
					throw ex;
				}
			}
			return new Mesh(vertices, indices, std::move(texMaterial));
		}
		std::unique_ptr<TexMaterial> loadMaterialTexures(aiMaterial* mat)
		{
			TextureManager *manager = &TextureManager::getInstance();
			aiString str;
			std::unique_ptr<TexMaterial> texMaterial = std::make_unique<TexMaterial>();
			for (GLuint i = 0; i < mat->GetTextureCount(aiTextureType_DIFFUSE); i++) {
				mat->GetTexture(aiTextureType_DIFFUSE, i, &str);
				try {
					texMaterial->addTexture(manager->loadModelTexture(
						m_directory + MODELS_TEXTURES_DIR + GetString(str.C_Str()),
						Texture::TYPE::DIFFUSE));
				}
				catch (std::runtime_error &ex) {
					throw ex;
				}
			}
			for (GLuint i = 0; i < mat->GetTextureCount(aiTextureType_SPECULAR); i++) {
				mat->GetTexture(aiTextureType_SPECULAR, i, &str);
				try {
					texMaterial->addTexture(manager->loadModelTexture(
					m_directory + MODELS_TEXTURES_DIR + GetString(str.C_Str()),
					Texture::TYPE::SPECULAR
					));
				}
				catch (std::runtime_error &ex) {
					throw ex;
				}
			}
			for (GLuint i = 0; i < mat->GetTextureCount(aiTextureType_EMISSIVE); i++) {
				mat->GetTexture(aiTextureType_EMISSIVE, i, &str);
				try {
					texMaterial->addTexture(manager->loadModelTexture(
					m_directory + MODELS_TEXTURES_DIR + GetString(str.C_Str()),
					Texture::TYPE::EMISSIVE
					));
				}
				catch (std::runtime_error &ex) {
					throw ex;
				}
			}
			for (GLuint i = 0; i < mat->GetTextureCount(aiTextureType_AMBIENT); i++) {
				mat->GetTexture(aiTextureType_AMBIENT, i, &str);
				try {
					texMaterial->addTexture(manager->loadModelTexture(
					m_directory + MODELS_TEXTURES_DIR + GetString(str.C_Str()),
					Texture::TYPE::EMISSIVE
					));
				}
				catch (std::runtime_error &ex) {
					throw ex;
				}
			}
			return std::move(texMaterial);
		}
	public:
		~Model() { m_meshes.clear(); }

		void loadModel(tstring filepath) {
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(GetC(filepath.c_str()),
				aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);
			if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
				std::string msg = "Assimp: ";
				msg.append(importer.GetErrorString());
				throw std::runtime_error(msg);
				return;
			}
			parsePath(filepath);
			processNode(scene->mRootNode, scene);
		}

		tstring GetFilePath() { return m_directory + m_filename; }
		inline void Draw(std::shared_ptr<Shader> shader) {
			try {
				for (GLuint i = 0; i < m_meshes.size(); i++)
					m_meshes[i]->Draw(shader);
			}
			catch (const std::runtime_error &ex) {
				throw ex;
			}
		}
	};
};
