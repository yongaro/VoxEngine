#version 430
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertUV;
layout(location = 3) in vec3 vertTangent;
layout(location = 4) in vec3 vertBiTangent;

layout(location = 0) out vec2 fragUV;

void main(){
	gl_Position = vec4(vertPos,1.0);
	fragUV = vertUV;
}
