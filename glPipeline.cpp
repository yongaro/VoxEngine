#include "glPipeline.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <random>

//######################### CLASSE GLPIPELINE ######################################
glPipeline::glPipeline(){}

glPipeline::~glPipeline() {
	for( unsigned int i = 0; i < ShadersIndex::SIZE_SI; ++i){
		glDetachShader(programID, shaders[i]);
		glDeleteShader(shaders[i]);
	}
	glDeleteProgram(programID);
}



void glPipeline::bind(){
	glUseProgram(programID);
}


std::string glPipeline::readFile(const char* fileName){
	std::string fileContent;
	std::ifstream fileStream(fileName, std::ios::in);
	if (!fileStream.is_open()) {
		printf("File %s not found\n", fileName);
		return "";
	}
	std::string line = "";
	while( !fileStream.eof() ){
		getline(fileStream, line);
		fileContent.append(line + "\n");
	}
	fileStream.close();
	return fileContent;
}


void glPipeline::printProgramLog(GLuint program){
	GLint result = GL_FALSE;
	int logLength;
	
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if( logLength > 0 ){
		GLchar* strInfoLog = new GLchar[logLength + 1];
		glGetProgramInfoLog(program, logLength, NULL, strInfoLog);
		printf("programlog: %s\n", strInfoLog);
	}
}

void glPipeline::printShaderLog(GLuint shader){
	GLint result = GL_FALSE;
	int logLength;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	if( logLength > 0 ){
		GLchar* strInfoLog = new GLchar[logLength + 1];
		glGetShaderInfoLog(shader, logLength, NULL, strInfoLog);
		printf("shaderlog: %s\n", strInfoLog);
	}
}

void glPipeline::generateShaders(const char* vertexShaderFile, const char* fragmentShaderFile, const char* geometryShaderFile){
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint geoShader = 0;
	if( geometryShaderFile != NULL ){ geoShader = glCreateShader(GL_GEOMETRY_SHADER); }
	
	// Read shaders
	std::string vertShaderStr = readFile(vertexShaderFile);
	std::string fragShaderStr = readFile(fragmentShaderFile);
	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	// Compile vertex shader
	std::cout << "Compiling vertex shader." << std::endl;
	glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
	glCompileShader(vertShader);
	printShaderLog(vertShader);
	// Compile fragment shader
	std::cout << "Compiling fragment shader." << std::endl;
	glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
	glCompileShader(fragShader);
	printShaderLog(fragShader);

	if( geometryShaderFile != NULL ){
		std::string geoShaderStr = readFile(geometryShaderFile);
		const char* geoShaderSrc = geoShaderStr.c_str();
		glShaderSource(geoShader, 1, &geoShaderSrc, NULL);
		glCompileShader(geoShader);
		printShaderLog(geoShader);
	}

	std::cout << "Linking program" << std::endl;
	programID = glCreateProgram();
	glAttachShader(programID, vertShader);
	glAttachShader(programID, fragShader);
	if( geometryShaderFile != NULL ){ glAttachShader(programID, geoShader); }

	// Bind vertex attributes to VBO indices
	glBindAttribLocation(programID, VertexAttributes::POS, "vertPos");
	glBindAttribLocation(programID, VertexAttributes::NRM, "vertNormal");
	glBindAttribLocation(programID, VertexAttributes::UV, "vertUV");
	glBindAttribLocation(programID, VertexAttributes::TANGENT, "vertTangent");
	glBindAttribLocation(programID, VertexAttributes::BITANGENT, "vertBiTangent");

	
	glLinkProgram(programID);
	printProgramLog(programID);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	glUseProgram(programID);
}



//####################################### CLASS GBUFFER ####################################
GBuffer::GBuffer(){}
GBuffer::~GBuffer(){}

bool GBuffer::init(GLuint windowWidth, GLuint windowHeight){
	//FBO creation
	glGenFramebuffers(1, &fboID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID);

	//GBuffer textures creation
	glGenTextures(GBuffer_Textures::SIZE_GBT, textures);
	glGenTextures(1, &depthTexture);

	for( GLuint i = 0; i < GBuffer_Textures::SIZE_GBT; ++i ){
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		if( i == GB_POS || i == GB_NRM ){ glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL); }
		else{ glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); }

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
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

void GBuffer::bindTextures(){
	for( size_t i = 0; i < GBuffer_Textures::SIZE_GBT; ++i ){
		glActiveTexture(GL_TEXTURE0+i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
	}
}

void GBuffer::initForGeometryPass(){
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void GBuffer::initForLightPass(){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.2f, 0.4f, 0.4f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	bindTextures();
	bind_Blurred_SSAO_Texture();
}



GLfloat lerp(GLfloat a, GLfloat b, GLfloat f){ return a + f * (b - a); }


void GBuffer::build_SSAO_Kernel(){
	//ssaoKernel.clear();
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // random floats between 0.0 - 1.0
	std::default_random_engine generator;

	//Creation of the ssao kernel
	for (GLuint i = 0; i < SSAO_KERNEL_SIZE; ++i){
		glm::vec3 sample( randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) );
		sample  = glm::normalize(sample);
		sample *= randomFloats(generator);
		GLfloat scale = GLfloat(i) / (GLfloat)SSAO_KERNEL_SIZE;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.kernel[i] = glm::vec4(sample, 1.0);
	}
	//passing ssao kernel to a Unifor Buffer Object
	glGenBuffers(1, &SSAO_UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, SSAO_UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(SSAO_Kernel), &ssaoKernel, GL_DYNAMIC_COPY);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	
	//creation of the ssao noise
	ssaoNoise.clear();
	for( GLuint i = 0; i < 16; ++i ){
		glm::vec3 noise( randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); 
		ssaoNoise.push_back(noise);
	}

	//Creation of the ssao noise texture
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGBA, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Creation of the ssao framebuffer
	glGenFramebuffers(1, &ssaoFBO);  
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

	//ssao framebuffer color attachment
	glGenTextures(1, &ssaoColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
	std::vector<GLenum> DrawBuffers;
	DrawBuffers.push_back(GL_COLOR_ATTACHMENT0);
	glDrawBuffers(1, DrawBuffers.data());

	
	//Creation of the blurred ssao ressources
	glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glGenTextures(1, &ssaoColorBufferBlur);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
	glDrawBuffers(1, DrawBuffers.data());
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::bind_SSAO_Kernel_UBO(){
	glBindBufferBase(GL_UNIFORM_BUFFER, UniformsBindingPoints::SSAO_KERNEL_UBP, SSAO_UBO);
}

void GBuffer::bind_SSAO_Noise(){
	glActiveTexture(GL_TEXTURE0+SIZE_GBT);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
}

void GBuffer::bind_SSAO_Texture(){
	glActiveTexture(GL_TEXTURE0+SIZE_GBT);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
}
void GBuffer::bind_Blurred_SSAO_Texture(){
	glActiveTexture(GL_TEXTURE0+SIZE_GBT);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
}

void GBuffer::initForSSAO(){
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	bindTextures();
	bind_SSAO_Noise();
}
void GBuffer::initForSSAOBlur(){
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	bindTextures();
	bind_SSAO_Texture();
}
