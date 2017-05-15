#ifndef GLSTRUCTURES_HPP
#define GLSTRUCTURES_HPP

#include <iostream>
#include <stdexcept>
#include <fstream>
#include <vector>
#include <cstring>
#include <array>
#define _USE_MATH_DEFINES
#include <cmath>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>


//Phong
#define max_lights 50
static uint32_t width = 800;
static uint32_t height = 600;
static std::string DEFAULT_TEXTURE = "./assets/default_texture.png";
//static float PI = 4.0f * std::atan(1.0f);


enum ModificationStates{ NONE, ROTATION, ORIENT_CAMERA, NORMAL, SPRINT, WALK };

//Uniforms structures
struct CamInfos{
	uint32_t modifMode;
	double clickedX;
	double clickedY;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 pos;
	glm::vec3 backupPos;
	glm::vec3 target;
	glm::mat4 rotateYaw;
	glm::mat4 rotatePitch;
	glm::vec3 targetToCam;
	uint32_t speed;

	CamInfos():modifMode(ModificationStates::NONE),
	           clickedX(), clickedY(),
	           up(0.0f, 1.0f, 0.0f), right(1.0f, 0.0f, 0.0f),
	           pos(), backupPos(), target(),
	           rotateYaw(1.0f), rotatePitch(1.0f),
	           targetToCam(),
	           speed(ModificationStates::NORMAL){}
};


struct UniformBufferObject {
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec3 camPos;


	UniformBufferObject():view( glm::lookAt(glm::vec3(2.5f, 2.5f, 2.5f),
	                                        glm::vec3(0.0f, 1.25f, 0.0f),
	                                        glm::vec3(0.0f, 1.0f, 0.0f))
	                            ),
	                      proj( glm::perspective(glm::radians(45.0f),
	                                             width / (float)height,
	                                             0.1f, 10.0f)
	                            ),
	                      camPos(){}

	void update(CamInfos& cam){
		camPos = cam.pos;
		view = glm::lookAt(camPos, cam.target, cam.up);
	}
};


struct LightSources{
	glm::vec4 pos[max_lights];
	glm::vec4 diffuse[max_lights];
	glm::vec4 specular[max_lights];
	glm::vec4 attenuation[max_lights]; //constant - linear - quadratic - spotExponent
	LightSources(){
		for( size_t i = 0; i < max_lights; ++i ){
			pos[i] = glm::vec4(0.0f, 0.0f, 0.0f, -1.0f);
			diffuse[i] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			specular[i] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			attenuation[i] = glm::vec4(10.0f, 1.0f, 0.0f, 1.0f);
		}
	}
};


struct ShadowInfos{
	glm::mat4 lightSpaceMatrix;
	glm::vec2 nearFarPlane;

	ShadowInfos():lightSpaceMatrix(1.0f),nearFarPlane(1.0f,15.0f){}
};

//Phong-global structures
struct Material{
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	float shininess;

	Material():ambient(0.8, 0.8, 0.8, 1.0),
	           diffuse(0.8, 0.8, 0.8, 1.0),
	           specular(0.8, 0.8, 0.8, 1.0),
	           shininess(16.0){}
};

#endif
