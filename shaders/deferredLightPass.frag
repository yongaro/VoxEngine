#version 430
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable


layout(binding = 0) uniform globalMatrices {
    mat4 view;
    mat4 proj;
	 vec3 camPos;
} globalMat;

#define max_lights 10
layout(binding = 1) uniform lightSources{
	vec4 pos[max_lights];
	vec4 diffuse[max_lights];
	vec4 specular[max_lights];
	vec4 attenuation[max_lights]; //constant - linear - quadratic - spotExpoment
	vec4 spots[max_lights]; // xyz - spotCutoff
} lights;


layout(binding = 5) uniform Dummy{
	mat4 lightSpaceMatrix;
}dummy;

#define POSITION 0
#define DIFFUSE 1
#define EMISSIVE 2
#define NORMALS 3
#define SPECULAR 4
#define SSAO 5
layout(binding = POSITION) uniform sampler2D positionTexSampler;
layout(binding = DIFFUSE) uniform sampler2D diffuseTexSampler;
layout(binding = EMISSIVE) uniform sampler2D emissiveTexSampler;
layout(binding = NORMALS) uniform sampler2D normalsTexSampler;
layout(binding = SPECULAR) uniform sampler2D specularTexSampler;
layout(binding = SSAO) uniform sampler2D ssaoTexSampler;

#define SHADOW 6
layout(binding = SHADOW) uniform sampler2D shadowTexSampler;


layout(location = 0) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

vec4 fragPos      = texture(positionTexSampler, fragUV);
vec4 fragDiffuse  = texture(diffuseTexSampler, fragUV);
vec4 fragEmissive = texture(emissiveTexSampler, fragUV);
vec4 fragNormal   = texture(normalsTexSampler, fragUV);
vec4 fragSpecular = texture(specularTexSampler, fragUV) * vec4(0.5, 0.5, 0.5, 1.0);
vec3 fragToCamera = normalize(globalMat.camPos - fragPos.xyz);
vec4 ambiantOcclusion = texture(ssaoTexSampler, fragUV);
vec4 lightSpaceFragPos = dummy.lightSpaceMatrix * fragPos;

//vec4 lightSpaceFragPos = dummy.lightSpaceMatrix * fragPos;
//vec4 scene_ambient = vec4(0.3, 0.3, 0.3, 1.0) * fragDiffuse;




float ShadowCalculation(vec4 fragPosLightSpace, float bias){
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0){
	    return 0.0;
    }
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowTexSampler, projCoords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check whether current frag pos is in shadow
	 float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0; 

    return shadow;
}



vec3 ApplyLight(int index) {
	vec3 currentLightPos = lights.pos[index].xyz;
	vec3 L = normalize(currentLightPos - fragPos.xyz);
	vec3 V = normalize(globalMat.camPos);
	vec3 R = reflect(-L, fragNormal.xyz);
	float attenuation = 1.0;

	
	if( lights.pos[index].w == 0.0 ){
		//directional light
		L = normalize( currentLightPos );
		attenuation = 1.0; //no attenuation for directional lights
	}
	else{
		//point light
		float distanceToLight = length(currentLightPos - fragPos.xyz);
		attenuation = 1.0 / (1.0
		                     + lights.attenuation[index].x //constant
		                     + lights.attenuation[index].y * distanceToLight //linear
		                     + lights.attenuation[index].z * distanceToLight * distanceToLight); //quadratic
	}

	//ambient
	vec3 ambient = fragDiffuse.rgb * ambiantOcclusion.rgb;
	//diffuse
	vec3 diffuse = max(dot(fragNormal.xyz, L), 0.0) * fragDiffuse.rgb * lights.diffuse[index].rgb;
	//specular
	vec3 specular = pow(max(dot(R, V), 0.0), fragSpecular.w*25.0)  * lights.specular[index].rgb * fragSpecular.rgb;

	//linear color (color before gamma correction)
	return ambient + attenuation*(diffuse + specular);
	//return ambient + attenuation*(diffuse);

	//float bias = max(0.05 * (1.0 - dot(fragNormal.rgb, L)), 0.005);
	//float shadow = ShadowCalculation(lightSpaceFragPos,bias);
	//return (ambient + (1.0 - shadow) * (diffuse + specular));
}


void main(){
	if( fragPos.x == 0.0 && fragPos.y == 0.0 && fragPos.z == 0.0 ){ discard; }
	vec3 linearColor = vec3(0.0);
	if( fragEmissive.x != 0 && fragEmissive.y != 0 && fragEmissive.z != 0){ linearColor = fragEmissive.rgb; }
	else{
		for( int i = 0; i < max_lights; ++i ){
			if( lights.pos[i].w < 0.0 ){ continue; } //if light is used
			linearColor += ApplyLight(i);
		}
	}	
	//final color with gamma correction
	vec3 gamma = vec3(1.0/2.2);
	outColor = vec4(pow(linearColor, gamma), 1.0);
	//outColor = vec4(texture(shadowTexSampler, fragUV).r);
}
