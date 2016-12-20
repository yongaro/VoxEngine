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

layout(binding = MESH_TRANSFORMS_UBP) uniform MeshTranforms{
	mat4 model;
	mat4 mvp;
} meshTransforms;

layout(binding = OFFSET_UBP) uniform InstanceOffset{
	uint value;
} offset;

#define INSTANCE_DRAW_UBP 0
#define LIGHTS_SSBP 1

struct Instance{
	vec4 translate;
};

layout (binding = INSTANCE_DRAW_UBP) buffer InstanceSSBO{
	Instance infos[];
} instanceSSBO;


struct DeferredLight{
	vec4 pos;
	vec4 diffuse;
	vec4 specular;
	vec4 attenuation; 
};

layout (binding = LIGHTS_SSBP) buffer LightsSSBO{
	DeferredLight lights[];
} ssboLights;

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertUV;
layout(location = 3) in vec3 vertTangent;
layout(location = 4) in vec3 vertBiTangent;


layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec4 lightPos;
layout(location = 2) out vec4 lightDiffuse;
layout(location = 3) out vec4 lightSpecular;
layout(location = 4) out vec4 lightAttenuation;

void main() {
	//creating new model and mvp matrix based on instance offset
	mat4 instanceModel = meshTransforms.model;
	instanceModel[3][0] += ssboLights.lights[gl_InstanceID].pos.x;//instanceSSBO.infos[gl_InstanceID+offset.value].translate.x;
	instanceModel[3][1] += ssboLights.lights[gl_InstanceID].pos.y;//instanceSSBO.infos[gl_InstanceID+offset.value].translate.y;
	instanceModel[3][2] += ssboLights.lights[gl_InstanceID].pos.z;//instanceSSBO.infos[gl_InstanceID+offset.value].translate.z;
	mat4 instanceMVP = globalMat.proj * globalMat.view * instanceModel;
	
	gl_Position = instanceMVP * vec4(vertPos, 1.0);
	
	fragUV = vertUV;
	lightPos = ssboLights.lights[gl_InstanceID].pos;
	lightDiffuse = ssboLights.lights[gl_InstanceID].diffuse;
	lightSpecular = ssboLights.lights[gl_InstanceID].specular;
	lightAttenuation = ssboLights.lights[gl_InstanceID].attenuation;
}
