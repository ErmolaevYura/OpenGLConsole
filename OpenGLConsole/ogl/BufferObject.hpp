#pragma once
#include <vector>
#include <exception>
#include "opengl.h"

// void glGenBuffers(GLsizei n​, GLuint * buffers​);
//	n - Specifies the number of buffer object names to be generated.
//	buffers - Specifies an array in which the generated buffer object names are stored.
//	Errors: GL_INVALID_VALUE is generated if n​ is negative.

// GLboolean glIsBuffer(GLuint buffer​);
//	buffer - Specifies a value that may be the name of a buffer object.
//	return GL_TRUE or GL_FALSE

//void glBindBuffer(GLenum target​, GLuint buffer​);
//	target - Specifies the target buffer object. 
//		The symbolic constant must be 
//			GL_ARRAY_BUFFER, 
//			GL_ATOMIC_COUNTER_BUFFER, 
//			GL_COPY_READ_BUFFER, 
//			GL_COPY_WRITE_BUFFER, 
//			GL_DRAW_INDIRECT_BUFFER, 
//			GL_DISPATCH_INDIRECT_BUFFER, 
//			GL_ELEMENT_ARRAY_BUFFER, 
//			GL_PIXEL_PACK_BUFFER, 
//			GL_PIXEL_UNPACK_BUFFER, 
//			GL_QUERY_BUFFER, 
//			GL_SHADER_STORAGE_BUFFER, 
//			GL_TEXTURE_BUFFER, 
//			GL_TRANSFORM_FEEDBACK_BUFFER, 
//			GL_UNIFORM_BUFFER
//	buffer - Specifies the name of a buffer object.
//	Errors
//		GL_INVALID_ENUM is generated if target​ is not one of the allowable values.
//		GL_INVALID_VALUE is generated if buffer​ is not a name previously returned from a call to glGenBuffers.

// void glBufferData(GLenum target​, GLsizeiptr size​, const GLvoid * data​, GLenum usage​);
// target - Specifies the target buffer object.
//		The symbolic constant must be 
//			GL_ARRAY_BUFFER, 
//			GL_ATOMIC_COUNTER_BUFFER, 
//			GL_COPY_READ_BUFFER, 
//			GL_COPY_WRITE_BUFFER, 
//			GL_DRAW_INDIRECT_BUFFER, 
//			GL_DISPATCH_INDIRECT_BUFFER, 
//			GL_ELEMENT_ARRAY_BUFFER, 
//			GL_PIXEL_PACK_BUFFER, 
//			GL_PIXEL_UNPACK_BUFFER, 
//			GL_QUERY_BUFFER, 
//			GL_SHADER_STORAGE_BUFFER, 
//			GL_TEXTURE_BUFFER, 
//			GL_TRANSFORM_FEEDBACK_BUFFER, 
//			GL_UNIFORM_BUFFER
//	size - Specifies the size in bytes of the buffer object's new data store.
//	data - Specifies a pointer to data that will be copied into the data store 
//		for initialization, or NULL if no data is to be copied.
//	usage - Specifies the expected usage pattern of the data store.
//		The symbolic constant must be 
//			GL_STREAM_DRAW, 
//			GL_STREAM_READ, 
//			GL_STREAM_COPY, 
//			GL_STATIC_DRAW,
//			GL_STATIC_READ, 
//			GL_STATIC_COPY, 
//			GL_DYNAMIC_DRAW, 
//			GL_DYNAMIC_READ, 
//			GL_DYNAMIC_COPY
//	Errors
//		GL_INVALID_ENUM is generated if target​ is not one of the accepted buffer targets.
//		GL_INVALID_ENUM is generated if usage​ is not valid
//		GL_INVALID_VALUE is generated if size​ is negative.
//		GL_INVALID_OPERATION is generated if the reserved buffer object name 0 is bound to target​.
//		GL_OUT_OF_MEMORY is generated if the GL is unable to create a data store with the specified size​.

//DeleteBuffers(sizei n, const uint *buffers);

/*
Core API Ref Buffer Objects"
glBindBuffer
glBindBufferBase
glBindBufferRange
glBindBuffersBase
glBindBuffersRange
glBufferData
glBufferStorage
glBufferSubData
glClearBufferData
glClearBufferSubData
glCopyBufferSubData
glDeleteBuffers
glFlushMappedBufferRange
glGenBuffers
glGetBufferParameter
glGetBufferPointer
glGetBufferSubData
glInvalidateBufferData
glInvalidateBufferSubData
glIsBuffer
glMapBuffer
glMapBufferRange
glUnmapBuffer
*/

namespace ogl {
	class BufferObject {
	public:
		enum Target {
			ARRAY = GL_ARRAY_BUFFER,
			ATOMIC_COUNTER = GL_ATOMIC_COUNTER_BUFFER,			// only if the GL version is 4.2 or greater.
			COPY_READ = GL_COPY_READ_BUFFER,					// only if the GL version is 3.1 or greater.
			COPY_WRITE = GL_COPY_WRITE_BUFFER,					// only if the GL version is 3.1 or greater.
			DRAW_INDIRECT = GL_DRAW_INDIRECT_BUFFER,
			DISPATCH_INDERECT = GL_DISPATCH_INDIRECT_BUFFER,	// only if the GL version is 4.3 or greater.
			ELEMENT_ARRAY = GL_ELEMENT_ARRAY_BUFFER,
			PIXEL_PACK = GL_PIXEL_PACK_BUFFER,
			PIXEL_UNPACK = GL_PIXEL_UNPACK_BUFFER,
			QUERY = GL_QUERY_BUFFER,							// only if the GL version is 4.4 or greater.
			SHADER_STORAGE = GL_SHADER_STORAGE_BUFFER,			// only if the GL version is 4.3 or greater.
			TEXTURE = GL_TEXTURE_BUFFER,
			TRANSFORM_FEEDBACK = GL_TRANSFORM_FEEDBACK_BUFFER,
			UNIFORM = GL_UNIFORM_BUFFER							// only if the GL version is 3.1 or greater.
		};
		enum Usage {
			STREAM_DRAW = GL_STREAM_DRAW,
			STREAM_READ = GL_STREAM_READ,
			STREAM_COPY = GL_STREAM_COPY,
			STATIC_DRAW = GL_STATIC_DRAW,
			STATIC_READ = GL_STATIC_READ,
			STATIC_COPY = GL_STATIC_COPY,
			DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
			DYNAMIC_READ = GL_DYNAMIC_READ,
			DYNAMIC_COPY = GL_DYNAMIC_COPY
		};
		enum Access {
			READ_ONLY = GL_READ_ONLY, 
			WRITE_ONLY = GL_WRITE_ONLY,
			READ_WRITE = GL_READ_WRITE
		};
	public:
		static bool IsBuffer(GLuint handle) { return glIsBuffer(handle); }
	protected:
		GLuint m_handle = 0;					// OpenGL buffer handle
		Target m_target = ARRAY;				// Current target
		std::vector<byte> m_data;				// Data for sending
		GLsizei m_size;							// Size buffer in gpu
		bool m_dataUploaded = false;			// Uploaded
	public:
		BufferObject* create() {
			glGenBuffers(1, &m_handle);
			if (m_handle == 0) throw ogl::ResourceNotAllocate(m_target);
			return this;
		}
		void release() {
			if (m_handle != 0) glDeleteBuffers(1, &m_handle);
			if(!m_data.empty()) m_data.clear();
			m_dataUploaded = false;
		}
		
		BufferObject* bind(Target targetData) {
			if (m_handle == 0) throw ogl::ResourceNotAllocate(targetData);
			m_target = targetData;
			glBindBuffer(targetData, m_handle);
			return this;
		}
		BufferObject* bind(GLenum target = GL_ARRAY_BUFFER) {
			return bind((Target)target);
		}
		void unbind() { glBindBuffer(m_target, 0); }
		BufferObject* addData(void* data, size_t dataSize) {
			m_data.insert(m_data.end(), (byte*)data, (byte*)data + dataSize);
			return this;
		}
		BufferObject* uploadDataToGPU(Usage usage) {
			if (m_dataUploaded && m_size == m_data.size())
				glBufferSubData(m_target, 0, m_data.size(), &m_data[0]);
			else 
				glBufferData(m_target, m_data.size(), &m_data[0], usage);
			m_dataUploaded = true;
			m_data.clear();
			return this;
		}
		BufferObject* uploadDataToGPU(GLenum usage) {
			return uploadDataToGPU((Usage)usage);
		}
		void* getDataPointer() {
			if (m_dataUploaded) return nullptr;
			else return (void*)m_data[0];
		}
		void* mapBuffer(Access access) {
			if (!m_dataUploaded) return nullptr;
			return glMapBuffer(m_target, access);
		}
		void* mapBuffer(GLenum access) {
			return mapBuffer((Access)access);
		}
		void* mapSubBuffer(Access access, GLintptr offset, GLsizeiptr length) {
			if (!m_dataUploaded) return nullptr;
			return glMapBufferRange(m_target, offset, length, access);
		}
		void* mapSubBuffer(GLbitfield access, GLintptr offset, GLsizeiptr length) {
			mapSubBuffer((Access)access, offset, length);
		}
		void unmap() { glUnmapBuffer(m_target); }
		GLuint get() { return m_handle; }
	};
};
