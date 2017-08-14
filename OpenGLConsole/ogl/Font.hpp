#pragma once

#include <memory>
#include <cmath>
#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif // WIN32 || WIN64
#include <ft2build.h>
#include FT_FREETYPE_H
#include "../repository/tstring.h"
#include "opengl.h"
#include "Texture.hpp"
#include "ArrayBuffer.hpp"
#include "BufferObject.hpp"
#include "Shader.hpp"

namespace ogl {
	class Font {
		tstring m_filename;
		tstring m_directory;
		TextureSOIL m_charTextures[256];
		glm::vec4 m_color;
		std::shared_ptr<ogl::Shader> m_shader;
		int m_advX[256], m_advY[256];
		int m_bearingX[256], m_bearingY[256];
		int m_charWidth[256], m_charHeight[256];
		int m_size, m_newLine, m_status;
		bool m_loaded = false;

		ArrayBuffer m_vao;
		BufferObject m_vbo;

		FT_Library Lib;
		FT_Face m_face;

		inline int next_p2(int n) { int res = 1; while (res < n)res <<= 1; return res; }
		void createChar(int index) {
			FT_Load_Glyph(m_face, FT_Get_Char_Index(m_face, index), FT_LOAD_DEFAULT);
			FT_Render_Glyph(m_face->glyph, FT_RENDER_MODE_NORMAL);
			FT_Bitmap* bitmap = &m_face->glyph->bitmap;

			int w = bitmap->width, h = bitmap->rows,
				tw = next_p2(w), th = next_p2(h);
			GLubyte* data = new GLubyte[tw * th];
			for (auto ch = 0; ch < th; ch++)
				for (auto cw = 0; cw < tw; cw++)
					data[ch * tw + cw] = (ch >= h || cw >= w) ? 0 : bitmap->buffer[(h - ch - 1) * w + cw];

			try {
				m_charTextures[index].createFromData(data, tw, th, 8, GL_DEPTH_COMPONENT);
				m_charTextures[index].Magnification(GL_LINEAR);
				m_charTextures[index].Minification(GL_LINEAR);

				m_charTextures[index].setSamplerParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				m_charTextures[index].setSamplerParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			catch (const std::runtime_error &ex) {
				throw ex;
			}

			m_advX[index] = m_face->glyph->advance.x >> 6;
			m_bearingX[index] = m_face->glyph->metrics.horiBearingX >> 6;
			m_charWidth[index] = m_face->glyph->metrics.width >> 6;

			m_advY[index] = (m_face->glyph->metrics.height - m_face->glyph->metrics.horiBearingY) >> 6;
			m_bearingY[index] = m_face->glyph->metrics.horiBearingY >> 6;
			m_charHeight[index] = m_face->glyph->metrics.height >> 6;

			m_newLine = max(m_newLine, int(m_face->glyph->metrics.height >> 6));

			glm::vec2 quad[] = {
				glm::vec2(0.0f, float(-m_advY[index] + th)),
				glm::vec2(0.0f, float(-m_advY[index])),
				glm::vec2(float(tw), float(-m_advY[index] + th)),
				glm::vec2(float(tw), float(-m_advY[index])),
			};
			glm::vec2 texQuad[] = { glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f) };
			for (auto i = 0; i < 4; i++) {
				m_vbo.addData(&quad[i], sizeof(glm::vec2));
				m_vbo.addData(&texQuad[i], sizeof(glm::vec2));
			}
			delete[] data;
		}
	public:
		Font(tstring filepath, int size = 24) {
			try {
				LoadFont(filepath, size);
			}
			catch (const std::runtime_error &ex) {
				throw ex;
			}
		}
		~Font() { Release(); }
		bool LoadFont(tstring filepath, int size) {
			bool error = FT_Init_FreeType(&Lib);
			error = FT_New_Face(Lib, GetC(filepath.c_str()), 0, &m_face);
			if (error) return false;
			FT_Set_Pixel_Sizes(m_face, size, size);
			m_size = size;
			try {
				m_vao.create()->bind();
				m_vbo.create()->bind();
				for (auto i = 0; i < 128; i++) createChar(i);
			}
			catch (const std::runtime_error &ex) {
				throw ex;
			}
			m_loaded = true;
			FT_Done_Face(m_face);
			FT_Done_FreeType(Lib);

			m_vbo.uploadDataToGPU(GL_STATIC_DRAW);
			m_vao.addAttrib(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2) * 2, 0);
			m_vao.addAttrib(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2) * 2, sizeof(glm::vec2));
			return true;
		}
		bool LoadSystemFont(tstring name, int size) {
			TCHAR buf[512];
#if defined(_WIN32) || defined(_WIN64)
			GetWindowsDirectory(buf, 512);
#else 
			buf = _T("resource");
#endif // WIN32 || WIN64
			tstring path = buf;
			path += _T("/Fonts/");
			path += name;
			return LoadFont(path, size);
		}
		void Release() {
			for (auto i = 0; i < 128; i++) m_charTextures[i].release();
			m_vbo.release();
			m_vao.release();
		}
		void Print(tstring text, int x, int y, int size = -1) {
			if (!m_loaded) throw std::runtime_error("Don't loaded font");
			GLboolean blendEn, depthEn;
			GLint blendSrc, blendDst;
			m_vao.bind();
			m_shader->bind(_T("fontColor"), m_color);
			glGetBooleanv(GL_BLEND, &blendEn);
			glGetBooleanv(GL_DEPTH_TEST, &depthEn);
			glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
			glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);
			if (depthEn) glDisable(GL_DEPTH_TEST);
			if (!blendEn) glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			int curX = x, curY = y;
			if (size == -1) size = m_size;
			float scale = float(size) / float(m_size);
			for (auto i = 0; i < text.length(); i++) {
				if (text[i] == _T('\n')) {
					curX = x;
					curY -= m_newLine * scale;
					continue;
				}
				int index = int(text[i]);
				curX += m_bearingX[index] * scale;
				if (text[i] != _T(' ')) {
					try {
						m_charTextures[index].bind(m_shader, _T("textSampler"));
					}
					catch (const std::runtime_error &ex) {
						throw ex;
					}
					glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(float(curX), float(curY), 0.0f));
					model = glm::scale(model, glm::vec3(scale));
					m_shader->bind(_T("model"), model);
					glDrawArrays(GL_TRIANGLE_STRIP, index * 4, 4);
				}
				curX += (m_advX[index] - m_bearingX[index]) * scale;
			}
			if(!blendEn) glDisable(GL_BLEND);
			glBlendFunc(blendSrc, blendDst);
			if(depthEn) glEnable(GL_DEPTH_TEST);
		}
		Font* Color(glm::vec4 color) { m_color = color; return this;  }
		glm::vec4 Color() { return m_color; }
		Font* Shader(std::shared_ptr<ogl::Shader> shader) { m_shader = shader; return this; }
		std::shared_ptr<ogl::Shader> Shader() { return m_shader; }
	};
};
