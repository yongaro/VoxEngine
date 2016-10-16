#ifndef GLPIPELINE_HPP
#define GLPIPELINE_HPP

#include "glStructures.hpp"

#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>


#define max_lights 10

//##################### GLPIPELINE ##################################
enum UniformsBindingPoints{ GLOBAL_UBP, LIGHTS_UBP, MATERIAL_UBP, FEATURES_UBP, MESH_TRANS_UBP, SIZE_UBP };
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
	void loadShader(const char*, const char*);
	void printProgramLog(GLuint);
	void printShaderLog(GLuint);
	void generateShaders();
};


//A renommer
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


#endif
