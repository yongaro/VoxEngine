#version 430
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#define GLOBAL_MATRIX_UBP 0
#define LIGHT_SOURCES_UBP 1
#define MATERIAL_UBP 2
#define FEATURES_UBP 3
#define MESH_TRANSFORMS_UBP 4
#define LIGHTSPACE_UBP 5
#define OFFSET_UBP 6

layout(binding = GLOBAL_MATRIX_UBP) uniform globalMatrices {
    mat4 view;
    mat4 proj;
	 vec3 camPos;
} globalMat;

#define max_lights 10
layout(binding = LIGHT_SOURCES_UBP) uniform lightSources{
	vec4 pos[max_lights];
	vec4 diffuse[max_lights];
	vec4 specular[max_lights];
	vec4 attenuation[max_lights]; //constant - linear - quadratic - spotExpoment
	vec4 spots[max_lights]; // xyz - spotCutoff
} lights;

layout(binding = MATERIAL_UBP) uniform Material{
	vec4 Ka;
	vec4 Kd;
	vec4 Ks;
	float shininess; 
} mat;

layout(binding = FEATURES_UBP) uniform Features{ mat4 list; } features;

layout(binding = MESH_TRANSFORMS_UBP) uniform MeshTranforms{
	mat4 model;
	mat4 mvp;
} meshTransforms;


layout(binding = LIGHTSPACE_UBP) uniform Dummy{
	mat4 lightSpaceMatrix;
}dummy;

layout(binding = OFFSET_UBP) uniform InstanceOffset{
	uint value;
} offset;

#define INSTANCE_DRAW_UBP 0
struct Instance{
	vec4 translate;
	//vec4 color;
};

layout (binding = INSTANCE_DRAW_UBP) buffer InstanceSSBO{
	Instance infos[];
} instanceSSBO;

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

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertUV;
layout(location = 3) in vec3 vertTangent;
layout(location = 4) in vec3 vertBiTangent;


layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragUV;
layout(location = 3) out mat3 fragTBN; //takes slots 3,4,5
//layout(location = 6) out vec4 lightSpaceFragPos;
//layout(location = 7) out vec4 fragColor;


void main() {
	//creating new model and mvp matrix based on instance offset
	mat4 instanceModel = meshTransforms.model;
	instanceModel[3][0] += instanceSSBO.infos[gl_InstanceID+offset.value].translate.x;
	instanceModel[3][1] += instanceSSBO.infos[gl_InstanceID+offset.value].translate.y;
	instanceModel[3][2] += instanceSSBO.infos[gl_InstanceID+offset.value].translate.z;
	mat4 instanceMVP = globalMat.proj * globalMat.view * instanceModel;
	
	gl_Position = instanceMVP * vec4(vertPos, 1.0);
	//displacement map -- pas terrible -- a refaire
	if( features.list[0][1] > 0.0 ){
		vec4 dv = 1.0 - texture( heightTexSampler, vertUV );
		float df =  0.30*dv.x + 0.59*dv.y + 0.11*dv.z;
		vec4 newVertexPos = vec4(vertNormal * df * 0.1, 0.0) + vec4(vertPos,1.0);
		gl_Position = instanceMVP * newVertexPos;
	}
	
	
	fragPos = vec3(instanceModel * vec4(vertPos ,1.0));
	fragNormal = mat3(transpose(inverse(instanceModel))) * vertNormal; 
	fragUV = vertUV;

	//vec3 T = normalize(vec3(instanceModel * vec4(vertTangent,   0.0)));
	//vec3 B = normalize(vec3(instanceModel * vec4(vertBiTangent, 0.0)));
	//vec3 N = normalize(vec3(instanceModel * vec4(vertNormal,    0.0)));
	//fragTBN = mat3(T, B, N);
	//lightSpaceFragPos = dummy.lightSpaceMatrix * vec4(fragPos,1.0);
	
	//fragColor =  instanceSSBO.infos[gl_InstanceID].color;
}
