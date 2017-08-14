#pragma once
#include "stdafx.h"
#include "Exception.h"
#include "opengl.h"

using namespace ogl;

FileNotFound::FileNotFound(tstring filename)
	: runtime_error(GetC((_T("File ") + filename + _T(" not found")).c_str())) {
	m_filename = filename;
}
const char* FileNotFound::what() const throw() {
	cnvt.str(runtime_error::what());
	return cnvt.str().c_str();
}
tstring FileNotFound::getFilename() { return m_filename; }
tstringstream FileNotFound::cnvt;

ResourceNotAllocate::ResourceNotAllocate(GLenum type)
	: runtime_error(std::string("OpenGL resource not allocate: ") + GetC(getMsg(type).c_str())) {
	m_type = type;
}
const char* ResourceNotAllocate::what() const throw() {
	cnvt.str(runtime_error::what());
	cnvt << GetC(getMsg(m_type).c_str());
	return cnvt.str().c_str();
}
GLenum ResourceNotAllocate::getType() { return m_type; }
tstringstream ResourceNotAllocate::cnvt;