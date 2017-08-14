#pragma once

#define SOIL
#ifdef SOIL
#include <SOIL.h>
#endif //SOIL

#ifdef FREEIMAGE
#include "freeimage.h"
#endif //FREEIMAGE
#include "opengl.h"
#include "Shader.hpp"

namespace ogl
{
	class Texture {
	public:
		enum class TYPE : uint8_t {
			NONE = 0, DIFFUSE = 1, SPECULAR = 2, AMBIENT = 3, EMISSIVE = 4, HEIGHT = 5,
			NORMALS = 6, SHININESS = 7, OPACITY = 8, DISPLACEMENT = 9, LIGHTMAP = 10,
			REFLECTION = 11, UNKNOW = 12, CUBEMAP = 100
		};
		static TYPE typeFromString(tstring type) {
			if (!type.compare(_T("diffuse"))) return TYPE::DIFFUSE;
			else if (!type.compare(_T("specular"))) return TYPE::SPECULAR;
			else if (!type.compare(_T("ambient"))) return TYPE::AMBIENT;
			else if (!type.compare(_T("emissive"))) return TYPE::EMISSIVE;
			else if (!type.compare(_T("height"))) return TYPE::HEIGHT;
			else if (!type.compare(_T("normals"))) return TYPE::NORMALS;
			else if (!type.compare(_T("shininess"))) return TYPE::SHININESS;
			else if (!type.compare(_T("opacity"))) return TYPE::OPACITY;
			else if (!type.compare(_T("displacement"))) return TYPE::DISPLACEMENT;
			else if (!type.compare(_T("lightmap"))) return TYPE::LIGHTMAP;
			else if (!type.compare(_T("reflection"))) return TYPE::REFLECTION;
			else return TYPE::NONE;
		};
	protected:
		GLuint m_textureId = 0;
		GLuint m_samplerId = 0;
		GLuint m_activeIndex = 0;
		tstring m_filename;
		tstring m_directory;
		TYPE m_type = TYPE::NONE;

		int m_width, m_height, m_bpp;
		int m_minification, m_magnification;
		bool m_mipMaps = false;

		inline void parsePath(tstring filepath) {
			std::size_t div = filepath.find_last_of('/');
			if (div != tstring::npos) {
				m_directory = filepath.substr(0, div);
				m_filename = filepath.substr(++div, filepath.length());
			}
			else m_filename = filepath;
		}
	public:
		Texture() { }
		virtual ~Texture() { release(); };

		inline GLuint Id() { return m_textureId; }

		inline TYPE Type() { return m_type; }
		inline void Type(TYPE type) { m_type = type; }

		Texture* Minification(GLint minification) {
			return setSamplerParameter(GL_TEXTURE_MIN_FILTER, minification);
		}
		Texture* Magnification(GLint magnification) {
			return setSamplerParameter(GL_TEXTURE_MAG_FILTER, magnification);
		}
		inline Texture* setSamplerParameter(GLenum parameter, GLenum value) {
			if (m_samplerId != 0) {
				glSamplerParameteri(m_samplerId, parameter, value);
				switch (parameter) {
				case GL_TEXTURE_MIN_FILTER: m_minification = value; break;
				case GL_TEXTURE_MAG_FILTER: m_magnification = value; break;
				}
			}
			return this;
		}
		int Minification() { return m_minification; }
		int Magnification() { return m_magnification; }

		int Width() { return m_width; }
		int Height() { return m_height; }
		int Bpp() { return m_bpp; }

		inline tstring FileName() { return m_filename; }
		inline tstring Directory() { return m_directory; }
		inline tstring FilePath() { return m_directory.empty() ? m_filename : m_directory + _T('/') + m_filename; }
		virtual GLuint loadTexture(tstring filepath, TYPE type = TYPE::NONE, bool mipMaps = true) = 0;
		static GLuint createFromData(uint8_t* data, int width, int height, GLenum format = GL_RGB, bool mipMaps = true) {
			GLuint textureId = 0;
			glGenTextures(1, &textureId);
			if (textureId == 0) throw ogl::ResourceNotAllocate(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, textureId);
			if (format == GL_RGB || format == GL_BGR) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				if (mipMaps) glGenerateMipmap(GL_TEXTURE_2D);
			}
			else {
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				if (mipMaps) glGenerateMipmap(GL_TEXTURE_2D);
			}
			return textureId;
		}
		GLuint createFromData(uint8_t* data, int width, int height, int bbp, GLenum format = GL_RGB, bool mipMaps = true) {
			try {
				m_textureId = Texture::createFromData(data, width, height, format, mipMaps);
			}
			catch (std::runtime_error& ex) {
				throw ex;
			}
			m_width = width;
			m_height = height;
			m_bpp = bbp;
			m_filename = _T("");
			m_mipMaps = mipMaps;
			glGenSamplers(1, &m_samplerId);
			if (m_samplerId == 0) throw ogl::ResourceNotAllocate(GL_SAMPLER_2D);
			return m_textureId;
		}
		virtual std::shared_ptr<Shader> bind(std::shared_ptr<Shader> shader, tstring name, bool isArray = false, GLuint num = 0, GLuint active = 0) {
			if (m_textureId == 0) throw std::runtime_error("There is no Texture!");
			m_activeIndex = active;
			glActiveTexture(GL_TEXTURE0 + m_activeIndex);
			if (isArray) shader->bind(name + '[' + to_tstring(num) + ']', m_activeIndex);
			else shader->bind(name, m_activeIndex);
			if(m_type == Texture::TYPE::CUBEMAP) glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureId);
			else glBindTexture(GL_TEXTURE_2D, m_textureId);
			glBindSampler(active, m_samplerId);
			return shader;
		}
		virtual void unbind() {
			glActiveTexture(GL_TEXTURE0 + m_activeIndex);
			if (m_type == Texture::TYPE::CUBEMAP) glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			else glBindTexture(GL_TEXTURE_2D, 0);
			m_activeIndex = 0;
		}
		virtual void release() {
			if(m_samplerId != 0) glDeleteSamplers(1, &m_samplerId);
			if(m_textureId != 0) glDeleteTextures(1, &m_textureId);
		}
	};
	
	class TextureOGL : public Texture {
	public:
		static GLuint loadTexture(tstring filepath, int &width, int &height, bool mipMaps = true) {
			try {
				if (filepath.find(_T(".bmp/0")) != std::string::npos)
					return TextureOGL::loadBMP_texture(filepath, width, height, mipMaps);
				else if (filepath.find(_T(".dds/0")) != std::string::npos)
					return TextureOGL::loadDDS_texture(filepath, width, height, mipMaps);
			}
			catch (const std::runtime_error &ex) {
				throw ex;
			}
		}
		GLuint loadTexture(tstring filepath, TYPE type = TYPE::NONE, bool mipMaps = true) {
			m_type = type;
			parsePath(filepath);
			try {
				if (filepath.find(_T(".bmp/0")) != std::string::npos)
					return loadBMP_texture(filepath, mipMaps);
				else if (filepath.find(_T(".dds/0")) != std::string::npos)
					return loadDDS_texture(filepath, mipMaps);
			}
			catch (const std::runtime_error &ex) {
				throw ex;
			}
		}
		static GLuint loadBMP_texture(tstring filepath, int &width, int &height, bool mipMaps = true) {
			ConsoleLogger::AddInfo(_T("Reading BMP image {0}"), filepath);

			unsigned char header[56];
			unsigned int dataPos;
			unsigned int imageSize;

			uint8_t* data;

			FILE* file;
			fopen_s(&file, GetC(filepath.data()), "rb");
			if (!file) { throw ogl::FileNotFound(filepath); return 0; }
			if (fread_s(header, 54, 1, 54, file)) {
				tstring msg = _T("File ") + filepath + _T(". Not correct BMP");
				throw std::runtime_error(GetC(msg.c_str()));
				fclose(file);
				return 0;
			}
			if (header[0] != 'B' || header[1] != 'M' || *(int*)&header[0x1E] != 0 || *(int*)&header[0x1C] != 24) {
				tstring msg = _T("File ") + filepath + _T(". Not correct BMP");
				throw std::runtime_error(GetC(msg.c_str()));
				fclose(file);
				return 0;
			}

			dataPos = *(int*)&(header[0x0A]);
			imageSize = *(int*)&(header[0x22]);
			width = *(int*)&(header[0x12]);
			height = *(int*)&(header[0x16]);

			if (imageSize == 0) imageSize = width * height * 3;
			if (dataPos == 0) dataPos = 54;
			data = new uint8_t[imageSize];
			fread_s(data, imageSize, 1, imageSize, file);
			fclose(file);

			GLuint textureId;
			glGenTextures(1, &textureId);
			if (textureId == 0) throw std::runtime_error("No Texture was created!");
			glBindTexture(GL_TEXTURE_2D, textureId);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			delete[] data;

			if (mipMaps) glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			return textureId;
		}
		GLuint loadBMP_texture(tstring filepath, bool mipMaps = true) {
			try {
				m_textureId = TextureOGL::loadBMP_texture(filepath, m_width, m_height, mipMaps);
			}
			catch (const std::runtime_error &ex) {
				throw ex;
			}
			m_mipMaps = mipMaps;
			glBindTexture(GL_TEXTURE_2D, 0);
			glGenSamplers(1, &m_textureId);
			glBindTexture(GL_TEXTURE_2D, 0);
			if (m_samplerId == 0) throw ogl::ResourceNotAllocate(GL_SAMPLER_2D);
			return m_textureId;
		}
		static GLuint loadDDS_texture(tstring filepath, int &width, int &height, bool mipMaps = true) {
			static const int FOURCC_DXT1 = 0x31545844;
			static const int FOURCC_DXT3 = 0x33545844;
			static const int FOURCC_DXT5 = 0x35545844;

			ConsoleLogger::AddInfo(_T("Reading DDS image {0}"), filepath);

			unsigned char header[124];
			FILE* file;
			fopen_s(&file, GetC(filepath.data()), "rb");
			if (!file) { throw ogl::FileNotFound(filepath); return 0; }
			char filecode[4];
			fread_s(filecode, 4, 1, 4, file);
			if (strncmp(filecode, "DDS", 4) != 0)
			{
				ConsoleLogger::AddError(_T("File {0}. Not correct DDS"), filepath);
				core::logger->AddError(_T("File {0}. Not correct DDS"), filepath);
				fclose(file);
				return 0;
			}

			fread_s(header, 124, 1, 124, file);

			height = *(unsigned int*)&(header[8]);
			width = *(unsigned int*)&(header[12]);
			unsigned int linearSize = *(unsigned int*)&(header[16]);
			unsigned int mipMapCount = *(unsigned int*)&(header[24]);
			unsigned int fourCC = *(unsigned int*)&(header[80]);

			unsigned char * buffer;
			unsigned int bufsize;

			bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
			buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
			fread(buffer, 1, bufsize, file);

			fclose(file);

			unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
			unsigned int format;
			switch (fourCC)
			{
			case FOURCC_DXT1: format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; break;
			case FOURCC_DXT3: format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
			case FOURCC_DXT5: format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
			default: free(buffer); return 0;
			}

			GLuint textureId;
			glGenTextures(1, &textureId);
			if (textureId == 0) throw std::runtime_error("No Texture was created!");
			glBindTexture(GL_TEXTURE_2D, textureId);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
			unsigned int offset = 0;
			for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) {
				unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
				glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer + offset);
				offset += size;
				width /= 2;
				height /= 2;

				if (width < 1) width = 1;
				if (height < 1) height = 1;
			}
			free(buffer);
			if (mipMaps) glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			return textureId;
		}
		GLuint loadDDS_texture(tstring filepath, bool mipMaps = true) {
			m_textureId = loadDDS_texture(filepath, m_width, m_height, mipMaps);
			m_mipMaps = mipMaps;
			glBindTexture(GL_TEXTURE_2D, m_textureId);
			glGenSamplers(1, &m_samplerId);
			glBindTexture(GL_TEXTURE_2D, 0);
			if (m_samplerId == 0) throw ogl::ResourceNotAllocate(GL_SAMPLER_2D);
			return m_textureId;
		}
	};
	
#ifdef SOIL
	class TextureSOIL : public Texture
	{
	public:
		static GLuint loadTexture(tstring filepath, int &width, int &height, bool mipMaps = true) {
			uint8_t* image = SOIL_load_image(GetC(filepath.c_str()), &width, &height, 0, SOIL_LOAD_RGB);
			if (image == nullptr) { throw ogl::FileNotFound(filepath); return 0; }
			GLuint textureId;
			glGenTextures(1, &textureId);
			if (textureId == 0) {
				throw ogl::ResourceNotAllocate(GL_TEXTURE_2D);
				return 0;
			}
			glBindTexture(GL_TEXTURE_2D, textureId);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			SOIL_free_image_data(image);

			if (mipMaps) glGenerateMipmap(GL_TEXTURE_2D);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);

			return textureId;
		}
		GLuint loadTexture(tstring filepath, TYPE type = TYPE::NONE, bool mipMaps = true) {
			m_type = type;
			parsePath(filepath);
			try {
				m_textureId = TextureSOIL::loadTexture(filepath, m_width, m_height, mipMaps);
			}
			catch (const std::runtime_error &ex) {
				throw ex;
			}
			m_mipMaps = mipMaps;
			glGenSamplers(1, &m_samplerId);
			glBindTexture(GL_TEXTURE_2D, 0);
			if (m_samplerId == 0) throw ogl::ResourceNotAllocate(GL_SAMPLER_2D);
			return m_textureId;
		}
		static GLuint loadCubemap(tstring rootpath, tstring name, std::vector<tstring> faces, int &width, int &height, bool mipMaps = true) {
			GLuint textureId;
			glGenTextures(1, &textureId);
			
			if (textureId == 0) throw ogl::ResourceNotAllocate(GL_TEXTURE_CUBE_MAP);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
			uint8_t* image;
			for (auto i = 0; i < faces.size(); i++) {
				image = SOIL_load_image(GetC((rootpath + name + _T("/") + faces.at(i)).c_str()), &width, &height, 0, SOIL_LOAD_RGB);
				if (image == nullptr) {
					glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
					glDeleteTextures(1, &textureId);
					throw ogl::FileNotFound(rootpath + name + _T("/") + faces.at(i));
				}
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
				SOIL_free_image_data(image);
			}
			if (mipMaps) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			return textureId;
		}
		GLuint loadCubemap(tstring rootpath, tstring name, std::vector<tstring> faces, bool mipMaps = true) {
			try {
				m_textureId = TextureSOIL::loadCubemap(rootpath, name, faces, m_width, m_height, mipMaps);
			}
			catch (std::runtime_error &ex) {
				throw ex;
			}
			m_directory = rootpath;
			m_filename = name;
			m_mipMaps = mipMaps;
			glGenSamplers(1, &m_samplerId);
			if (m_samplerId == 0) {
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
				throw ogl::ResourceNotAllocate(GL_SAMPLER_CUBE);
				return m_textureId;
			}
			setSamplerParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			setSamplerParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			setSamplerParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			return m_textureId;
		}
	};
#endif

#ifdef FREEIMAGE
	class TextureFreeImage : public Texture
	{
		static bool m_initLib;
		static size_t m_countTextures;
	public:
		TextureFreeImage() {
			m_countTextures++;
#ifdef FREEIMAGE_LIB
			// call this ONLY when linking with FreeImage as a static library
			if (!m_initLib) {
				FreeImage_Initialise();
				m_initLib = true;
			}
#endif
		}
		~TextureFreeImage() {
#ifdef FREEIMAGE_LIB
			// call this ONLY when linking with FreeImage as a static library
			if (m_initLib && m_countTextures == 0) {
				FreeImage_DeInitialise();
				m_initLib = false;
			}
#endif
			m_countTextures--;
		}	
		static GLuint loadTexture(tstring filepath, int &width, int &height, int &bpp, bool mipMaps = true) {
			FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
			FIBITMAP* did(0);
			fif == FreeImage_GetFileType(GetC(filepath.c_str()), 0);
			if (fif == FIF_UNKNOWN) fif = FreeImage_GetFIFFromFilename(GetC(filepath.c_str()));
			if (fif == FIF_UNKNOWN) {
				tstring msg = _T("Unknow format from load texture ") + filepath;
				throw std::runtime_error(GetC(msg.c_str()));
				return 0;
			}

			if (FreeImage_FIFSupportsReading(fif))
				did = FreeImage_Load(fif, GetC(filepath.c_str()));
			else {
				tstring msg = _T("FreeImage not supported format ") + filepath;
				throw std::runtime_error(GetC(msg.c_str()));
				return 0;
			}
			if (!did) {
				FreeImage_Unload(did);
				tstring msg = _T("FreeImage not loaded ") + filepath;
				throw std::runtime_error(GetC(msg.c_str()));
				return 0;
			}

			uint8_t* data = FreeImage_GetBits(did);
			width = FreeImage_GetWidth(did);
			height = FreeImage_GetHeight(did);
			bpp = FreeImage_GetBPP(did);
			FreeImage_Unload(did);
			if (data == nullptr || width == 0 || height == 0) return 0;

			GLuint textureId;
			glGenTextures(1, &textureId);
			if (textureId == 0) throw std::runtime_error("No Texture was created!");
			glBindTexture(GL_TEXTURE_2D, textureId);

			int format = bpp == 24 ? GL_RGB : bpp == 8 ? GL_LUMINANCE : 0;
			int internalFormat = bpp == 24 ? GL_RGB : GL_DEPTH_COMPONENT;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			if (mipMaps) glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			return textureId;
		}
		GLuint loadTexture(tstring filepath, TYPE type = TYPE::NONE, bool mipMaps = true) {
			m_type = type;
			parsePath(filepath);
			m_textureId = TextureFreeImage::loadTexture(filepath, m_width, m_height, m_bpp, m_mipMaps);
			glBindTexture(GL_TEXTURE_2D, m_textureId);
			m_mipMaps = mipMaps;
			glGenSamplers(1, &m_samplerId);
			if (m_samplerId == 0) throw std::runtime_error("No Texture Sampler was created!");
			glBindTexture(GL_TEXTURE_2D, 0);
			return m_textureId;
		}
	};
	bool TextureFreeImage::m_initLib = false;
	size_t TextureFreeImage::m_countTextures = 0;
#endif
};
