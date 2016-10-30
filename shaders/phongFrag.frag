#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#define max_lights 10

layout(binding = 0) uniform globalMatrices {
    mat4 view;
    mat4 proj;
	 vec3 camPos;
} globalMat;


layout(binding = 1) uniform lightSources{
	vec4 pos[max_lights];
	vec4 diffuse[max_lights];
	vec4 specular[max_lights];
	vec4 attenuation[max_lights]; //constant - linear - quadratic - spotExpoment
	vec4 spots[max_lights]; // xyz - spotCutoff
} lights;

layout(binding = 2) uniform Material{
	vec4 Ka;
	vec4 Kd;
	vec4 Ks;
	float shininess; 
} mat;

layout(binding = 3) uniform Features{ mat4 list; } features;

layout(binding = 4) uniform MeshTranforms{
	mat4 model;
	mat4 mvp;
} meshTransforms;


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


#define SHADOW 6
layout(binding = SHADOW) uniform sampler2D shadowTexSampler;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragUV;
layout(location = 3) in mat3 fragTBN; //takes slots 3,4,5
layout(location = 6) in vec4 lightSpaceFragPos;

layout(location = 0) out vec4 outColor;

float ShadowCalculation(vec4 fragPosLightSpace, float bias){
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0){
	    return 0.0;
    }
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowTexSampler, projCoords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check whether current frag pos is in shadow
	 float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0; 

    return shadow;
}

vec4 scene_ambient = vec4(0.01, 0.01, 0.01, 1.0);
float height_scale = 0.01;


vec3 surfacePos = fragPos;
vec2 fragTexCoord = vec2(fragUV);
vec3 normal = fragNormal;
vec4 surfaceColor = vec4(1.0, 1.0, 1.0, 1.0);

vec3 ApplyLight(int index, vec3 surfaceColor, vec3 N) {
	vec3 currentLightPos = lights.pos[index].xyz;
	vec3 L = normalize(currentLightPos - surfacePos.xyz);
	vec3 V = normalize(globalMat.camPos);
	vec3 R = reflect(-L, N);
	float attenuation = 1.0;

	
	if( lights.pos[index].w == 0.0 ){
		//directional light
		L = normalize( currentLightPos );
		attenuation = 1.0; //no attenuation for directional lights
	}
	else{
		//point light
		float distanceToLight = length(currentLightPos - surfacePos);
		attenuation = 1.0 / (1.0
		                     + lights.attenuation[index].x //constant
		                     + lights.attenuation[index].y * distanceToLight //linear
		                     + lights.attenuation[index].z * distanceToLight * distanceToLight); //quadratic
	}

	//ambient
	vec3 ambient = scene_ambient.rgb * mat.Ka.rgb;
	
	//diffuse
	vec3 diffuse = max(dot(N, L), 0.0) * surfaceColor.rgb * lights.diffuse[index].rgb * mat.Kd.rgb;
    
	//specular
	vec3 specular = pow(max(dot(R, V), 0.0), mat.shininess)  * lights.specular[index].rgb * mat.Ks.rgb;
	if( features.list[1][1] > 0.0 ){ specular *= texture(specularTexSampler, fragTexCoord).rgb; }
	//else{ specular *= mat.specular.rgb; }

	//linear color (color before gamma correction)
	//return ambient + attenuation*(diffuse + specular);
	
	float bias = max(0.05 * (1.0 - dot(N, L)), 0.005);
	float shadow = ShadowCalculation(lightSpaceFragPos,bias);
	return (ambient + (1.0 - shadow) * (diffuse + specular));
}


//steep parallax mapping
vec2 parallaxMapping(){
	vec3 tangentViewPos = fragTBN * globalMat.camPos;
	vec3 tangentFragPos = fragTBN * fragPos;
	vec3 viewDir = normalize(tangentViewPos - tangentFragPos);
	
	
	// number of depth layers
	const float minLayers = 64;
	const float maxLayers = 256;
	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
	//float numLayers = mix(maxLayers, minLayers, abs(dot(normalize(fragNormal), viewDir)));;
	// calculate the size of each layer
	float layerDepth = 1.0 / numLayers;
	// depth of current layer
	float currentLayerDepth = 0.0;
	// the amount to shift the texture coordinates per layer (from vector P)
	vec2 P = viewDir.xy * height_scale; 
	vec2 deltaTexCoords = P / numLayers;

	// get initial values
	vec2  currentTexCoords     = fragUV;
	float currentDepthMapValue = texture(heightTexSampler, currentTexCoords).r;
  
	while( currentLayerDepth < currentDepthMapValue ){
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = texture(heightTexSampler, currentTexCoords).r;
		// get depth of next layer
		currentLayerDepth += layerDepth;  
	}
	
	return currentTexCoords;
}   



void main() {
	//parallax mapping
	height_scale = 0.06;
	if( features.list[0][3] > 0.0 ){ fragTexCoord = parallaxMapping(); }
	
	//normal map
	vec3 tangentNormal = normalize( texture(normalsTexSampler, fragTexCoord).rgb * 2.0 - 1.0);
	vec3 normal = normalize(fragNormal);
	if( features.list[1][0] > 0.0 ){ normal =  normalize(fragTBN * tangentNormal); }

	//phong base informations
	vec3 surfaceToCamera = normalize(globalMat.camPos - surfacePos);

	//texture for diffuse lighting
	if( features.list[0][0] > 0.0 ){ surfaceColor = texture(diffuseTexSampler, fragTexCoord); }
	//else{ surfaceColor = vec4(1.0, 1.0, 1.0, 1.0); }
	
	//combine color from all the lights
	vec3 linearColor = vec3(0.0);
	if( features.list[0][2] > 0.0 ){ linearColor = texture(emissiveTexSampler, fragTexCoord).rgb; }
	else{
		for( int i = 0; i < max_lights; ++i ){
			if( lights.pos[i].w < 0.0 ){ continue; } //if light is used
			linearColor += ApplyLight(i, surfaceColor.rgb, normal);
		}
	}
	
	//final color with gamma correction
	vec3 gamma = vec3(1.0/2.2);
	outColor = vec4(pow(linearColor, gamma), surfaceColor.a);
	//outColor = vec4(linearColor,surfaceColor.a);
}
