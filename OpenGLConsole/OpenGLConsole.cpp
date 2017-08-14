#include "stdafx.h"
#include "app.h"
#include "ogl/ogl.h"

using namespace ogl;

int main(int argc, char* argv[]) {
	if (ogl::app.Init()) return core::Exit(0);
	ogl::app.resetTimer();
	while (!glfwWindowShouldClose(ogl::app.m_window)) {
		ogl::app.appBoby();
		ogl::app.swapBuffers();
	}
	ogl::app.Deinit();
	return 0;
}

