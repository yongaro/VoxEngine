#version 430
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable


void main(){
	//openGL do it anyway but just to be sure	
	gl_FragDepth = gl_FragCoord.z;
}  
