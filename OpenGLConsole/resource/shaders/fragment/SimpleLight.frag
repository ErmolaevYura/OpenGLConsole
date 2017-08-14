#version 330

in VertexFS
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
 //   vec4 SunLightMVP;
	vec4 PointLightMVP;
} vert;

struct FogParameters { vec4 color; float start; float end; float density; int equation; };
float getFogFactor(FogParameters params, float FogCoord);

struct DirectionalLight { vec3 ambient; vec3 diffuse; vec3 color; vec3 direction; sampler2DShadow shadowMap; mat4 lightMVP; };
vec3 getDirectionalLightFactor(DirectionalLight light, vec4 position, vec3 normal, vec2 texCoords, sampler2D texture);
float getDirectionShadowFactor(DirectionalLight light, vec4 position, vec3 normal);

struct PointLight { vec3 diffuse; vec3 specular; vec3 color; vec3 position; float constant; float quadratic; float linear; sampler2DShadow shadowMap; mat4 lightMVP; };
vec3 getPointLight(PointLight light, vec4 positionByMVP, vec3 position, vec3 normal, vec3 viewDirection, vec2 texCoords, sampler2D diffTex, sampler2D specTex, float shininess);
float getPointShadowFactor(PointLight light, vec4 position, vec3 normal);

struct SpotLight { vec3 diffuse; vec3 specular; vec3 color; vec3 position; vec3 direction; float cutOff; float outerCutOff; float constant; float quadratic; float linear; sampler2DShadow shadowMap; mat4 lightMVP; };
vec3 getSpotLight(SpotLight light, vec4 positionByLight, vec3 position, vec3 normal, vec3 viewDirection, vec2 texCoords, sampler2D diffTex, sampler2D specTex, float shininess);
float getSpotShadowFactor(SpotLight light, vec4 position, vec3 normal);

uniform FogParameters fog;
uniform vec3 viewPosition;
uniform sampler2D diffuse;
uniform sampler2D specular;
//uniform DirectionalLight sun;
uniform PointLight pointLight;
//uniform SpotLight spotLight;
uniform bool fogOn = true;
uniform bool spotOn = true;
uniform bool depthOn = false;
uniform bool shadowOn = false;
uniform float gamma = 5.2f;

vec3 getDepth(vec3 result);
void main() {
	vec3 normal = normalize(vert.Normal);
	vec3 viewDirection = normalize(viewPosition - vert.Position);
	//vec3 directional = getDirectionalLightFactor(sun, vert.SunLightMVP, normal, vert.TexCoords, diffuse);
	vec3 ptLight = getPointLight(pointLight, vert.PointLightMVP, vert.Position, normal, viewDirection, vert.TexCoords, diffuse, specular, 0.5f);
	//vec3 spot;
	//if(spotOn) spot = getSpotLight(spotLight, vert.SpotPointLightMVP, vert.Position, normal, viewDirection, vert.TexCoords, diffuse, specular,  0.5f);
	//else spot = vec3(0.0f, 0.0f, 0.0f);
	if(depthOn) {
		gl_FragColor =  vec4(getDepth(vec3(0.05f)), 1.0f);
	}
	else {
		if(shadowOn) {
			//gl_FragColor = vec4(vec3(getDirectionShadowFactor(sun, vert.SunLightMVP, normal)), 1.0f);
			gl_FragColor = vec4(vec3(getPointShadowFactor(pointLight, vert.PointLightMVP, normal)), 1.0f);
		}
		else {
			if(fogOn) {
				float fogFactor = getFogFactor(fog, gl_FragCoord.z / gl_FragCoord.w);
				gl_FragColor =  mix(vec4(ptLight, 1.0f), fog.color, fogFactor);
			} else {
				gl_FragColor =  vec4(ptLight, 1.0f);
			}
		}
	}
	if(gl_FrontFacing)
		gl_FragColor.a = 0.5f;
	gl_FragColor.rgb = pow(gl_FragColor.rgb, vec3(1.0f/gamma));
}
vec3 getDepth(vec3 result) 
{
	float near = 1.0f;
	float far = 100.0f;
    float z = gl_FragCoord.z * 2.0f - 1.0f;
    float depth = (2.0f * near * far) / (far + near - z * (far - near)) / far;
    return vec3(result / vec3(depth));
}