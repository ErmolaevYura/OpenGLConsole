#pragma once
#include "opengl.h"
#include "ArrayBuffer.hpp"
#include "BufferObject.hpp"
#include "TextureManager.hpp"
#include "Material.hpp"
#include "Shader.hpp"

namespace ogl 
{
	class Mesh {
	private:
		ArrayBuffer m_vao;
		BufferObject m_vbo, m_ebo;
		inline void setupMesh() {
			try {
				m_vao.create()->bind();
				m_vbo.create()->bind()
					->addData(&m_vertices[0], m_vertices.size() * sizeof(Vertex))
					->uploadDataToGPU(GL_STATIC_DRAW);
				m_ebo.create()->bind(GL_ELEMENT_ARRAY_BUFFER)
					->addData(&m_indices[0], m_indices.size() * sizeof(GLuint))
					->uploadDataToGPU(GL_STATIC_DRAW);
			}
			catch (std::runtime_error &ex) {
				throw ex;
			}
			m_vao.addAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
			m_vao.addAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, m_normal));
			m_vao.addAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, m_texCoords));

			m_ebo.unbind();
			m_vbo.unbind();
			m_vao.unbind();
		}
		void bind(std::shared_ptr<Shader> shader) {
			try {
				m_vao.bind();
				m_ebo.bind(GL_ELEMENT_ARRAY_BUFFER);
				m_material->bind(shader);
			}
			catch (std::runtime_error &ex) {
				throw ex;
			}
		}
		void unbind() {
			m_material->unbind();
			m_ebo.unbind();
			m_vao.unbind();
		}
	public: 
		std::vector<Vertex> m_vertices;
		std::vector<GLuint> m_indices;
		std::unique_ptr<TexMaterial> m_material;
		Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::unique_ptr<TexMaterial> material)
			: m_vertices(vertices), m_indices(indices) {
			m_material = std::move(material);
			try {
				setupMesh();
			}
			catch (const std::exception &ex) {
				ConsoleLogger::AddError(ex.what());
			}
		}
		~Mesh() {
			m_ebo.release();
			m_vbo.release();
			m_vao.release();
		}
		inline void Draw(std::shared_ptr<Shader> shader) {
			try {
				bind(shader);
			}
			catch (const std::runtime_error &ex){
				throw ex;
			}
			glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, (void*)0);
			unbind();
		}
	};
};
