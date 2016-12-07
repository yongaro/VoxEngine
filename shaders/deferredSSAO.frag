#version 430
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable


layout(binding = 0) uniform globalMatrices {
    mat4 view;
    mat4 proj;
	 vec3 camPos;
} globalMat;

#define SSAO_KERNEL_SIZE 64
layout(binding = 7) uniform SSAO_Kernel{
	vec4 kernel[SSAO_KERNEL_SIZE];
	vec2 screenDim;
} ssao_kernel;

#define POSITION 0
#define DIFFUSE 1
#define EMISSIVE 2
#define NORMALS 3
#define SPECULAR 4
#define NOISE 5
layout(binding = POSITION) uniform sampler2D positionTexSampler;
layout(binding = DIFFUSE) uniform sampler2D diffuseTexSampler;
layout(binding = EMISSIVE) uniform sampler2D emissiveTexSampler;
layout(binding = NORMALS) uniform sampler2D normalsTexSampler;
layout(binding = SPECULAR) uniform sampler2D specularTexSampler;
layout(binding = NOISE) uniform sampler2D noiseTexSampler;

layout(location = 0) in vec2 fragUV;

layout(location = 0) out vec4 fragColor;


const vec2 noiseScale = vec2( 640.0f/4.0f, 480.0f/4.0f );
const float radius = 1.0;

void main(){
	vec3 fragPos = texture(positionTexSampler, fragUV).xyz;
	vec3 fragNormal = texture(normalsTexSampler, fragUV).xyz;
	vec3 randomVec = texture(noiseTexSampler, fragUV * noiseScale).xyz;

	vec3 fragTangent = normalize(randomVec - fragNormal * dot(randomVec, fragNormal));
	vec3 fragBitangent = cross(fragNormal, fragTangent);
	mat3 TBN = mat3(fragTangent, fragBitangent, fragNormal);

	//iterate over the sample kernel and calculate occlusion factor
	float occlusion = 0.0;
	for( int i = 0; i < SSAO_KERNEL_SIZE; ++i ){
		vec3 SSAOsample = TBN * ssao_kernel.kernel[i].xyz; //conversion from tangent space
		SSAOsample = fragPos + SSAOsample * radius;

		//project sample position to sample texture to get position on screen/texture
		vec4 offset = vec4(SSAOsample,1.0);
		offset = globalMat.proj * offset;    //from view to clip space
		offset.xyz /= offset.w;              //perspective divide
		offset.xyz = offset.xyz * 0.5 + 0.5; //transform to range [0.0 ; 1.0]

		//get sample depth
		float sampleDepth = -texture(positionTexSampler, offset.xy).w; //get depth value of kernel sample

		//range check and accumulate
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth > SSAOsample.z ? 1.0 : 0.0) * rangeCheck;
	}
	occlusion = 1.0 - (occlusion / SSAO_KERNEL_SIZE);
	fragColor = vec4(occlusion, occlusion, occlusion, 1.0);
}
