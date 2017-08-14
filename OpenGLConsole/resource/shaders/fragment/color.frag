#version 400

struct ColorMaterial {
	vec3 color;
	vec3 ambien;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct TexMaterial {
	//int diffuseCount;
	sampler2D diffuse[2];
	//int specularCount; 
	sampler2D specular[1];
	//int reflectionCount;
	sampler2D reflection[1];
	//int emissionCount;
	sampler2D emission[1];
	float shininess;
};

struct DirLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 color;
	vec3 direction;
};

struct PointLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 color;

	vec3 position;
	float constant;
	float quadratic;
	float linear;
};

struct SpotLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 color;

	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	float constant;
	float quadratic;
	float linear;
};

struct FogParameters {
	float density;	// For exp and exp2 equation
	float start;	// This is only for linear fog
	float end;		// This is only for linear fog
	vec4 color;	// Fog color
	int Equation;	// 0 = linear, 1 = exp, 2 = exp2
};
in vec3 Position;
in vec3 Normal;
in vec2 TexCoords;

layout(location = 0) out vec4 color;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;
uniform TexMaterial texMaterial;
uniform FogParameters fog;
uniform samplerCube skybox[1];

vec4 getDefault(vec3 result);
vec4 getHideBack(vec3 result);
vec4 getInversion(vec3 result);
vec4 getGrayscaling(vec3 result);
vec4 getDepth(vec3 result);
vec4 getKernel(vec3 result);
vec4 getBlur(vec3 result);
vec4 getEdge(vec3 result);

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, TexMaterial material);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 position, vec3 viewDir, TexMaterial material);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 position, vec3 viewDir, TexMaterial material);

float getFogFactor(FogParameters params, float fogCoord);
void main()
{
	//gl_FragCoord;			// xf, yf, zf, wf = 1 / wc
	//gl_FrontFacing;
	//gl_PrimitiveID;		// only OpenGL 4.1+
    //Position = vec3(gl_FragCoord.x, gl_FragCoord.y, gl_FragCoord.z);
	//color = vec4(ourColor, 1.0f);
	//color = texture(ourTexture, TexCoords) ;
	//color = mix(texture(ourTexture1, TexCoords) * vec4(ourColor, 0.3f), texture(ourTexture2, TexCoords), 0.8);
	//color = texture(ourTexture1, TexCoords);
	
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - Position);
	
	vec3 result;
	result = CalcDirLight(dirLight, norm, viewDir, texMaterial);
	result += CalcPointLight(pointLight, norm, Position, viewDir, texMaterial);
	result += CalcSpotLight(spotLight, norm, Position, viewDir, texMaterial);
	//result += vec3(texture(texMaterial.emission0, TexCoords)) * 0.2f;
	color = getDepth(result);

	vec3 I = normalize(Position - viewPos);
	vec3 R = reflect(I, normalize(Normal));
	float reflect_intensity = texture(texMaterial.reflection[0], TexCoords).r;
	vec4 reflect_color;
	if(reflect_intensity > 0.1) reflect_color = texture(skybox[0], R) * reflect_intensity;
	color += reflect_color;
}
vec4 getDefault(vec3 result) {
	return vec4(result, 1.0f);
}
vec4 getHideBack(vec3 result) {
	if(gl_FrontFacing) return vec4(result, 1.0f);
	else return vec4(result, 0.5f);
}
vec4 getInversion(vec3 result) { return vec4(vec3(1.0f) - result, 1.0f); }
vec4 getGrayscaling(vec3 result)
{
	float average = 0.2126 * result.r + 0.7152 * result.g + 0.0722 * result.b;
	return vec4(average, average, average, 1.0f);
}
vec4 getDepth(vec3 result) 
{
	float near = 1.0f;
	float far = 100.0f;
    float z = gl_FragCoord.z * 2.0f - 1.0f;
    float depth = (2.0f * near * far) / (far + near - z * (far - near)) / far;
    return vec4(result / vec3(depth), 1.0f);
}
vec4 getKernel(vec3 result)
{
	const float offset = 1.0f / 300;
	vec2 offsets[9] = vec2[](
		vec2(-offset,	offset),
		vec2(0.0f,		offset),
		vec2(offset,	offset),
		vec2(-offset,	0.0f),
		vec2(0.0f,		0.0f),
		vec2(offset,	0.0f),
		vec2(-offset,	-offset),
		vec2(0.0f,		-offset),
		vec2(offset,	-offset)
	);
	float kernel[9] = float[](
		-1,	-1, -1,
		-1,	 9, -1, 
		-1,	-1,	-1
	); 
	vec3 sampleTex[9];
	for(int i = 0; i < 9; i++)
	{
		sampleTex[i] = vec3(texture(texMaterial.diffuse[0], TexCoords.st + offsets[i]));
	}
	for(int i = 0; i < 9; i++)
		result += sampleTex[i] * kernel[i];
	return vec4(result, 1.0f);
}
vec4 getBlur(vec3 result) 
{
	const float offset = 1.0f / 300;
	vec2 offsets[9] = vec2[](
		vec2(-offset,	offset),
		vec2(0.0f,		offset),
		vec2(offset,	offset),
		vec2(-offset,	0.0f),
		vec2(0.0f,		0.0f),
		vec2(offset,	0.0f),
		vec2(-offset,	-offset),
		vec2(0.0f,		-offset),
		vec2(offset,	-offset)
	);
	float kernel[9] = float[](
		1.0 / 16,	2.0 / 16,	1.0 / 16,
		2.0 / 16,	4.0 / 16,	2.0 / 16,
		1.0 / 16,	2.0 / 16,	1.0 / 16
	);
	vec3 sampleTex[9];
	for(int i = 0; i < 9; i++)
	{
		sampleTex[i] = vec3(texture(texMaterial.diffuse[0], TexCoords.st + offsets[i]));
	}
	for(int i = 0; i < 9; i++)
		result += sampleTex[i] * kernel[i];
	return vec4(result, 1.0f);
}
vec4 getEdge(vec3 result) 
{
	const float offset = 1.0f / 300;
	vec2 offsets[9] = vec2[](
		vec2(-offset,	offset),
		vec2(0.0f,		offset),
		vec2(offset,	offset),
		vec2(-offset,	0.0f),
		vec2(0.0f,		0.0f),
		vec2(offset,	0.0f),
		vec2(-offset,	-offset),
		vec2(0.0f,		-offset),
		vec2(offset,	-offset)
	);
	float kernel[9] = float[](
		1.0,	1.0,	1.0,
		1.0,	-8.0,	1.0,
		1.0,	1.0,	1.0
	);
	vec3 sampleTex[9];
	for(int i = 0; i < 9; i++)
	{
		sampleTex[i] = vec3(texture(texMaterial.diffuse[0], TexCoords.st + offsets[i]));
	}
	for(int i = 0; i < 9; i++)
		result += sampleTex[i] * kernel[i];
	return vec4(result, 1.0f);
}
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, TexMaterial material)
{
	vec3 lightDir = normalize(-light.direction);
	//return lightDir;
    // Diffuse shading
	float diff = max(dot(normal, lightDir), 0.0f);
    // Specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	//return reflectDir;
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    // Combine results
	vec3 ambient = light.ambient  * vec3(texture(material.diffuse[0], TexCoords));
	//return ambient;
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse[0], TexCoords));
	diffuse += light.diffuse * diff * vec3(texture(material.diffuse[0], TexCoords));
	//return diffuse;
	vec3 specular = light.specular * spec * vec3(texture(material.specular[0], TexCoords));
	return (ambient + diffuse + specular) * light.color;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 position, vec3 viewDir, TexMaterial material)
{
    vec3 lightDir = normalize(light.position - position);
	//return lightDir;
	// Diffuse shading
	float diff = max(dot(normal, lightDir), 0.0f);
	// Specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	//return reflectDir;
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	// Attenuation
	float distance = length(light.position - position);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // Combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse[0], TexCoords));
	//return ambient;
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse[0], TexCoords));
	diffuse += light.diffuse * diff * vec3(texture(material.diffuse[0], TexCoords));
	//return diffuse;
	vec3 specular = light.specular * spec * vec3(texture(material.specular[0], TexCoords));
	//return specular;
    return (ambient + diffuse + specular) * attenuation * light.color;;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 position, vec3 viewDir, TexMaterial material)
{
	vec3 lightDir = normalize(light.position - position);
	//return lightDir;
	// Diffuse shading
	float diff = max(dot(normal, lightDir), 0.0f);
	// Specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	//return reflectDir;
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	// Attenuation
	float distance = length(light.position - position);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// Spotlight intensity
	float theta = dot(lightDir, normalize(-light.direction));
	if (theta > light.outerCutOff) {
		float epsilon = light.cutOff - light.outerCutOff;
		float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);
		// Combine result
		vec3 ambient = light.ambient * vec3(texture(material.diffuse[0], TexCoords));
		//return ambient;
		vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse[0], TexCoords));
		diffuse += light.diffuse * diff * vec3(texture(material.diffuse[0], TexCoords));
		//return diffuse;
		vec3 specular = light.specular * spec * vec3(texture(material.specular[0], TexCoords));
		//return specular;
		return (ambient + diffuse + specular) * attenuation * intensity * light.color;
	}
	return vec3(0.0f, 0.0f, 0.0f);
}
