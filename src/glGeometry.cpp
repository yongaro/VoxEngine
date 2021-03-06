#include <glGeometry.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


//############################# STRUCT VERTEX #############################################################################
glVertex::glVertex():pos(),normal(),texCoord(),tangent(),bitangent(){}
glVertex::glVertex(glm::vec3 rPos, glm::vec3 nrm, glm::vec2 UV, glm::vec3 tgt, glm::vec3 bitgt):pos(rPos), normal(nrm), texCoord(UV), tangent(tgt), bitangent(bitgt){}
glVertex::glVertex(const glVertex& ref):glVertex(ref.pos, ref.normal, ref.texCoord, ref.tangent, ref.bitangent){}
glVertex::~glVertex(){}



//############################# STRUCT MATERIALGROUP  #####################################################################
MaterialGroup::MaterialGroup():mat(){}
MaterialGroup::~MaterialGroup(){}

void MaterialGroup::createTextureImage(const char* texturePath, uint32_t index){
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(texturePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	if( !pixels ){ throw std::runtime_error( stbi_failure_reason() ); }

	glGenTextures(1, &textures[index]);
	glBindTexture(GL_TEXTURE_2D, textures[index]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void MaterialGroup::createUniformBuffers(){
	glGenBuffers(1, &UBO[MaterialUniforms::MATERIAL]);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO[MaterialUniforms::MATERIAL]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialGroup), &mat, GL_DYNAMIC_COPY);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &UBO[MaterialUniforms::FEATURES]);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO[MaterialUniforms::FEATURES]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Features), &features, GL_DYNAMIC_COPY);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	updateUniformBuffers();
}

void MaterialGroup::updateUniformBuffers(){
	void* materialData;
	glBindBuffer(GL_UNIFORM_BUFFER, UBO[MaterialUniforms::MATERIAL]);
	materialData = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(materialData, &mat, sizeof(MaterialGroup));
	glUnmapBuffer(GL_UNIFORM_BUFFER);

	void* featuresData;
	glBindBuffer(GL_UNIFORM_BUFFER, UBO[MaterialUniforms::FEATURES]);
	featuresData = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(featuresData, &features, sizeof(Features));
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}


void MaterialGroup::bindUBO(){
	glBindBufferBase(GL_UNIFORM_BUFFER, UniformsBindingPoints::MATERIAL_UBP, UBO[MaterialUniforms::MATERIAL]);
	glBindBufferBase(GL_UNIFORM_BUFFER, UniformsBindingPoints::FEATURES_UBP, UBO[MaterialUniforms::FEATURES]);
}

void MaterialGroup::bindTextures(){
	for( unsigned int i = 0; i < Textures::SIZE_TEX; ++i ){
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
	}
}

//############################# STRUCT SUBMESH ###############################################################################
glSubMesh::glSubMesh():vertices(),indices(), mat(NULL){}

glSubMesh::~glSubMesh(){}


void glSubMesh::createVertexBuffer(){
	glGenBuffers(1, &vbo[VBO::VERTEX]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VERTEX]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glVertex), vertices.data(), GL_STATIC_DRAW);

	createIndexBuffer();
}



void glSubMesh::createIndexBuffer(){
	glGenBuffers(1, &vbo[VBO::INDEX]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[VBO::INDEX]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	createVAO();
}

void glSubMesh::createVAO(){
	glGenVertexArrays(1,&VAO);

	bindVAO();

	//Bind back to the default state
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
}

void glSubMesh::bindVAO(){
	glBindVertexArray(VAO);


	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VERTEX]);
	//position
	glVertexAttribPointer(VertexAttributes::POS, 3, GL_FLOAT, GL_FALSE, sizeof(glVertex), (void*)offsetof(glVertex,pos));
	glEnableVertexAttribArray(VertexAttributes::POS);

	//normal
	glVertexAttribPointer(VertexAttributes::NRM, 3, GL_FLOAT, GL_FALSE, sizeof(glVertex), (void*)offsetof(glVertex,normal));
	glEnableVertexAttribArray(VertexAttributes::NRM);

	//uv
	glVertexAttribPointer(VertexAttributes::UV, 2, GL_FLOAT, GL_FALSE, sizeof(glVertex), (void*)offsetof(glVertex,texCoord));
	glEnableVertexAttribArray(VertexAttributes::UV);

	//tangent
	glVertexAttribPointer(VertexAttributes::TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(glVertex), (void*)offsetof(glVertex,tangent));
	glEnableVertexAttribArray(VertexAttributes::TANGENT);

	//bitangent
	glVertexAttribPointer(VertexAttributes::BITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(glVertex), (void*)offsetof(glVertex,bitangent));
	glEnableVertexAttribArray(VertexAttributes::BITANGENT);
}

void glSubMesh::render(){
	mat->bindUBO();
	mat->bindTextures();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	bindVAO();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[VBO::INDEX]);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void glSubMesh::testDraw(){
	for( unsigned int i = 0; i < indices.size()-2; i+=3){
		glVertex& p1 = vertices.at( indices.at(i) );
		glVertex& p2 = vertices.at( indices.at(i+1) );
		glVertex& p3 = vertices.at( indices.at(i+2) );

		glBegin(GL_TRIANGLES);
		glNormal3f(p1.normal.x, p1.normal.y, p1.normal.z);
		glVertex3f(p1.pos.x, p1.pos.y, p1.pos.z);

		glNormal3f(p2.normal.x, p2.normal.y, p2.normal.z);
		glVertex3f(p2.pos.x, p2.pos.y, p2.pos.z);

		glNormal3f(p3.normal.x, p3.normal.y, p3.normal.z);
		glVertex3f(p3.pos.x, p3.pos.y, p3.pos.z);

		glEnd();
	}
}




//############################# STRUCT MESH ####################################################################################
glMesh::glMesh():subMeshes(),materials(){ createUniformBuffer(); }
glMesh::~glMesh(){
	for(unsigned int i = 0; i < subMeshes.size(); ++i){ delete subMeshes.at(i); }
	for(unsigned int i = 0; i < materials.size(); ++i){ delete materials.at(i); }
}


void glMesh::loadMesh(const std::string& assetPath, const std::string& fileName){
	std::string pFile = assetPath + fileName;
	// Create an instance of the Importer class
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile( pFile, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_GenNormals
                                            | aiProcess_RemoveRedundantMaterials  | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType );

  // If the import failed, report it
  if( !scene ){ throw std::runtime_error("failed to load Mesh with assimp!"); }
  // Now we can access the file's contents.
  loadScene( scene, assetPath );
}




void glMesh::loadScene( const aiScene* sc, const std::string& assetPath ){
	aiVector3D Zero3D(0.0f, 0.0f, 0.0f); //Default vector if component is not delivered by assimp

	//Texture and materials recovery
	if( sc->HasMaterials() ){
		for( unsigned int i = 0; i < sc->mNumMaterials; ++i ){
			MaterialGroup* matg = new MaterialGroup();
			const aiMaterial* material = sc->mMaterials[i];
			aiString texturePath;
			std::string path;

			aiString name;
			material->Get(AI_MATKEY_NAME,name);
			std::cout << "\e[1;33m New Material -- \e[0m"<< name.C_Str() << std::endl;

			if( material->GetTextureCount(aiTextureType_DIFFUSE) > 0 && material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS ){
				path = assetPath + std::string( texturePath.C_Str() );
				std::replace( path.begin(), path.end(), '\\', '/');
				std::cout << "\t texture DIFF \e[1;31m" << path.c_str() << "\e[0m"<< std::endl;
				matg->createTextureImage(path.c_str(), Textures::DIFF);
				matg->features.list[0][0] = 1.0f;
			}
			else{ matg->createTextureImage(DEFAULT_TEXTURE.c_str(), Textures::DIFF);  }

			if( material->GetTextureCount(aiTextureType_DISPLACEMENT) > 0 && material->GetTexture(aiTextureType_DISPLACEMENT, 0, &texturePath) == AI_SUCCESS ){
				path = assetPath + std::string( texturePath.C_Str() );
				std::replace( path.begin(), path.end(), '\\', '/');
				std::cout << "\t texture DISPLACEMENT \e[1;32m" << path.c_str() << "\e[0m"<< std::endl;
				matg->createTextureImage(path.c_str(), Textures::DISPLACEMENT);
				matg->features.list[0][1] = 1.0f;
			}
			else{ matg->createTextureImage(DEFAULT_TEXTURE.c_str(), Textures::DISPLACEMENT);  }

			if( material->GetTextureCount(aiTextureType_EMISSIVE) > 0 && material->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath) == AI_SUCCESS ){
				path = assetPath + std::string( texturePath.C_Str() );
				std::replace( path.begin(), path.end(), '\\', '/');
				std::cout << "\t texture EMISSIVE \e[1;33m" << path.c_str() << "\e[0m"<< std::endl;
				matg->createTextureImage(path.c_str(), Textures::EMISSIVE);
				matg->features.list[0][2] = 1.0f;
			}
			else{ matg->createTextureImage(DEFAULT_TEXTURE.c_str(), Textures::EMISSIVE);  }

			if( material->GetTextureCount(aiTextureType_HEIGHT) > 0 && material->GetTexture(aiTextureType_HEIGHT, 0, &texturePath) == AI_SUCCESS ){
				path = assetPath + std::string( texturePath.C_Str() );
				std::replace( path.begin(), path.end(), '\\', '/');
				std::cout << "\t texture HEIGHT \e[1;34m" << path.c_str() << "\e[0m"<< std::endl;
				matg->createTextureImage(path.c_str(), Textures::HEIGHT);
				matg->features.list[0][3] = 1.0f;
			}
			else{ matg->createTextureImage(DEFAULT_TEXTURE.c_str(), Textures::HEIGHT);  }

			if( material->GetTextureCount(aiTextureType_NORMALS) > 0 && material->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == AI_SUCCESS ){
				path = assetPath + std::string( texturePath.C_Str() );
				std::replace( path.begin(), path.end(), '\\', '/');
				std::cout << "\t texture NRM \e[1;35m" << path.c_str() << "\e[0m"<< std::endl;
				matg->createTextureImage(path.c_str(), Textures::NORMALS);
				matg->features.list[1][0] = 1.0f;
			}
			else{ matg->createTextureImage(DEFAULT_TEXTURE.c_str(), Textures::NORMALS);  }


			if( material->GetTextureCount(aiTextureType_SPECULAR) > 0 && material->GetTexture(aiTextureType_SPECULAR, 0, &texturePath) == AI_SUCCESS ){
				path = assetPath + std::string( texturePath.C_Str() );
				std::replace( path.begin(), path.end(), '\\', '/');
				std::cout << "\t texture SPEC \e[1;36m" << path.c_str() << "\e[0m"<< std::endl;
				matg->createTextureImage(path.c_str(), Textures::SPECULAR);
				matg->features.list[1][1] = 1.0f;
			}
			else{ matg->createTextureImage(DEFAULT_TEXTURE.c_str(), Textures::SPECULAR);  }


			//Material infos
			aiColor3D color (0.f,0.f,0.f);
			material->Get(AI_MATKEY_COLOR_AMBIENT,color);
			matg->mat.ambient = glm::vec4(color.r, color.g, color.b, 1.0f);
			std::cout << "\t ambient -- " << matg->mat.ambient.x << " " << matg->mat.ambient.y << " " << matg->mat.ambient.z << std::endl;
			material->Get(AI_MATKEY_COLOR_DIFFUSE,color);
			matg->mat.diffuse = glm::vec4(color.r, color.g, color.b, 1.0f);
			std::cout << "\t diffuse -- \e[1;31m" << matg->mat.diffuse.x << " " << matg->mat.diffuse.y << " " << matg->mat.diffuse.z << "\e[0m" << std::endl;
			material->Get(AI_MATKEY_COLOR_SPECULAR,color);
			matg->mat.specular = glm::vec4(color.r, color.g, color.b, 1.0f);
			std::cout << "\t specular -- \e[1;36m" << matg->mat.specular.x << " " << matg->mat.specular.y << " " << matg->mat.specular.z << "\e[0m" << std::endl;
			material->Get(AI_MATKEY_SHININESS,color);
			matg->mat.shininess = color.r / 4.0f; //divided by 4 because of an obj spec misunderstood in assimp
			std::cout << "\t shininess -- \e[1;38m" << matg->mat.shininess << "\e[0m" << std::endl;

			std::cout << std::endl;
			matg->createUniformBuffers();
			materials.push_back(matg);
		}
	}

	for( unsigned int i = 0; i < sc->mNumMeshes; ++i ){
		glSubMesh* subM_temp = new glSubMesh();
		//Recovering vertices data
		for( unsigned int j = 0; j < sc->mMeshes[i]->mNumVertices; ++j ){
			aiVector3D* pPos = &(sc->mMeshes[i]->mVertices[j]);
			aiVector3D* pNormal = &(sc->mMeshes[i]->mNormals[j]);
			aiVector3D* pTexCoord = (sc->mMeshes[i]->HasTextureCoords(0)) ? &(sc->mMeshes[i]->mTextureCoords[0][j]) : &Zero3D;
			aiVector3D* pTangent = (sc->mMeshes[i]->HasTangentsAndBitangents()) ? &(sc->mMeshes[i]->mTangents[j]) : &Zero3D;
			aiVector3D* pBiTangent = (sc->mMeshes[i]->HasTangentsAndBitangents()) ? &(sc->mMeshes[i]->mBitangents[j]) : &Zero3D;

			glVertex vertex_temp( glm::vec3(pPos->x, pPos->y, pPos->z),
			                      glm::vec3(pNormal->x, pNormal->y, pNormal->z),
			                      glm::vec2(pTexCoord->x , 1.0f - pTexCoord->y),
			                      glm::vec3(pTangent->x, pTangent->y, pTangent->z),
			                      glm::vec3(pBiTangent->x, pBiTangent->y, pBiTangent->z)
			                      );

			subM_temp->vertices.push_back(vertex_temp);
		}

		//Recovering vertices indices
		for( unsigned int j = 0; j < sc->mMeshes[i]->mNumFaces; ++j ){
			const aiFace& face = sc->mMeshes[i]->mFaces[j];
			if( face.mNumIndices != 3 ){ continue; }

			subM_temp->indices.push_back(face.mIndices[0]);
			subM_temp->indices.push_back(face.mIndices[1]);
			subM_temp->indices.push_back(face.mIndices[2]);
		}

		//Recovering material
		subM_temp->mat = materials.at( sc->mMeshes[i]->mMaterialIndex );
		subM_temp->createVertexBuffer();
		subM_temp->createIndexBuffer();
		subMeshes.push_back(subM_temp);
	}
}


void glMesh::render(){
	bindUBO();
	for(glSubMesh* subm : subMeshes){ subm->render(); }
}

void glMesh::testDraw(){
	for(glSubMesh* subm : subMeshes){ subm->testDraw(); }
}

glm::vec3 glMesh::getCamPos(){
	float axis[6];
	axis[0] = 0.0f; axis[1] = 0.0f;
	axis[2] = 0.0f; axis[3] = 0.0f;
	axis[4] = 0.0f; axis[5] = 0.0f;

	for(glSubMesh* subm : subMeshes){
		for(glVertex& v : subm->vertices){
			if( v.pos.x < axis[0] ){ axis[0] = v.pos.x; }
			if( v.pos.x > axis[1] ){ axis[1] = v.pos.x; }
			if( v.pos.y < axis[2] ){ axis[2] = v.pos.y; }
			if( v.pos.y > axis[3] ){ axis[3] = v.pos.y; }
			if( v.pos.z < axis[4] ){ axis[4] = v.pos.z; }
			if( v.pos.z > axis[5] ){ axis[5] = v.pos.z; }
		}
	}

	float distx = axis[1] - axis[0];
	float disty = axis[3] - axis[2];
	float distz = axis[5] - axis[4];

	return glm::vec3( distx, disty, distz  );
}




void glMesh::createUniformBuffer(){
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(MeshTransforms), &matrices, GL_DYNAMIC_COPY);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	updateUniformBuffer();
}



void glMesh::updateUniformBuffer(){
	void* data;
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	data = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(data, &matrices, sizeof(MeshTransforms));
	glUnmapBuffer(GL_UNIFORM_BUFFER);

}

void glMesh::bindUBO(){
	glBindBufferBase(GL_UNIFORM_BUFFER, UniformsBindingPoints::MESH_TRANS_UBP, UBO);
}


void glMesh::updateMVP(glm::mat4 proj, glm::mat4 view){
	matrices.updateMVP(proj,view);
	updateUniformBuffer();
}





//#################################### CLASS GLINSTANCEDMESH ####################################
glInstancedMesh::glInstancedMesh():glMesh(),instances(),instanceSSBO(),maxNbInstances(),ssboOffset(),offsetUBO(),commonSSBO(false){}
glInstancedMesh::~glInstancedMesh(){}

void glInstancedMesh::createInstanceSSBO(GLuint maxNb){
	maxNbInstances = maxNb;
	std::cout << "Allocation of \e[1;33m" << (GLfloat)maxNbInstances*sizeof(InstanceInfos)/1000000.0f << "\e[0m MB of vram"<< std::endl;
	instances.resize(maxNbInstances);
	glGenBuffers(1, &instanceSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(InstanceInfos)*maxNbInstances, instances.data(), GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	instances.clear();
}
void glInstancedMesh::updateInstanceSSBO(){
	if( instances.size() > 0 && instances.size() < maxNbInstances ){
		void* data;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceSSBO);
		data = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, ssboOffset, sizeof(InstanceInfos)*instances.size(), GL_MAP_WRITE_BIT);
		memcpy(data, instances.data(), sizeof(InstanceInfos)*instances.size());
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	}
}
void glInstancedMesh::bindSSBO(){
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ShaderStorageBindingPoints::INSTANCE_SSBP, instanceSSBO);
}


void glInstancedMesh::createOffsetUBO(){
	glGenBuffers(1, &offsetUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, offsetUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(GLuint), &ssboOffset, GL_DYNAMIC_COPY);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void glInstancedMesh::updateOffsetUBO(){
	void* data;
	glBindBuffer(GL_UNIFORM_BUFFER, offsetUBO);
	data = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(data, &ssboOffset, sizeof(GLuint));
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void glInstancedMesh::bindOffsetUBO(){
	glBindBufferBase(GL_UNIFORM_BUFFER, UniformsBindingPoints::OFFSET_UBP, offsetUBO);
}


void glInstancedMesh::addInstance(glm::vec4 tr){
	if( instances.size() < maxNbInstances ){
		instances.push_back( InstanceInfos(tr) );
	}
}
void glInstancedMesh::removeInstance(glm::vec4 tr){
	for( size_t i = 0; i < instances.size(); ++i ){
		if( tr.x == instances[i].translate.x && tr.y == instances[i].translate.y && tr.z == instances[i].translate.z ){
			instances.erase( instances.begin()+i );
			break;
		}
	}
}


void glInstancedMesh::render(){
	if( instances.size() > 0 ){
		bindUBO();
		bindSSBO();
		bindOffsetUBO();
		for( glSubMesh* smesh : subMeshes ){
			smesh->mat->bindUBO();
			smesh->mat->bindTextures();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			smesh->bindVAO();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, smesh->vbo[VBO::INDEX]);
			glDrawElementsInstanced(GL_TRIANGLES, smesh->indices.size(), GL_UNSIGNED_INT, 0, instances.size());

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER,0);
		}
	}
}




//####################################### CLASS DEFERRED RENDERER ##################################
DeferredLight::DeferredLight():pos( glm::vec4(0.0f, 0.0f, 0.0f, -1.0f) ),
                               diffuse( glm::vec4(1.2f, 1.2f, 1.2f, 1.2f) ),
                               specular( glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) ),
                               attenuation( glm::vec4(0.0f, 0.0f, 10.0f, 1.0f) ){}


glDeferredRenderer::glDeferredRenderer():width(),height(),gbuffer(),context(NULL),
                                         geometryPipeline(),lightPipeline(),lightVolumePipeline(),
                                         fullScreenQuad(),lightVolume(),
                                         max_deferred_lights(),lights(),deferredLightsSSBO(){}
glDeferredRenderer::glDeferredRenderer(GLuint w, GLuint h):width(w),height(h),gbuffer(),context(NULL),
                                                           geometryPipeline(),lightPipeline(),lightVolumePipeline(),
                                                           fullScreenQuad(),lightVolume(),
                                                           max_deferred_lights(),lights(),deferredLightsSSBO(){}
glDeferredRenderer::~glDeferredRenderer(){}

void glDeferredRenderer::init(glContext* ctx){
	//Geometry Pass
	std::string instancedDeferredGeoPass_vertex = "./shaders/instancedDeferredGeoPass.vert";
	std::string instancedDeferredGeoPass_fragment = "./shaders/instancedDeferredGeoPass.frag";
	std::cout << "\e[1;33mCompilation \e[1;36minstanced deferred rendering(Geometry pass) pipeline\e[0m" << std::endl;
	geometryPipeline.generateShaders(instancedDeferredGeoPass_vertex.c_str(), instancedDeferredGeoPass_fragment.c_str(), NULL);
	std::cout << "\e[1;32mDONE\e[0m" << std::endl;

	//Generic naive light pass pipeline
	std::string deferredLightPass_vertex = "./shaders/deferredLightPass.vert";
	std::string deferredLightPass_fragment = "./shaders/deferredLightPass.frag";
	std::cout << "\e[1;33mCompilation \e[1;36mdeferred rendering(Light pass) pipeline\e[0m" << std::endl;
	lightPipeline.generateShaders(deferredLightPass_vertex.c_str(), deferredLightPass_fragment.c_str(), NULL);
	std::cout << "\e[1;32mDONE\e[0m" << std::endl;

	//Light volume pipeline
	std::string deferredLight_vertex = "./shaders/instancedDeferredLight.vert";
	std::string deferredLight_fragment = "./shaders/instancedDeferredLight.frag";
	std::cout << "\e[1;33mCompilation \e[1;36mdeferred rendering(Light Volume) pipeline\e[0m" << std::endl;
	lightVolumePipeline.generateShaders(deferredLight_vertex.c_str(), deferredLight_fragment.c_str(), NULL);
	std::cout << "\e[1;32mDONE\e[0m" << std::endl;


	std::string deferredSSAO_vertex = "./shaders/deferredSSAO.vert";
	std::string deferredSSAO_fragment = "./shaders/deferredSSAO.frag";
	std::string SSAOBlur_vertex = "./shaders/SSAOBlur.vert";
	std::string SSAOBlur_fragment = "./shaders/SSAOBlur.frag";
	std::cout << "\e[1;33mCompilation \e[1;36mdeferred SSAO pipeline\e[0m" << std::endl;
	ssaoPipeline.generateShaders(deferredSSAO_vertex.c_str(), deferredSSAO_fragment.c_str(), NULL);
	std::cout << "\e[1;32mDONE\e[0m" << std::endl;
	std::cout << "\e[1;33mCompilation \e[1;36mdeferred SSAO Blur pipeline\e[0m" << std::endl;
	ssaoBlurPipeline.generateShaders(SSAOBlur_vertex.c_str(), SSAOBlur_fragment.c_str(), NULL);
	std::cout << "\e[1;32mDONE\e[0m" << std::endl;

	std::string instancedDeferredShadowPass_vertex = "./shaders/instancedShadow.vert";
	std::string instancedDeferredShadowPass_fragment = "./shaders/instancedShadow.frag";
	std::cout << "\e[1;33mCompilation \e[1;36minstanced deferred rendering(Shadow pass) pipeline\e[0m" << std::endl;
	shadowPipeline.generateShaders(instancedDeferredGeoPass_vertex.c_str(), instancedDeferredGeoPass_fragment.c_str(), NULL);
	std::cout << "\e[1;32mDONE\e[0m" << std::endl;


	gbuffer.init(width,height);
	context = ctx;

	max_deferred_lights = 250;
	fullScreenQuad = new glMesh();
	std::string path = "./assets/";
	std::string name = "fullscreenQuad.obj";
	fullScreenQuad->loadMesh(path,name);

	lightVolume = new glInstancedMesh();
	path = "./assets/lightVolumes/";
	name = "uvSphere.obj";
	lightVolume->loadMesh(path,name);
	lightVolume->matrices.model = glm::scale(lightVolume->matrices.model, glm::vec3(25.0f, 25.0f, 25.0f));
	lightVolume->updateUniformBuffer();
	lightVolume->ssboOffset = 0;
	lightVolume->createOffsetUBO();
	lightVolume->createInstanceSSBO(max_deferred_lights);


	gbuffer.build_SSAO_Kernel();

	lights.resize(max_deferred_lights);
	std::cout << "Allocation of \e[1;33m" << (GLfloat)max_deferred_lights*sizeof(DeferredLight)/1000000.0f << "\e[0m MB of vram"<< std::endl;
	glGenBuffers(1, &deferredLightsSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, deferredLightsSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(DeferredLight)*max_deferred_lights, lights.data(), GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
void glDeferredRenderer::bindGeometryPipeline(){
	gbuffer.initForGeometryPass();
	geometryPipeline.bind();
}
void glDeferredRenderer::bindLightPipeline(){
	gbuffer.initForLightPass();
	lightPipeline.bind();
}

void glDeferredRenderer::basicLightPass(){
	/*
	//glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	lightVolumePipeline.bind();
	context->bindUBO();
	bindLightSSBO();
	lightVolume->bindUBO();

	for(glSubMesh* subm : lightVolume->subMeshes){
		lightVolume->bindOffsetUBO();
		lightVolume->bindSSBO();
		subm->mat->bindUBO();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		subm->bindVAO();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subm->vbo[VBO::INDEX]);
		glDrawElementsInstanced(GL_TRIANGLES, subm->indices.size(), GL_UNSIGNED_INT, 0, lightVolume->instances.size());

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}
	*/

	lightPipeline.bind();
	bindLightSSBO();
	fullScreenQuad->bindUBO();
	for(glSubMesh* subm : fullScreenQuad->subMeshes){
		subm->mat->bindUBO();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		subm->bindVAO();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subm->vbo[VBO::INDEX]);
		glDrawElements(GL_TRIANGLES, subm->indices.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void glDeferredRenderer::ssaoPass(){
	gbuffer.initForSSAO();
	gbuffer.bind_SSAO_Kernel_UBO();
	ssaoPipeline.bind();

	fullScreenQuad->bindUBO();
	for(glSubMesh* subm : fullScreenQuad->subMeshes){
		subm->mat->bindUBO();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		subm->bindVAO();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subm->vbo[VBO::INDEX]);
		glDrawElements(GL_TRIANGLES, subm->indices.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}

	gbuffer.initForSSAOBlur();
	ssaoBlurPipeline.bind();

	fullScreenQuad->bindUBO();
	for(glSubMesh* subm : fullScreenQuad->subMeshes){
		subm->mat->bindUBO();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		subm->bindVAO();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subm->vbo[VBO::INDEX]);
		glDrawElements(GL_TRIANGLES, subm->indices.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}
}


void glDeferredRenderer::addLight(glm::vec4 pos){
	bool added = false;

	for( DeferredLight& l : lights ){
		if( l.pos.x == pos.x && l.pos.y == pos.y && l.pos.z == pos.z ){
			l.pos.w = 1.0f;
			added = true;
			break;
		}
		if( l.pos.w < 0.0f ){
			l.pos.x = pos.x; l.pos.y = pos.y; l.pos.z = pos.z;
			l.pos.w = pos.w;
			added = true;
			break;
		}
	}
	if( added ){
		update_Light_SSBO();
		lightVolume->addInstance(pos);
		lightVolume->updateInstanceSSBO();
	}
}
void glDeferredRenderer::removeLight(glm::vec4 pos){
	bool removed = false;
	size_t index = 0;

	for( DeferredLight& l : lights ){
		if( l.pos.x == pos.x && l.pos.y == pos.y && l.pos.z == pos.z ){
			l.pos.w = -1.0f;
			removed = true;
			break;
		}
		++index;
	}
	if( removed ){ update_Light_SSBO(); }
}

void glDeferredRenderer::update_Light_SSBO(){
	void* data;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, deferredLightsSSBO);
	data = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(DeferredLight)*lights.size(), GL_MAP_WRITE_BIT);
	memcpy(data, lights.data(), sizeof(DeferredLight)*lights.size());
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void glDeferredRenderer::bindLightSSBO(){ glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ShaderStorageBindingPoints::LIGHTS_SSBP, deferredLightsSSBO); }

void glDeferredRenderer::bindShadowPipeline(){
	gbuffer.initForShadowPass();
	shadowPipeline.bind();
}
