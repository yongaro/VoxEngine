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


layout(binding = OFFSET_UBP) uniform InstanceOffset{
	uint value;
} offset;

#define INSTANCE_DRAW_UBP 0
struct Instance{
	vec4 translate;
};

layout (binding = INSTANCE_DRAW_UBP) buffer InstanceSSBO{
	Instance infos[];
} instanceSSBO;

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertUV;
layout(location = 3) in vec3 vertTangent;
layout(location = 4) in vec3 vertBiTangent;


layout(location = 0) out vec2 fragUV;

void main() {
	//creating new model and mvp matrix based on instance offset
	mat4 instanceModel = meshTransforms.model;
	instanceModel[3][0] += instanceSSBO.infos[gl_InstanceID+offset.value].translate.x;
	instanceModel[3][1] += instanceSSBO.infos[gl_InstanceID+offset.value].translate.y;
	instanceModel[3][2] += instanceSSBO.infos[gl_InstanceID+offset.value].translate.z;
	mat4 instanceMVP = globalMat.proj * globalMat.view * instanceModel;
	
	gl_Position = instanceMVP * vec4(vertPos, 1.0);
	
	fragUV = vertUV;
}
