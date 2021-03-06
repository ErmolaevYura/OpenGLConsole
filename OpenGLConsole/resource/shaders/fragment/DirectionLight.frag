#version 330

const vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float random(vec3 seed, int i);

struct DirectionalLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 color;
	vec3 direction;
	sampler2DShadow shadowMap; 
	mat4 lightMVP;
};

float getDirectionShadowFactor(DirectionalLight light, vec4 position, vec3 normal) {
    if(position.z > 1.0f) return 1.0f;
	float shadow = 1.0f;
	vec3 projCoords = (position.xyz / position.w + vec3(1.0)) * 0.5;	
	float bias = 0.005 * tan(acos(dot(normal, normalize(-light.direction))));
	bias = clamp(bias, 0, 0.004);
	for(int i = 0; i < 4; i++) {
		int index = int(16.0 * random(gl_FragCoord.xyy, i)) % 16;
        float closestDepth = shadow2D(light.shadowMap, projCoords.xyz + vec3(poissonDisk[index] / 700.0, -0.001)).r;
		if(closestDepth < projCoords.z - bias)
			shadow -= 0.2;
	}
	return shadow;
	//float closestDepth = texture(light.shadowMap, projCoords.xy).r;	
    //float closestDepth = shadow2D(light.shadowMap, projCoords.xyz, -0.001).r;
	//return (projCoords.z - bias) > closestDepth ? 0.0f : 1.0f;
}

vec3 getDirectionalLightFactor(DirectionalLight light, vec4 position, vec3 normal, vec2 texCoords, sampler2D texture) {
	return (light.ambient + 
			getDirectionShadowFactor(light, position, normal) *
			light.diffuse * 
			max(dot(normal, normalize(-light.direction)), 0.0f)) * 
			vec3(texture2D(texture, texCoords)) * 
			light.color;
}