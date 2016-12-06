#include "glPipeline.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>

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

void GBuffer::bindForWriting(){ glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID); }
void GBuffer::bindForReading(){  glBindFramebuffer(GL_READ_FRAMEBUFFER, fboID); }
void GBuffer::setReadBuffer(GBuffer_Textures texType){ glReadBuffer(GL_COLOR_ATTACHMENT0 + texType); }
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
}
