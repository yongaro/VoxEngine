#version 430
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec4 fragPos;

layout(location = 0) out vec4 outColor;

void main(){
	//openGL do it anyway but just to be sure	
	gl_FragDepth = gl_FragCoord.z;
	outColor = vec4( 1.0, 0.5, 1.0, 1.0);
}  
