#pragma once
#include "opengl.h"
#include "Shader.hpp"

namespace ogl {
	enum FOG_TYPE { LINEAR = 0, EXP = 1, EXP2 = 2 };
	struct FogParameters {
		float Density = 0.05f;
		float Start = 25.0f;
		float End = 75.0f;
		glm::vec4 Color = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
		int Equation = FOG_TYPE::EXP2;
		void bind(std::shared_ptr<Shader> shader, std::string name) {
			shader
				->bind(name + ".density", Density)
				->bind(name + ".start", Start)
				->bind(name + ".end", End)
				->bind(name + ".color", Color)
				->bind(name + ".equation", (GLint)Equation)
				;
		}
	};
}
