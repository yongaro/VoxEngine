#version 430
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 fragColor;

#define SSAO 6
layout(binding = SSAO) uniform sampler2D ssaoTexSampler;


void main(){
	vec2 texelSize = 1.0 / vec2(textureSize(ssaoTexSampler, 0));
	float result = 0.0;
	for( int x = -2; x < 2; ++x ){
		for( int y = -2; y < 2; ++y ){
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(ssaoTexSampler, fragUV + offset).r;
		}
	}
	fragColor = vec4(result / (4.0 * 4.0), result / (4.0 * 4.0), result / (4.0 * 4.0), 1.0);
}
