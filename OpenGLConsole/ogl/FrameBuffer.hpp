#pragma once
#include <exception>
#include "Exception.h"
#include "opengl.h"

namespace ogl
{
	class FrameBuffer
	{
		inline static int maxColorAttachemnts() {
			int n;
			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &n);
			return n;
		}
	public:
		enum class FRAMETYPE : std::int16_t {
			EMPTY, DEPTH, COLOR, COLORDEPTH, DEPTHSTENCIL, FULL
		};
	private:
		int m_width;						// Ширина
		int m_height;						// Высота
		GLint m_curViewport[4];				// Вьюпорт перед и после привязки фреймбуфера
		GLuint m_status = 0;				// Статус (последней операции)

		GLuint m_fbo = 0;					// Хендл Фреймбуфера
		FRAMETYPE m_type = FRAMETYPE::EMPTY;// Тип фреймбуфера
		GLuint *m_colorTexture;				// Указатель на массив текстур цвета
		GLuint m_depthTexture = 0;			// Текстура глубины
		GLuint m_depthStencilTexture = 0;	// Текстура глубины и трафарета

		GLuint m_depthBuffer = 0;			// Renderbuffer для хранения глубины / трафарета

		/*  Генератор текстуры цвета */
		void f_createColorTexture(uint8_t num = 0) {
			if (num >= maxColorAttachemnts()) return;
			detachColorTexture(num);
			glGenTextures(1, &m_colorTexture[num]);
			if (m_colorTexture[num] == 0) throw ogl::ResourceNotAllocate(GL_COLOR_ATTACHMENT0);
			glBindTexture(GL_TEXTURE_2D, m_colorTexture[num]);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + num, m_colorTexture[num], 0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		/* Генератор текстуры глубины */
		void f_createDepthTexture() {
			detachDepthTexture();
			glGenTextures(1, &m_depthTexture);
			if (m_depthTexture == 0) throw ogl::ResourceNotAllocate(GL_DEPTH_COMPONENT);
			glBindTexture(GL_TEXTURE_2D, m_depthTexture);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTexture, 0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		/* Генератор текстуры глубины и трафарета */
		void f_createDepthStencilTexture() {
			detachDepthStencilTexture();
			glGenTextures(1, &m_depthStencilTexture);
			if (m_depthStencilTexture == 0) throw ogl::ResourceNotAllocate(GL_DEPTH_STENCIL);
			glBindTexture(GL_TEXTURE_2D, m_depthStencilTexture);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_width, m_height, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_depthStencilTexture, 0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		/* Голая привязка фреймбуфера */
		void f_bind() const {
			GLint fbo;
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo);
			if (fbo != m_fbo) glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		}
	public:
		/* Пока не используется */
		inline void initRBO() {
			// create a MSAA renderbuffer object to store depth info
			glGenRenderbuffers(1, &m_depthBuffer);
			if (m_depthBuffer == 0) throw ogl::ResourceNotAllocate(GL_RENDERBUFFER);
			glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			m_status = checkFramebufferStatus(m_fbo);
		}
	public:
		FrameBuffer() { }
		FrameBuffer(int width, int height, FRAMETYPE target = FRAMETYPE::EMPTY) { 
			try {
				if (create(width, height, target) != GL_FRAMEBUFFER_COMPLETE)
					release();
			}
			catch (std::runtime_error &ex) {
				throw ex;
			}
		}
		~FrameBuffer() { release(); }
		
		int create(int width, int height, FRAMETYPE target = FRAMETYPE::EMPTY) {
			m_width = width; m_height = height;
			m_colorTexture = new GLuint[maxColorAttachemnts()];
			glGenFramebuffers(1, &m_fbo);
			if (m_fbo == 0) throw ogl::ResourceNotAllocate(GL_FRAMEBUFFER);
			glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
			try {
				switch (target) {
				case FRAMETYPE::DEPTH:
					f_createDepthTexture();
					glDrawBuffer(GL_NONE);
					glReadBuffer(GL_NONE);
					break;
				case FRAMETYPE::COLOR:
					f_createColorTexture();
					break;
				case FRAMETYPE::COLORDEPTH:
					f_createColorTexture();
					f_createDepthTexture();
					break;
				case FRAMETYPE::DEPTHSTENCIL:
					f_createDepthStencilTexture();
					glDrawBuffer(GL_NONE);
					glReadBuffer(GL_NONE);
					break;
				case FRAMETYPE::FULL:
					f_createColorTexture();
					f_createDepthStencilTexture();
					break;
				default:
					glDrawBuffer(GL_NONE);
					glReadBuffer(GL_NONE);
					//initRBO();
					break;
				}
			}
			catch (const std::runtime_error &ex) {
				throw ex;
			}
			core::AddInfo(getFramebufferInfo(m_fbo, GL_FRAMEBUFFER));
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			m_type = target;
			return m_status = checkFramebufferStatus(m_fbo);
		}
		void release() {
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
			for (auto i = 0; i < maxColorAttachemnts(); i++)
				if (m_colorTexture[i] != 0)
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			for (auto i = 0; i < maxColorAttachemnts(); i++) if (m_colorTexture[i] != 0) glDeleteTextures(1, &m_colorTexture[i]);
			if (m_depthTexture != 0) glDeleteTextures(1, &m_depthTexture);
			if (m_depthStencilTexture != 0) glDeleteTextures(1, &m_depthStencilTexture);

			if (m_depthBuffer != 0) glDeleteRenderbuffers(1, &m_depthBuffer);
			if (m_fbo != 0) glDeleteFramebuffers(1, &m_fbo);
		}
		
		FRAMETYPE Type() const { return m_type; }
		void Type(FRAMETYPE type) { m_type = type; }
		
		GLuint ColorTexture(int num = 0) const { return num < maxColorAttachemnts() ? m_colorTexture[num] : 0; }
		void ColorTexture(GLuint texture, int num = 0) { if (num < maxColorAttachemnts()) m_colorTexture[num] = texture; }

		GLuint DepthTexture() const { return m_depthTexture; }
		void DepthTexture(GLuint texture) { m_depthTexture = texture; }

		GLuint DepthStencilTexture() const { return m_depthStencilTexture; }
		void DepthStencilTexture(GLuint texture) { m_depthStencilTexture = texture;  }

		int getWidth() const { return m_width; }
		int getHeight() const { return m_height; }
		GLuint get() const { return m_fbo; }
		
		void bind(int width = 0, int height = 0) {
			if (m_fbo == 0) throw ogl::ResourceNotAllocate(GL_FRAMEBUFFER);
			glGetIntegerv(GL_VIEWPORT, m_curViewport);
			f_bind();
			switch (m_type) {
			case FRAMETYPE::COLOR:
				glClear(GL_COLOR_BUFFER_BIT);
				break;
			case FRAMETYPE::DEPTH:
				glClear(GL_DEPTH_BUFFER_BIT);
				break;
			case FRAMETYPE::COLORDEPTH:
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				break;
			case FRAMETYPE::DEPTHSTENCIL:
				glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
				break;
			case FRAMETYPE::FULL:
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
				break;
			default: break;
			}
			glViewport(0, 0, width == 0 ? m_width : width, height == 0 ? m_height : height);
		}
		void unbind() const {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(m_curViewport[0], m_curViewport[1], m_curViewport[2], m_curViewport[3]);
		}
		
		bool attachColorTexture(GLuint texture = 0, int num = 0) {
			f_bind();
			m_colorTexture[num] = texture;
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + num, m_colorTexture[num], 0);
			return true;
		}
		bool detachColorTexture(int num = 0) { 
			if (m_colorTexture[num] != 0) {
				glDeleteTextures(1, &m_colorTexture[num]);
				m_colorTexture[num] = 0;
			}
			return attachColorTexture(0, num); 
		}
		GLuint initColorTexture(uint8_t num = 0) {
			f_createColorTexture(num);
			if (checkFramebufferStatus(m_fbo) != GL_FRAMEBUFFER_COMPLETE)
				detachColorTexture(num);
			return m_colorTexture[num];
		}
		
		bool attachDepthTexture(GLuint texture = 0) {
			f_bind();
			m_depthTexture = texture;
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTexture, 0);
			return true;
		}
		bool detachDepthTexture() {
			if (m_depthTexture != 0) {
				glDeleteTextures(1, &m_depthTexture);
				m_depthTexture = 0;
			}
			return attachDepthTexture();
		}
		GLuint initDephtTexture() {
			f_createDepthTexture();
			if (checkFramebufferStatus(m_fbo) != GL_FRAMEBUFFER_COMPLETE)
				detachDepthTexture();
			return m_depthTexture;
		}

		bool attachDepthStencilTexture(GLuint texture = 0) {
			f_bind();
			m_depthStencilTexture = texture;
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_depthStencilTexture, 0);
			return true;
		}
		bool detachDepthStencilTexture() {
			if (m_depthStencilTexture != 0) {
				glDeleteTextures(1, &m_depthStencilTexture);
				m_depthStencilTexture = 0;
			}
			return attachDepthStencilTexture();
		}
		GLuint initDepthStencilTexture() {
			f_createDepthStencilTexture();
			if (checkFramebufferStatus(m_fbo) != GL_FRAMEBUFFER_COMPLETE)
				detachDepthStencilTexture();
			return m_depthStencilTexture;
		}
	};
};
