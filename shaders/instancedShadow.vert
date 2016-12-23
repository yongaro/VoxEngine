#version 430
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0) uniform globalMatrices {
    mat4 view;
    mat4 proj;
	 vec3 camPos;
} globalMat;

layout(binding = 4) uniform MeshTranforms{
	mat4 model;
	mat4 mvp;
} meshTransforms;

layout(binding = 5) uniform LightSpace{
	mat4 matrix;
}lightSpace;

layout(binding = 6) uniform InstanceOffset{
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

layout(location = 0) out vec4 fragPos;


void main(){
	mat4 instanceModel = meshTransforms.model;
	instanceModel[3][0] += instanceSSBO.infos[gl_InstanceID+offset.value].translate.x;
	instanceModel[3][1] += instanceSSBO.infos[gl_InstanceID+offset.value].translate.y;
	instanceModel[3][2] += instanceSSBO.infos[gl_InstanceID+offset.value].translate.z;

	fragPos = lightSpace.matrix * instanceModel * vec4(vertPos, 1.0f);
	gl_Position = fragPos;
}  
