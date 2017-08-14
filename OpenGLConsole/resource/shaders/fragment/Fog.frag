#version 330

struct FogParameters {
	vec4 color; // Fog color
	float start; // This is only for linear fog
	float end; // This is only for linear fog
	float density; // For exp and exp2 equation
	int equation; // 0 = linear, 1 = exp, 2 = exp2
};

float getFogFactor(FogParameters params, float Distance) {
	float result = 0.0;
	if(params.equation == 0)
		result = (params.end - Distance) / (params.end - params.start);
	else if(params.equation == 1)
		result = exp(-params.density * Distance);
	else if(params.equation == 2)
		result = exp(-pow(params.density * Distance, 2.0));
	result = 1.0 - clamp(result, 0.0, 1.0);
	return result;
}