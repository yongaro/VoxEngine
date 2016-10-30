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

void glPipeline::loadShader(const char* vertexShaderFile, const char* fragmentShaderFile){
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
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

	std::cout << "Linking program" << std::endl;
	programID = glCreateProgram();
	glAttachShader(programID, vertShader);
	glAttachShader(programID, fragShader);

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

void glPipeline::generateShaders(const std::string vertex, const std::string fragment){
	loadShader( vertex.c_str(), fragment.c_str() );
}
