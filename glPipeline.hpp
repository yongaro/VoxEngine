#ifndef GLPIPELINE_HPP
#define GLPIPELINE_HPP

#include "glStructures.hpp"

#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>


#define max_lights 10

//##################### GLPIPELINE ##################################
enum UniformsBindingPoints{ GLOBAL_UBP, LIGHTS_UBP, MATERIAL_UBP, FEATURES_UBP, MESH_TRANS_UBP,
                            SHADOW_TRANS_UBP, OFFSET_UBP, SIZE_UBP };
enum ShaderStorageBindingPoints{ INSTANCE_SSBP, SIZE_SSBP};
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
};



/**
 * Structure used to regroup the framebuffer and textures for deferred shading
 */
enum GBuffer_Textures{ GB_POS, GB_DIFF, GB_EMISSIVE, GB_NRM, GB_SPECULAR, SIZE_GBT };
struct GBuffer{
public:
	GLuint fboID;
	GLuint textures[GBuffer_Textures::SIZE_GBT];
	GLuint depthTexture;

	GBuffer(){}
	~GBuffer(){}

	bool init(GLuint windowWidth, GLuint windowHeight){
		//FBO creation
		glGenFramebuffers(1, &fboID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID);

		//GBuffer textures creation
		glGenTextures(GBuffer_Textures::SIZE_GBT, textures);
		glGenTextures(1, &depthTexture);

		for( GLuint i = 0; i < GBuffer_Textures::SIZE_GBT; ++i ){
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);
		}

		//depth buffer
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

		//defining attachment for drawing
		std::vector<GLenum> DrawBuffers;
		for( size_t i = 0; i < GBuffer_Textures::SIZE_GBT; ++i ){
			DrawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
		}
		glDrawBuffers(GBuffer_Textures::SIZE_GBT, DrawBuffers.data());

		//final FBO check
		if( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ){
			std::cout << "\e[1;31mGBUFFER erreur creation FBO\e[0m" << std::endl;
			return false;
		}
		
		//restore default FBO
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		return true;
	}

	void bindForWrithing(){ glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID); }
	void bindForReading(){  glBindFramebuffer(GL_READ_FRAMEBUFFER, fboID); }
	void setReadBuffer(GBuffer_Textures texType){ glReadBuffer(GL_COLOR_ATTACHMENT0 + texType); }
	void bindForLightPass(){
		for( size_t i = 0; i < GBuffer_Textures::SIZE_GBT; ++i ){
			glActiveTexture(GL_TEXTURE0+i);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
		}
	}
};



#endif
