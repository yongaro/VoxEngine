#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable


layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertUV;
layout(location = 3) in vec3 vertTangent;
layout(location = 4) in vec3 vertBiTangent;

layout(binding = 4) uniform MeshTranforms{
	mat4 model;
	mat4 mvp;
} meshTransforms;

layout(binding = 5) uniform Dummy{
	mat4 lightSpaceMatrix;
}dummy;

void main(){
    gl_Position = dummy.lightSpaceMatrix * meshTransforms.model * vec4(vertPos, 1.0f);
}  
