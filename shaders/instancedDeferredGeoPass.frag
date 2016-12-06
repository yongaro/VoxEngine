#version 430
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#define max_lights 10

layout(binding = 0) uniform globalMatrices {
    mat4 view;
    mat4 proj;
	 vec3 camPos;
} globalMat;

layout(binding = 2) uniform Material{
	vec4 Ka;
	vec4 Kd;
	vec4 Ks;
	float shininess; 
} mat;

layout(binding = 3) uniform Features{ mat4 list; } features;

layout(binding = 4) uniform MeshTranforms{
	mat4 model;
	mat4 mvp;
} meshTransforms;


//I NEED MOAR SYNTAXIK SUGAR
#define DIFFUSE 0
#define DISPLACEMENT 1
#define EMISSIVE 2
#define HEIGHT 3
#define NORMALS 4
#define SPECULAR 5
layout(binding = DIFFUSE) uniform sampler2D diffuseTexSampler;
layout(binding = DISPLACEMENT) uniform sampler2D displacementTexSampler;
layout(binding = EMISSIVE) uniform sampler2D emissiveTexSampler;
layout(binding = HEIGHT) uniform sampler2D heightTexSampler;
layout(binding = NORMALS) uniform sampler2D normalsTexSampler;
layout(binding = SPECULAR) uniform sampler2D specularTexSampler;


layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragUV;
layout(location = 3) in mat3 fragTBN; //takes slots 3,4,5

layout(location = 0) out vec3 outPos;
layout(location = 1) out vec4 outDiff;
layout(location = 2) out vec4 outEmissive;
layout(location = 3) out vec3 outNrm;
layout(location = 4) out vec4 outSpecular;



vec4 scene_ambient = vec4(0.01, 0.01, 0.01, 1.0);
float height_scale = 0.01;


vec2 fragTexCoord = vec2(fragUV);
vec3 normal = fragNormal;
vec4 fragDiffuse = vec4(1.0, 1.0, 1.0, 1.0);
vec4 fragEmissive = vec4(0.0, 0.0, 0.0, 0.0);
vec4 fragSpecular = vec4(1.0, 1.0, 1.0, 1.0);

//steep parallax mapping
vec2 parallaxMapping(){
	vec3 tangentViewPos = fragTBN * globalMat.camPos;
	vec3 tangentFragPos = fragTBN * fragPos;
	vec3 viewDir = normalize(tangentViewPos - tangentFragPos);
	
	
	// number of depth layers
	const float minLayers = 64;
	const float maxLayers = 256;
	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
	//float numLayers = mix(maxLayers, minLayers, abs(dot(normalize(fragNormal), viewDir)));;
	// calculate the size of each layer
	float layerDepth = 1.0 / numLayers;
	// depth of current layer
	float currentLayerDepth = 0.0;
	// the amount to shift the texture coordinates per layer (from vector P)
	vec2 P = viewDir.xy * height_scale; 
	vec2 deltaTexCoords = P / numLayers;

	// get initial values
	vec2  currentTexCoords     = fragUV;
	float currentDepthMapValue = texture(heightTexSampler, currentTexCoords).r;
  
	while( currentLayerDepth < currentDepthMapValue ){
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = texture(heightTexSampler, currentTexCoords).r;
		// get depth of next layer
		currentLayerDepth += layerDepth;  
	}
	return currentTexCoords;
}   


void main() {
	//parallax mapping
	height_scale = 0.06;
	if( features.list[0][3] > 0.0 ){ fragTexCoord = parallaxMapping(); }
	
	//normal map
	vec3 normal = normalize(fragNormal);
	if( features.list[1][0] > 0.0 ){
		vec3 tangentNormal = normalize( texture(normalsTexSampler, fragTexCoord).rgb * 2.0 - 1.0);
		normal =  normalize(fragTBN * tangentNormal);
	}
	
	//texture for diffuse lighting
	if( features.list[0][0] > 0.0 ){ fragDiffuse = texture(diffuseTexSampler, fragTexCoord); }
	fragDiffuse *= mat.Kd;
	//texture for emissive lighting
	if( features.list[0][2] > 0.0 ){ fragEmissive = texture(emissiveTexSampler, fragTexCoord); }
	//texture for specular lighting
	if( features.list[1][1] > 0.0 ){ fragSpecular = texture(specularTexSampler, fragTexCoord); }
	fragSpecular *= mat.Ks;
	fragSpecular.w = mat.shininess;
	
	//writing data to framebuffer attachments
	outPos      = fragPos;
	outDiff     = fragDiffuse;
	outEmissive = fragEmissive;
	outNrm      = normal;
	outSpecular = fragSpecular;
}
