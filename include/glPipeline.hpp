#ifndef GLPIPELINE_HPP
#define GLPIPELINE_HPP

#include <glStructures.hpp>
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>


//##################### GLPIPELINE ##################################
enum UniformsBindingPoints{ GLOBAL_UBP, LIGHTS_UBP, MATERIAL_UBP, FEATURES_UBP, MESH_TRANS_UBP,
                            SHADOW_TRANS_UBP, OFFSET_UBP, SSAO_KERNEL_UBP, SIZE_UBP };
enum ShaderStorageBindingPoints{ INSTANCE_SSBP, LIGHTS_SSBP, SIZE_SSBP};
enum VertexAttributes{ POS, NRM, UV, TANGENT, BITANGENT, SIZE_VA };
enum GlobalUniforms{ GLOBAL_GU, LIGHTS_GU, SIZE_GU };
enum ShadersIndex{ VERTEX_SI, FRAG_SI, SIZE_SI };


struct glPipeline {
	GLuint programID;
	GLuint shaders[ShadersIndex::SIZE_SI];

	glPipeline();
	virtual ~glPipeline();
	void bind();

	std::string readFile(const char*);
	void printProgramLog(GLuint);
	void printShaderLog(GLuint);
	void generateShaders(const char*, const char*, const char*);

	//ssao
	void buildSSAOKernel();
};


/**
 * Structure containing the lights and necessary informations for the view
 */
struct glContext{
	UniformBufferObject globalUBO;
	LightSources lights;
	CamInfos camera;
	GLuint UBO[GlobalUniforms::SIZE_GU];

	glContext():globalUBO(), lights(), camera(){
		//Creation des uniform buffer objects et initialisation
		glGenBuffers(1, &UBO[GlobalUniforms::GLOBAL_GU]);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO[GlobalUniforms::GLOBAL_GU]);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBufferObject), &(globalUBO), GL_DYNAMIC_COPY );
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glGenBuffers(1, &UBO[GlobalUniforms::LIGHTS_GU]);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO[GlobalUniforms::LIGHTS_GU]);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(LightSources), &(lights), GL_DYNAMIC_COPY );
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void updateGlobalUniformBuffer(){
		void* globalData;
		glBindBuffer(GL_UNIFORM_BUFFER, UBO[GlobalUniforms::GLOBAL_GU]);
		globalData = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
		memcpy(globalData, &globalUBO, sizeof(UniformBufferObject));
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	}

	void updateLightsUniformBuffer(){
		void* lightsData;
		glBindBuffer(GL_UNIFORM_BUFFER, UBO[GlobalUniforms::LIGHTS_GU]);
		lightsData = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
		memcpy(lightsData, &lights, sizeof(LightSources));
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	}

	void bindUBO(){
		glBindBufferBase(GL_UNIFORM_BUFFER, UniformsBindingPoints::GLOBAL_UBP, UBO[UniformsBindingPoints::GLOBAL_UBP]);
		glBindBufferBase(GL_UNIFORM_BUFFER, UniformsBindingPoints::LIGHTS_UBP, UBO[UniformsBindingPoints::LIGHTS_UBP]);
	}

	void addLight(glm::vec4 pos){
		bool added = false;
		for( size_t i = 0; i < max_lights; ++i ){
			if( lights.pos[i].x == pos.x && lights.pos[i].y == pos.y && lights.pos[i].z == pos.z ){
				lights.pos[i].w = 1.0f;
				added = true;
				break;
			}
			if( lights.pos[i].w < 0.0f ){
				lights.pos[i].x = pos.x; lights.pos[i].y = pos.y; lights.pos[i].z = pos.z;
				lights.pos[i].w = 1.0f;
				added = true;
				break;
			}
		}
		if( added ){ updateLightsUniformBuffer(); }
	}

	void removeLight(glm::vec4 pos){
		bool removed = false;
		for( size_t i = 0; i < max_lights; ++i ){
			if( lights.pos[i].x == pos.x && lights.pos[i].y == pos.y && lights.pos[i].z == pos.z ){
				lights.pos[i].w = -1.0f;
				removed = true;
				//break;
			}
		}
		if( removed ){ updateLightsUniformBuffer(); }
	}
};





/**
 * Structure used to send an SSAO kernel to the GPU
 */
const GLuint SSAO_KERNEL_SIZE = 16;
struct SSAO_Kernel{
	glm::vec4 kernel[SSAO_KERNEL_SIZE];
	glm::vec4 screenDim;

	SSAO_Kernel(){
		for( GLuint i = 0; i < SSAO_KERNEL_SIZE; ++i ){
			kernel[i] = glm::vec4(0.0f);
		}
		screenDim = glm::vec4(800.0f, 600.0f, 1.0f, 1.0f);
	}
};


/**
 * Structure used to regroup the framebuffer and textures for deferred shading
 */
enum GBuffer_Textures{ GB_POS, GB_DIFF, GB_EMISSIVE, GB_NRM, GB_SPECULAR, GB_REAL_POS, SIZE_GBT };
enum DeferredRenderer_Textures{ DR_SSAO, DR_SHADOW, DR_SIZE };
struct GBuffer{
public:
	//classic GBuffer
	GLuint fboID;
	GLuint textures[GBuffer_Textures::SIZE_GBT];
	GLuint depthTexture;

	//SSAO structures
	SSAO_Kernel ssaoKernel;
	GLuint SSAO_UBO;

	std::vector<glm::vec3> ssaoNoise;
	GLuint noiseTexture;

	GLuint ssaoFBO;
	GLuint ssaoColorBuffer;
	GLuint ssaoBlurFBO;
	GLuint ssaoColorBufferBlur;

	//Shadow map structures
	GLuint shadowMapFBO;
	GLuint shadowMap;
	GLuint lightSpaceUBO;


	GBuffer();
	~GBuffer();

	bool init(GLuint, GLuint);
	void bindTextures();

	void initForGeometryPass();
	void initForLightPass();

	void build_SSAO_Kernel();
	void bind_SSAO_Kernel_UBO();
	void bind_SSAO_Noise();
	void bind_SSAO_Texture();
	void bind_Blurred_SSAO_Texture();

	void initForSSAO();
	void initForSSAOBlur();

	void initShadowMap(GLuint, GLuint);
	void initForShadowPass();
	void bindShadowMap();
};



#endif
