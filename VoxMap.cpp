#include "VoxMap.hpp"






void VoxMap::diffuseLight( int x, int y, int z, unsigned char light, std::vector<PixelCoord>& stack ){
	for( int i = -1; i < 2; ++i ){
		for( int j = -1; j < 2; ++j ){
			for( int k = -1; k < 2; ++k ){
				if( ((x+i >= 0 && y+j >= 0) && z+k >= 0) && ((x+i != map.width() && y+j != map.height()) && z+k != map.depth()) ){
					if( light > 0 && !tempMap(x+i, y+j, z+k) ){
						map(x+i, y+j, z+k, MapChannels::LIGHT) += light - 1;
						//map(x+i, y+j, z+k, MapChannels::LIGHT) = std::min(map(x+i, y+j, z+k, MapChannels::LIGHT), maxLightLevel);
						tempMap(x+i, y+j, z+k) = true;
						stack.push_back( PixelCoord(x+i,y+j,z+k) );
					}
				}
			}
		}
	}
}


VoxMap::VoxMap(){
	voxelSize[0] = 2.0f;
	voxelSize[1] = 2.0f;
	voxelSize[2] = 2.0f;
	maxLightLevel = 15;
	cubeColors[CubeTypes::ADMINIUM] = glm::vec4(0.0f,0.0f,0.0f,1.0f);
	cubeColors[CubeTypes::AIR] = glm::vec4(0.0f,0.0f,0.0f,0.1f);
	cubeColors[CubeTypes::DIRT] = glm::vec4(0.6078f,0.4627f,0.3254f,1.0f);
	cubeColors[CubeTypes::FOLLIAGE] = glm::vec4(0.133f,0.545f,0.133f,0.9f);
	cubeColors[CubeTypes::GRASS] = glm::vec4(0.420f,0.557f,0.137f,1.0f);
	cubeColors[CubeTypes::SAND] = glm::vec4(0.961f,0.871f,0.702f,1.0f);
	cubeColors[CubeTypes::SNOW] = glm::vec4(1.0f,1.0f,1.0f,1.0f);
	cubeColors[CubeTypes::STONE] = glm::vec4(0.663f,0.663f,0.663f,1.0f);
	cubeColors[CubeTypes::WATER] = glm::vec4(0.678f,0.847f,0.902f,0.8f);
	cubeColors[CubeTypes::WOOD] = glm::vec4(0.545f,0.271f,0.075f,1.0f);
}
VoxMap::~VoxMap(){}

void VoxMap::save(std::string& path){ map.save_analyze(path.c_str(),voxelSize); }
void VoxMap::load(std::string& path){  map.load_analyze(path.c_str(),voxelSize); }
void VoxMap::newMap(int width, int height, int depth){
	map = cimg_library::CImg<unsigned char>(width,height,depth,2,0x0);
	voxelSize[0] = 2.0f;
	voxelSize[1] = 2.0f;
	voxelSize[2] = 2.0f;
}

void VoxMap::testMap(){
	for( int z = 0; z < map.depth(); ++z ){
		for( int y = 0; y < map.height(); ++y ){
			for( int x = 0; x < map.width(); ++x ){
				if( (size_t)y < CubeTypes::SIZE_CT ){
				//if( y == 5 ){
					map(x,y,z,MapChannels::BLOC) = y;
					++cubes[y].nbInstances;
					cubes[y].instanceInfos.push_back( InstanceInfos(glm::vec4(x*voxelSize[0],y*voxelSize[1],z*voxelSize[2],1.0f),
					                                                cubeColors[y]
					                                                )
					                                  );
				}
				else{ map(x,y,z,MapChannels::BLOC) = 0x0; }
				map(x,y,z,MapChannels::LIGHT) = 0xFF;	
			}
		}
	}

	for( size_t i = 0; i < CubeTypes::SIZE_CT; ++i ){
		cubes[i].createInstanceSSBO();
	}
}

void VoxMap::render(){
	for( size_t i = 0; i < CubeTypes::SIZE_CT; ++i ){
		cubes[i].render();
	}
}

glm::vec3 VoxMap::getCamPos(){
	return glm::vec3( map.width(), map.height(), map.depth() );
}

void VoxMap::loadVoxel(std::string& path, std::string& name){
	for( size_t i = 0; i < CubeTypes::SIZE_CT; ++i ){
		cubes[i].loadMesh(path,name);
	}
}
