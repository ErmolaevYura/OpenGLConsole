#pragma once
#include <array>
#include "ogl/Transformation.hpp"

#include "ArrayBuffer.hpp"
#include "BufferObject.hpp"

#include "TextureManager.hpp"
#include "Animation.hpp"
#include "Shader.hpp"

namespace ogl {
	class Primitives {
	protected:
		static const int countVert = 3;
		static const int sizeVert = 3;
		static const int sizeTex = 2;

		static std::array<GLfloat, 8 * countVert>  m_vertices;
		
		ArrayBuffer m_vao;		///< Vertex Array Buffer
		BufferObject m_vbo;		///< Vertex Buffer Object
		Transformation3D m_transformation;
		virtual void setupMesh() = 0;
		inline void bind() { 
			try {
				m_vao.bind();
			}
			catch (std::runtime_error &ex) {
				throw ex;
			}
		}
		inline void unbind() { m_vao.unbind(); }
		void calc(Shader *shader) {
			glm::mat4 model = m_transformation.toMat4();
			if (Animation != nullptr)
				model = Animation->Next(model);
			shader->bind("model", model);
		}
		void calcSkybox(Shader *shader) {
			glm::mat4 model;
			model = glm::scale(model, m_transformation.Scale());
			if (Animation != nullptr)
				model = Animation->Next(model);
			shader->bind("model", model);
		}
		virtual void draw() = 0;
		virtual void drawPath() = 0;
	public:
		Primitives() { }
		Primitives(const glm::vec3 &scale = glm::vec3(1.0f),
			const glm::vec3 &position = glm::vec3(0.0f),
			GLfloat angle = 0.0f, const glm::vec3 &rotate = glm::vec3(0.0f))
			: m_transformation(scale, angle, rotate, position) { }
		Primitives(const Primitives& primitives) {
			m_transformation = primitives.m_transformation;
			m_vao = primitives.m_vao;
			m_vbo = primitives.m_vbo;
			if (primitives.Animation != nullptr)
				Animation = std::make_unique<ogl::Animation>(primitives.Animation.get());
		}
		virtual ~Primitives() {
			m_vbo.release();
			m_vao.release();
		}

		inline glm::vec3 Scale() { return m_transformation.Scale(); }
		inline void Scale(const glm::vec3 &scale) { m_transformation.Scale(scale); }
		inline void Scale(GLfloat scale) { m_transformation.Scale(glm::vec3(scale)); }

		inline glm::vec3 Position() { return m_transformation.Position(); }
		inline void Position(const glm::vec3 &position) {m_transformation.Position(position); }

		inline glm::quat Rotate() { return m_transformation.Orientation(); }
		inline glm::vec3 RotateVec3() { return m_transformation.OrientationVec3(); }
		inline void Rotate(float angle, const glm::vec3 &normal) { m_transformation.OrientationVec3(angle, normal); }

		void Draw(std::shared_ptr<Shader> shader) {
			try {
				setupMesh();
				bind();
				calc(shader.get());
				draw();
			}
			catch (const std::runtime_error &ex) {
				throw ex;
			}
			unbind();
		}
		void DrawSkybox(std::shared_ptr<Shader> shader, GLuint texture) {
			try {
				setupMesh();
				bind();
			}
			catch (const std::runtime_error &ex) {
				throw ex;
			}
			GLint deptFunc;
			glGetIntegerv(GL_DEPTH_FUNC, &deptFunc);
			glDepthFunc(GL_LEQUAL);
			//glDepthMask(GL_FALSE);
			//glDisable(GL_BLEND);
			calcSkybox(shader.get());
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(glGetUniformLocation(shader->get(), "skybox"), 0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
			try {
				draw();
			}
			catch (std::runtime_error &ex) {
				throw ex;
			}
			glDepthFunc(deptFunc);
			//glDepthMask(GL_TRUE);
			//glEnable(GL_BLEND);
			unbind();
		}
		void DrawMirror(std::shared_ptr<Shader> shader, GLuint texture) {
			try {
				setupMesh();
				bind();
				calc(shader.get());
				TextureManager::getInstance().getTexture(texture).get()->bind(shader, "tex");
				draw();
			}
			catch (std::runtime_error &ex) {
				throw ex;
			}
			unbind();
		}
		void DrawGlass(std::shared_ptr<Shader> shader, GLuint texture, GLfloat ref = 1.0f) {
			try {
				setupMesh();
				bind();
				calc(shader.get());
				TextureManager::getInstance().getTexture(texture).get()->bind(shader, "tex");
				shader->bind(_T("ref"), ref);
				draw();
			}
			catch (std::runtime_error &ex) {
				throw ex;
			}
			unbind();
		}
		void DrawPath(std::shared_ptr<Shader> shader) {
			try {
				setupMesh();
				bind();
				calc(shader.get());
				drawPath();
			}
			catch (const std::runtime_error &ex) {
				throw ex;
			}
			unbind();
		}
	public:
		std::unique_ptr<ogl::Animation> Animation = nullptr;
	};

	class Triangle : public Primitives {
		static const int countVert = 3;
		static std::array<GLfloat, 8 * countVert> m_vertices;
		virtual void setupMesh() {
			if (!m_vao.get()) {
				try {
					m_vao.create()->bind();
					m_vbo.create()->bind();
				}
				catch (std::runtime_error &ex) {
					throw ex;
				}
				m_vbo.addData(&m_vertices, sizeof(m_vertices));
				m_vbo.uploadDataToGPU(GL_STATIC_DRAW);

				m_vao.addAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
				m_vao.addAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, m_normal));
				m_vao.addAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, m_texCoords));

				m_vbo.unbind();
				m_vao.unbind();
			}
		}
		virtual void draw() { glDrawArrays(GL_TRIANGLES, 0, countVert); }
		virtual void drawPath() { glDrawArrays(GL_PATCHES, 0, countVert); }
	public:
		Triangle(const glm::vec3 &scale = glm::vec3(1.0f), 
			const glm::vec3 &position = glm::vec3(0.0f), 
			float angle = 0.0f, const glm::vec3 &normal = glm::vec3(0.0f))
			: Primitives(scale, position, angle, normal) { }
		Triangle(const Triangle& triangle) : Primitives(triangle) { }
		~Triangle() { }
	};
	std::array<GLfloat, 8 * Triangle::countVert> Triangle::m_vertices = {
		-0.5f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
		0.5f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
		0.0f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,	0.5f, 1.0f
	};

	class Quad : public Primitives {
		static const int countVert = 4;
		static std::array<GLfloat, 8 * countVert> m_vertices;
		virtual void setupMesh() {
			if (!m_vao.get()) {
				try {
					m_vao.create()->bind();
					m_vbo.create()->bind();
				}
				catch (std::runtime_error &ex) {
					throw ex;
				}
				m_vbo.addData(&m_vertices, sizeof(m_vertices));
				m_vbo.uploadDataToGPU(GL_STATIC_DRAW);

				m_vao.addAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
				m_vao.addAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, m_normal));
				m_vao.addAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, m_texCoords));

				m_vbo.unbind();
				m_vao.unbind();
			}
		}
		virtual void draw() { glDrawArrays(GL_TRIANGLE_FAN, 0, countVert); }
		virtual void drawPath() { glDrawArrays(GL_PATCHES, 0, countVert); }
	public:
		Quad(const glm::vec3 &scale = glm::vec3(1.0f),
			const glm::vec3 &position = glm::vec3(0.0f),
			GLfloat angle = 0.0f, const glm::vec3 &normal = glm::vec3(0.0f))
			: Primitives(scale, position, angle, normal) { }
		Quad(const Quad& triangle) : Primitives(triangle) { }
		~Quad() { }
	};
	std::array<GLfloat, 8 * Quad::countVert> Quad::m_vertices = {
		-0.5f, -0.5f,  0.0f,  0.0f,  0.0f, 1.0f,	0.0f, 0.0f,
		0.5f, -0.5f,  0.0f,  0.0f,  0.0f, 1.0f,		1.0f, 0.0f,
		0.5f,  0.5f,  0.0f,  0.0f,  0.0f, 1.0f,		1.0f, 1.0f,
		-0.5f,  0.5f,  0.0f,  0.0f,  0.0f, 1.0f,	0.0f, 1.0f
	};

	class Cube : public Primitives {
		static const int countVert = 36;
		static std::array<GLfloat, 8 * countVert>  m_vertices;	

		virtual void setupMesh() {
			if (!m_vao.get()) {
				try {
					m_vao.create()->bind();
					m_vbo.create()->bind();
				}
				catch (std::runtime_error &ex) {
					throw ex;
				}
				m_vbo.addData(&m_vertices, sizeof(m_vertices));
				m_vbo.uploadDataToGPU(GL_STATIC_DRAW);

				m_vao.addAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
				m_vao.addAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, m_normal));
				m_vao.addAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, m_texCoords));

				m_vbo.unbind();
				m_vao.unbind();
			}
		}
		virtual void draw() { glDrawArrays(GL_TRIANGLES, 0, countVert); }
		virtual void drawPath() { glDrawArrays(GL_PATCHES, 0, countVert); }
	public:
		Cube(const glm::vec3 &scale = glm::vec3(1.0f),
			const glm::vec3 &position = glm::vec3(0.0f), 
			GLfloat angle = 0.0f, const glm::vec3 &normal = glm::vec3(0.0f))
			: Primitives(scale, position, angle, normal) { }
		Cube(const Cube& cube) : Primitives(cube) { }
		~Cube() { }
	};
	std::array<GLfloat, 8 * Cube::countVert> Cube::m_vertices = {
		-.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	0.0f, 0.0f,

		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,		1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,	0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,		1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,	0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,		1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,	0.0f, 0.0f,

		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,	1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,	1.0f, 0.0f,

		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,	0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,	0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,	0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,	1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,	0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,	0.0f, 1.0f
	};
	
	class Pyramide : public Primitives {
		static const int countVert = 18;
		static std::array<GLfloat, 8 * countVert>  m_vertices;

		virtual void setupMesh() {
			if (!m_vao.get()) {
				try {
					m_vao.create()->bind();
					m_vbo.create()->bind();
				}
				catch (std::runtime_error &ex) {
					throw ex;
				}

				m_vbo.addData(&m_vertices, sizeof(m_vertices));
				m_vbo.uploadDataToGPU(GL_STATIC_DRAW);

				m_vao.addAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
				m_vao.addAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, m_normal));
				m_vao.addAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, m_texCoords));

				m_vbo.unbind();
				m_vao.unbind();
			}
		}
		virtual void draw() { glDrawArrays(GL_TRIANGLES, 0, countVert); }
		virtual void drawPath() { glDrawArrays(GL_PATCHES, 0, countVert); }
	public:
		Pyramide(const glm::vec3 &scale = glm::vec3(10.0f),
			const glm::vec3 &position = glm::vec3(0.0f), 
			GLfloat angle = 0.0f, glm::vec3 &normal = glm::vec3(0.0f))
			: Primitives(scale, position, angle, normal) { }
		Pyramide(const Pyramide& pyramide) : Primitives(pyramide) { }
		~Pyramide() { }
	};
	std::array<GLfloat, 8 * Pyramide::countVert>  Pyramide::m_vertices = {
		0.0f, 0.5f, 0.0f,		-1.0f, 1.0f, 1.0f,	0.5f, 1.0f,
		-0.5f, -0.5f, 0.5f,		-1.0f, 1.0f, 1.0f,	0.0f, 0.0f,
		0.5f, -0.5f, 0.5f,		-1.0f, 1.0f, 1.0f,	1.0f, 1.0f,
			
		0.0f, 0.5f, 0.0f,		1.0f, 1.0f, 0.0f,	0.5f, 1.0f,	
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f, 0.0f,	0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	1.0f, 1.0f, 0.0f,	1.0f, 1.0f,

		0.0f, 0.5f, 0.0f,		0.0f, 1.0f, -1.0f,	0.5f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f, -1.0f,	0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,		0.0f, 1.0f, -1.0f,	1.0f, 1.0f,

		0.0f, 0.5f, 0.0f,		0.0f, 1.0f, 1.0f,	0.5f, 1.0f,
		0.5f, -0.5f, 0.5f,		0.0f, 1.0f, 1.0f,	0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		0.0f, 1.0f, 1.0f,	1.0f, 1.0f,

		-0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,	1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,	0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,	0.0f, 1.0f,

		0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,	1.0f, 1.0f,
		-0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,	1.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,	0.0f, 1.0f,

	};
	
	class Plain :public Primitives {
		static const int countVert = 6;
		static std::array<GLfloat, 8 * countVert>  m_vertices;

		virtual void setupMesh() {
			if (!m_vao.get()) {
				try {
					m_vao.create()->bind();
					m_vbo.create()->bind();
				}
				catch (std::runtime_error &ex) {
					throw ex;
				}
				m_vbo.addData(&m_vertices, sizeof(m_vertices));
				m_vbo.uploadDataToGPU(GL_STATIC_DRAW);

				m_vao.addAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
				m_vao.addAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, m_normal));
				m_vao.addAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, m_texCoords));

				m_vbo.unbind();
				m_vao.unbind();
			}
		}
		virtual void draw() { glDrawArrays(GL_TRIANGLES, 0, countVert); }
		virtual void drawPath() { glDrawArrays(GL_PATCHES, 0, countVert); }
	public:
		Plain(const glm::vec3 scale = glm::vec3(1.0f), 
			const glm::vec3 &position = glm::vec3(0.0f), 
			GLfloat angle = 0.0f, const glm::vec3 &normal = glm::vec3(0.0f))
			: Primitives(scale, position, angle, normal) { }
		Plain(const Plain& plain) : Primitives(plain) { }
		~Plain() { }
	};
	std::array<GLfloat, 8 * Plain::countVert>  Plain::m_vertices = {
		-1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f,
		1.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,	0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,	1.0f, 1.0f,

		-1.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f,
		1.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,	0.0f, 1.0f,
		-1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f,
	};

	class Tor : public Primitives {
		float m_radius, m_tubeRadius;
		int m_SubDivAround, m_subDivTibe;
		int m_facesCount = 0;
		virtual void setupMesh() {
			if (!m_vao.get()) {
				try {
					m_vao.create()->bind();
					m_vbo.create()->bind();
				}
				catch (std::runtime_error &ex) {
					throw ex;
				}
				m_facesCount = generateTorus(m_radius, m_tubeRadius, m_SubDivAround, m_subDivTibe);
				m_vbo.uploadDataToGPU(GL_STATIC_DRAW);

				m_vao.addAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
				m_vao.addAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, m_normal));
				m_vao.addAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, m_texCoords));

				m_vbo.unbind();
				m_vao.unbind();
			}
		}
		virtual void draw() { glDrawArrays(GL_TRIANGLES, 0, m_facesCount * 3); }
		virtual void drawPath() { glDrawArrays(GL_PATCHES, 0, m_facesCount * 3); }
		int generateTorus(float fRadius, float fTubeRadius, int iSubDivAround, int iSubDivTube) {
			float fAddAngleAround = 360.0f / (float)iSubDivAround;
			float fAddAngleTube = 360.0f / (float)iSubDivTube;
			float fCurAngleAround = 0.0f;
			int iStepsAround = 1;
			const float PI = float(atan(1.0)*4.0);
			int iFacesAdded = 0;
			while (iStepsAround <= iSubDivAround) {
				float fSineAround = sin(fCurAngleAround / 180.0f*PI);
				float fCosineAround = cos(fCurAngleAround / 180.0f*PI);
				glm::vec3 vDir1(fSineAround, fCosineAround, 0.0f);
				float fNextAngleAround = fCurAngleAround + fAddAngleAround;
				float fNextSineAround = sin(fNextAngleAround / 180.0f*PI);
				float fNextCosineAround = cos(fNextAngleAround / 180.0f*PI);
				glm::vec3 vDir2(fNextSineAround, fNextCosineAround, 0.0f);
				float fCurAngleTube = 0.0f;
				int iStepsTube = 1;
				while (iStepsTube <= iSubDivTube) {
					float fSineTube = sin(fCurAngleTube / 180.0f*PI);
					float fCosineTube = cos(fCurAngleTube / 180.0f*PI);
					float fNextAngleTube = fCurAngleTube + fAddAngleTube;
					float fNextSineTube = sin(fNextAngleTube / 180.0f*PI);
					float fNextCosineTube = cos(fNextAngleTube / 180.0f*PI);
					glm::vec3 vMid1 = vDir1*(fRadius - fTubeRadius / 2), vMid2 = vDir2*(fRadius - fTubeRadius / 2);
					glm::vec3 vQuadPoints[] = {
						vMid1 + glm::vec3(0.0f, 0.0f, -fNextSineTube*fTubeRadius) + vDir1*fNextCosineTube*fTubeRadius,
						vMid1 + glm::vec3(0.0f, 0.0f, -fSineTube*fTubeRadius) + vDir1*fCosineTube*fTubeRadius,
						vMid2 + glm::vec3(0.0f, 0.0f, -fSineTube*fTubeRadius) + vDir2*fCosineTube*fTubeRadius,
						vMid2 + glm::vec3(0.0f, 0.0f, -fNextSineTube*fTubeRadius) + vDir2*fNextCosineTube*fTubeRadius
					};
					glm::vec3 vNormals[] = {
						glm::vec3(0.0f, 0.0f, -fNextSineTube) + vDir1*fNextCosineTube,
						glm::vec3(0.0f, 0.0f, -fSineTube) + vDir1*fCosineTube,
						glm::vec3(0.0f, 0.0f, -fSineTube) + vDir2*fCosineTube,
						glm::vec3(0.0f, 0.0f, -fNextSineTube) + vDir2*fNextCosineTube
					};
					glm::vec2 vTexCoords[] = {
						glm::vec2(fCurAngleAround / 360.0f, fNextAngleTube / 360.0f),
						glm::vec2(fCurAngleAround / 360.0f, fCurAngleTube / 360.0f),
						glm::vec2(fNextAngleAround / 360.0f, fCurAngleTube / 360.0f),
						glm::vec2(fNextAngleAround / 360.0f, fNextAngleTube / 360.0f)
					};
					int iIndices[] = { 0, 1, 2, 2, 3, 0 };
					for (auto i = 0; i < 6; i++) {
						int index = iIndices[i];
						m_vbo.addData(&vQuadPoints[index], sizeof(glm::vec3));
						m_vbo.addData(&vTexCoords[index], sizeof(glm::vec2));
						m_vbo.addData(&vNormals[index], sizeof(glm::vec3));
					}
					iFacesAdded += 2; // Keep count of added faces
					fCurAngleTube += fAddAngleTube;
					iStepsTube++;
				}
				fCurAngleAround += fAddAngleAround;
				iStepsAround++;
			}
			return iFacesAdded;
		}
	public:
		Tor(float radius = 6.0f, float tubeRadius = 1.0f, int SubDivArround = 30, int SubDivTube = 50,
			const glm::vec3 &scale = glm::vec3(1.0f), 
			const glm::vec3 &position = glm::vec3(0.0f), 
			GLfloat angle = 0.0f, const glm::vec3 &normals = glm::vec3(0.0f))
			: Primitives(scale, position, angle, normals) {
			m_radius = radius; m_tubeRadius = tubeRadius; m_SubDivAround = SubDivArround; m_subDivTibe = SubDivTube;
		}
		Tor(const Tor& tor) : Primitives(tor) {
			m_radius = tor.m_radius;
			m_tubeRadius = tor.m_tubeRadius;
			m_SubDivAround = tor.m_SubDivAround;
			m_subDivTibe = tor.m_subDivTibe;
		}
		~Tor() { }
	};

	class Line : public Primitives {
		static const int countVert = 2;
		static std::array<GLfloat, 3 * countVert> m_vertices;

		virtual void setupMesh() {
			if (!m_vao.get()) {
				try {
					m_vao.create()->bind();
					m_vbo.create()->bind();
				}
				catch (std::runtime_error &ex) {
					throw ex;
				}
				m_vbo.addData(&m_vertices, sizeof(m_vertices));
				m_vbo.uploadDataToGPU(GL_STATIC_DRAW);

				m_vao.addAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

				m_vbo.unbind();
				m_vao.unbind();
			}
		}
		virtual void draw() {
			glEnable(GL_PROGRAM_POINT_SIZE);
			glDrawArrays(GL_LINES, 0, countVert);
		}
		virtual void drawPath() { glDrawArrays(GL_PATCHES, 0, countVert); }
	public:
		Line(const glm::vec3 scale = glm::vec3(1.0f),
			const glm::vec3 &position = glm::vec3(0.0f),
			GLfloat angle = 0.0f, const glm::vec3 &normal = glm::vec3(0.0f))
			: Primitives(scale, position, angle, normal) { }
		Line(const Line& plain) : Primitives(plain) { }
		~Line() { }
	};
	std::array <GLfloat, 3 * Line::countVert> Line::m_vertices = {
		1.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f
	};
};