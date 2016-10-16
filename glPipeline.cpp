#include "glPipeline.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>

//######################### CLASSE GLPIPELINE ######################################
glPipeline::glPipeline(){}
glPipeline::glPipeline(std::string& vertex, std::string& fragment){
	programID = glCreateProgram();
	
	//Those 2 shaders are needed to build a pipeline
	createShader(vertex, GL_VERTEX_SHADER);
	createShader(fragment, GL_FRAGMENT_SHADER);

	glBindAttribLocation(programID, VertexAttributes::POS, "vertPos");
	glBindAttribLocation(programID, VertexAttributes::NRM, "vertNormal");
	glBindAttribLocation(programID, VertexAttributes::TANGENT, "vertTangent");
	glBindAttribLocation(programID, VertexAttributes::BITANGENT, "vertBiTangent");
	glBindAttribLocation(programID, VertexAttributes::UV, "vertUV");

	//Creating uniform buffers binding points
	//bindingPoints[UniformsBindingPoints::GLOBAL_UBP] = glGetUniformBlockIndex(programID, "globalMat");
	//bindingPoints[UniformsBindingPoints::LIGHTS_UBP] = glGetUniformBlockIndex(programID, "lights");
	//bindingPoints[UniformsBindingPoints::MATERIAL_UBP] = glGetUniformBlockIndex(programID, "mat");
	//bindingPoints[UniformsBindingPoints::FEATURES_UBP] = glGetUniformBlockIndex(programID, "features");
	//bindingPoints[UniformsBindingPoints::MESH_TRANS_UBP] = glGetUniformBlockIndex(programID, "meshTransforms");
	
	//final linking
	link();

	
}

glPipeline::~glPipeline() {
	for( unsigned int i = 0; i < ShadersIndex::SIZE_SI; ++i){
		glDetachShader(programID, shaders[i]);
		glDeleteShader(shaders[i]);
	}
	glDeleteProgram(programID);
}


void glPipeline::attachShaders(){
	for( unsigned int i = 0; i < ShadersIndex::SIZE_SI; ++i){
		glAttachShader(programID, shaders[i]);
	}
}

void glPipeline::bind(){
	glUseProgram(programID);
	
}
void glPipeline::link(){
	glLinkProgram(programID);
	checkShaderError(programID, GL_LINK_STATUS, true, "Error linking shader program");

	glValidateProgram(programID);
	checkShaderError(programID, GL_LINK_STATUS, true, "Invalid shader program");
}
	
std::string glPipeline::loadShaderCode(const std::string& fileName){
	std::ifstream file;
	file.open((fileName).c_str());
	
	std::string output;
	std::string line;
	if(file.is_open()){
		while(file.good()){
			getline(file, line);
			output.append(line + "\n");
		}
	}
	else{ std::cerr << "Unable to load shader: " << fileName << std::endl; }
	
	file.close();
	return output;
}


void glPipeline::checkShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage){
	GLint success = 0;
	GLchar error[1024] = { 0 };
	
	if(isProgram){ glGetProgramiv(shader, flag, &success); }
	else{ glGetShaderiv(shader, flag, &success); }

	if(success == GL_FALSE){
		if(isProgram){ glGetProgramInfoLog(shader, sizeof(error), NULL, error); }
		else{ glGetShaderInfoLog(shader, sizeof(error), NULL, error); }
		
		std::cerr << errorMessage << ": '" << error << "'" << std::endl;
	}
}

void glPipeline::createShader(const std::string& path, unsigned int type){
	GLuint shader = glCreateShader(type);
	if(shader == 0){ std::cerr << "Error compiling shader type " << type << std::endl; }

	//loading GLSL code from file
	const std::string& code = loadShaderCode(path);
	const GLchar* p[1];
	p[0] = code.c_str();
	GLint lengths[1];
	lengths[0] = code.length();
	
	//GLSL code compilation
	glShaderSource(shader, 1, p, lengths);
	glCompileShader(shader);
	checkShaderError(shader, GL_COMPILE_STATUS, false, "Error compiling shader");

	//binding shader to the corresponding pipeline slot
	if( type == GL_VERTEX_SHADER ){ shaders[ShadersIndex::VERTEX_SI] = shader; }
	if( type == GL_FRAGMENT_SHADER ){ shaders[ShadersIndex::FRAG_SI] = shader; }
	glAttachShader(programID, shader);
}


std::string glPipeline::readFile(const char* fileName) {
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

GLuint glPipeline::loadShader(const char* vertexShaderFile, const char* fragmentShaderFile){
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

	//bindingPoints[UniformsBindingPoints::GLOBAL_UBP] = glGetUniformBlockIndex(programID, "globalMat");
	//bindingPoints[UniformsBindingPoints::LIGHTS_UBP] = glGetUniformBlockIndex(programID, "lights");
	//bindingPoints[UniformsBindingPoints::MATERIAL_UBP] = glGetUniformBlockIndex(programID, "mat");
	//bindingPoints[UniformsBindingPoints::FEATURES_UBP] = glGetUniformBlockIndex(programID, "features");
	//bindingPoints[UniformsBindingPoints::MESH_TRANS_UBP] = glGetUniformBlockIndex(programID, "meshTransforms");

	//glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
	//glBindBufferBase(GL_UNIFORM_BUFFER, 1, UBOInst);

	glUseProgram(programID);

	return programID;
}

void glPipeline::generateShaders(){ programID = loadShader("./shaders/phongVert.vert", "./shaders/phongFrag.frag"); }
