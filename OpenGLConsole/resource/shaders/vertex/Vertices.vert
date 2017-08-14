#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoords;

struct DirectionalLight { vec3 ambient; vec3 diffuse; vec3 color; vec3 direction; sampler2DShadow shadowMap;  mat4 lightMVP; };
struct PointLight { vec3 diffuse; vec3 specular; vec3 color; vec3 position;  float constant; float quadratic; float linear; sampler2DShadow shadowMap;  mat4 lightMVP; };
struct SpotLight { vec3 diffuse; vec3 specular; vec3 color; vec3 position; vec3 direction; float cutOff; float outerCutOff; float constant; float quadratic; float linear; sampler2DShadow shadowMap;  mat4 lightMVP; };

uniform mat4 vp;
uniform mat4 model;
//uniform DirectionalLight sun;
uniform PointLight pointLight;

// to fragment shader
out VertexFS
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
  //  vec4 SunLightMVP;
	vec4 PointLightMVP;
} vertfs;

void main(){
    gl_Position =  vp * model * vec4(position, 1.0f);
    vertfs.Position = vec3(model * vec4(position, 1.0f));
	vertfs.Normal = transpose(inverse(mat3(model))) * normal;
	vertfs.TexCoords = texcoords;
	//vertfs.SunLightMVP = sun.lightMVP * vec4(vertfs.Position, 1.0f);
	vertfs.PointLightMVP = pointLight.lightMVP * vec4(vertfs.Position, 1.0f);
}
