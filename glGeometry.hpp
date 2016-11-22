#ifndef GLGEOMETRY_HPP
#define GLGEOMETRY_HPP

#include "glPipeline.hpp"

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>


struct glVertex{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;
	glm::vec3 tangent;
	glm::vec3 bitangent;

	glVertex();
	glVertex(glm::vec3, glm::vec3, glm::vec2, glm::vec3, glm::vec3);
	glVertex(const glVertex&);
	~glVertex();
};





enum Textures{ DIFF, DISPLACEMENT, EMISSIVE, HEIGHT, NORMALS, SPECULAR, SIZE_T };
struct Features{
	glm::mat4 list;
	// Kd - disp - Ke - bump
	// Kn - Ks
	Features():list(){
		list[0][0] = -1.0f; list[0][1] = -1.0f; list[0][2] = -1.0f; list[0][3] = -1.0f;
		list[1][0] = -1.0f; list[1][1] = -1.0f;
	}
};


enum MaterialUniforms{ MATERIAL, FEATURES, SIZE_MU };
struct MaterialGroup{
	Material mat;
	Features features;
	GLuint textures[Textures::SIZE_T];
	GLuint UBO[MaterialUniforms::SIZE_MU];
	
	MaterialGroup();
	virtual ~MaterialGroup();

	void createTextureImage(const char*, uint32_t);
	void createUniformBuffers();
	void updateUniformBuffers();
	void bindUBO();
	void bindTextures();
};


enum VBO{ VERTEX, INDEX, SIZE_V };
struct glSubMesh{
	std::vector<glVertex> vertices;
	std::vector<GLuint> indices;

	MaterialGroup* mat;
	GLuint VAO;
	GLuint vbo[VBO::SIZE_V];
	
	glSubMesh();
	~glSubMesh();

	void createVertexBuffer();
	void createIndexBuffer();
	void createVAO();
	void bindVAO();
	void render();
	void testDraw(); //Use only with a compatibility openGL context
};




struct MeshTransforms{
	glm::mat4 model;
	glm::mat4 mvp;

	MeshTransforms():model(1.0f),mvp(1.0f){}
	void updateMVP(glm::mat4& proj, glm::mat4& view){ mvp = proj * view * model; }
};


class glMesh{
public:
	std::vector<glSubMesh*> subMeshes;
	std::vector<MaterialGroup*> materials;

	MeshTransforms matrices;
	GLuint UBO;
	
	
	glMesh();
	virtual ~glMesh();
	
	virtual void loadMesh(const std::string&, const std::string&);
	virtual void loadScene(const aiScene*, const std::string&);
	virtual void render();
	virtual void testDraw();
	virtual glm::vec3 getCamPos();

	virtual void createUniformBuffer();
	virtual void updateUniformBuffer();
	virtual void bindUBO();
	virtual void updateMVP(glm::mat4, glm::mat4);
};


struct InstanceInfos{
	glm::vec4 translate; //Faster than cpu-calculated model matrix for each instance
	//glm::vec4 lightColor;

	
	InstanceInfos():translate(){}//,lightColor(){}
	InstanceInfos(glm::vec4 tr):translate(tr){}//,lightColor(){}
	//InstanceInfos(glm::vec4 tr, glm::vec4 col):translate(tr),lightColor(col){}
};

struct InstanceOffset{
	GLuint value;
};

class glInstancedMesh : public glMesh{
public:
	std::vector< InstanceInfos > instances;
	GLuint instanceSSBO;
	GLuint maxNbInstances;

	GLuint ssboOffset;
	GLuint offsetUBO;
	bool commonSSBO;
	
	glInstancedMesh();
	virtual ~glInstancedMesh();
	virtual void createInstanceSSBO(GLuint);
	virtual void updateInstanceSSBO();
	virtual void bindSSBO();

	virtual void createOffsetUBO();
	virtual void updateOffsetUBO();
	virtual void bindOffsetUBO();

	virtual void addInstance(glm::vec4);
	virtual void render();
};


#endif
