#include "VoxMap.hpp"


VoxMap::VoxMap(){
	voxelSize[0] = 2.0f;
	voxelSize[1] = 2.0f;
	voxelSize[2] = 2.0f;
	maxLightLevel = 15;
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

cimg_library::CImg<bool> VoxMap::getMapObjects(){
	cimg_library::CImg<bool> res = cimg_library::CImg<bool>(map.width(), map.height(), map.depth(), 1, false);

	for( int z = 0; z < map.depth(); ++z ){
		for( int y = 0; y < map.height(); ++y ){
			for( int x = 0; x < map.width(); ++x ){
				if( map(x,y,z,MapChannels::BLOC) != CubeTypes::AIR ){
					res(x,y,z) = true;
				}
			}
		}
	}
	return res;
}

void VoxMap::getMapOutline(cimg_library::CImg<bool>& dil, cimg_library::CImg<bool>& ero){
	for( int z = 0; z < map.depth(); ++z ){
		for( int y = 0; y < map.height(); ++y ){
			for( int x = 0; x < map.width(); ++x ){
				if( dil(x,y,z) && !ero(x,y,z) ){
					if( map(x,y,z, MapChannels::BLOC) != CubeTypes::AIR ){
						cubes[ map(x,y,z, MapChannels::BLOC) ].addInstance( glm::vec4(x*voxelSize[0], y*voxelSize[1], z*voxelSize[2], 1.0f) );
					}
				}
			}
		}
	}
	/*
	fillOffsets();
	for( size_t i = 0; i < CubeTypes::SIZE_CT; ++i ){
		cubes[i].updateInstanceSSBO();
	}
	*/
	fillSSBO();
}


void VoxMap::testMap(){
	createInstanceSSBO();
	for( size_t i = 0; i < CubeTypes::SIZE_CT; ++i ){
		cubes[i].maxNbInstances = map.width()*map.height()*map.depth();
		cubes[i].instanceSSBO = mapSSBO;
		cubes[i].createOffsetUBO();
	}
	for( int z = 1; z < map.depth()-1; ++z ){
		for( int y = 1; y < map.height()-1; ++y ){
			for( int x = 1; x < map.width()-1; ++x ){
				if( (size_t)y < CubeTypes::SIZE_CT ){
					map(x,y,z,MapChannels::BLOC) = y;
				}
				else{ map(x,y,z,MapChannels::BLOC) = CubeTypes::AIR; }
				map(x,y,z,MapChannels::LIGHT) = 0xFF;	
			}
		}
	}

	cimg_library::CImg<bool> mapObjects = getMapObjects();
	cimg_library::CImg<bool> dilate = mapObjects.get_dilate(3);
	cimg_library::CImg<bool> erode = mapObjects.get_erode(3);
	getMapOutline(dilate,erode);
}

void VoxMap::render(){
	for( size_t i = 1; i < CubeTypes::SIZE_CT; ++i ){
		cubes[i].render();
	}
}

glm::vec3 VoxMap::getCamPos(){
	return glm::vec3( map.width(), map.height(), map.depth() );
}

void VoxMap::loadVoxel(std::string& path, std::string& name){
	std::vector< std::string > cubesPath;
	std::string cubeName = "cube.obj";
	cubesPath.push_back("./assets/cubes/");
	cubesPath.push_back("./assets/cubes/adminium/");
	cubesPath.push_back("./assets/cubes/dirt/");
	cubesPath.push_back("./assets/cubes/folliage/");
	cubesPath.push_back("./assets/cubes/glowstone/");
	cubesPath.push_back("./assets/cubes/grass/");
	cubesPath.push_back("./assets/cubes/sand/");
	cubesPath.push_back("./assets/cubes/snow/");
	cubesPath.push_back("./assets/cubes/stone/");
	cubesPath.push_back("./assets/cubes/water/");
	cubesPath.push_back("./assets/cubes/wood/");

	cubes[0].loadMesh(path,name);
	for( size_t i = 1; i < CubeTypes::SIZE_CT; ++i ){
		cubes[i].loadMesh(cubesPath.at(i),cubeName);
	}
}


void VoxMap::diffuseLight( int x, int y, int z, unsigned char light, std::vector<PixelCoord>& stack ){
	for( int i = -1; i < 2; ++i ){
		for( int j = -1; j < 2; ++j ){
			for( int k = -1; k < 2; ++k ){
				if( ((x+i >= 0 && y+j >= 0) && z+k >= 0) && ((x+i != map.width() && y+j != map.height()) && z+k != map.depth()) ){
					if( light > 0 && !tempMap(x+i, y+j, z+k) ){
						map(x+i, y+j, z+k, MapChannels::LIGHT) += light - 1;
						tempMap(x+i, y+j, z+k) = true;
						stack.push_back( PixelCoord(x+i,y+j,z+k) );
					}
				}
			}
		}
	}
}

void VoxMap::undiffuseLight( int x, int y, int z, unsigned char light, std::vector<PixelCoord>& stack ){
	for( int i = -1; i < 2; ++i ){
		for( int j = -1; j < 2; ++j ){
			for( int k = -1; k < 2; ++k ){
				if( ((x+i >= 0 && y+j >= 0) && z+k >= 0) && ((x+i != map.width() && y+j != map.height()) && z+k != map.depth()) ){
					if( light > 0 && !tempMap(x+i, y+j, z+k) ){
						map(x+i, y+j, z+k, MapChannels::LIGHT) -= light - 1;
						tempMap(x+i, y+j, z+k) = true;
						stack.push_back( PixelCoord(x+i,y+j,z+k) );
					}
				}
			}
		}
	}
}

void VoxMap::addLight(size_t x, size_t y, size_t z, unsigned char light){
	tempMap = cimg_library::CImg<bool>(map.width(), map.height(), map.depth(), 1, false);
	std::vector<PixelCoord> stack;
	std::vector<PixelCoord> stackBuffer;

	//Initialisation
	map(x,y,z,MapChannels::LIGHT) = light;
	tempMap(x,y,z) = true;
	diffuseLight((int)x, (int)y, (int)z, light , stack);

	while( !stack.empty() ){
		for( PixelCoord& pix : stack ){
			diffuseLight((int)pix.x, (int)pix.y, (int)pix.z, map(pix.x, pix.y, pix.z, MapChannels::LIGHT), stackBuffer);
		}
		stack.clear();
		stack = stackBuffer;
		stackBuffer.clear();
	}
}

void VoxMap::removeLight(size_t x, size_t y, size_t z, unsigned char light){
	tempMap = cimg_library::CImg<bool>(map.width(), map.height(), map.depth(), 1, false);
	std::vector<PixelCoord> stack;
	std::vector<PixelCoord> stackBuffer;

	//Initialisation
	map(x,y,z,MapChannels::LIGHT) = light;
	tempMap(x,y,z) = true;
	undiffuseLight((int)x, (int)y, (int)z, light , stack);

	while( !stack.empty() ){
		for( PixelCoord& pix : stack ){
			undiffuseLight((int)pix.x, (int)pix.y, (int)pix.z, map(pix.x, pix.y, pix.z, MapChannels::LIGHT), stackBuffer);
		}
		stack.clear();
		stack = stackBuffer;
		stackBuffer.clear();
	}
}

void VoxMap::resetVisibleCubes(){
	for( size_t i = 0; i < CubeTypes::SIZE_CT; ++i ){
		cubes[i].instances.clear();
	}
}


bool VoxMap::seeThroughCubeType(size_t cubeType){
	return cubeType == CubeTypes::AIR || cubeType == CubeTypes::FOLLIAGE || cubeType == CubeTypes::WATER; 
}

void VoxMap::getVisibleNeighbors(int x, int y, int z, std::vector<PixelCoord>& stack){
	for( int i = -1; i < 2; ++i ){
		for( int j = -1; j < 2; ++j ){
			for( int k = -1; k < 2; ++k ){
				if( ((x+i >= 0 && y+j >= 0) && z+k >= 0) && ((x+i != map.width() && y+j != map.height()) && z+k != map.depth()) ){
					if( seeThroughCubeType( map(x+i, y+j, z+k, MapChannels::BLOC) ) && tempMap(x+i, y+j, z+k) == false ){
						tempMap(x+i, y+j, z+k) = true;
						stack.push_back( PixelCoord(x+i,y+j,z+k) );
					}
				}
			}
		}
	}
}
void VoxMap::fillVisibleCubes(size_t x, size_t y, size_t z){
	tempMap = cimg_library::CImg<bool>(map.width(), map.height(), map.depth(), 1, false);
	std::vector<PixelCoord> stack;
	std::vector<PixelCoord> stackBuffer;

	//Initialisation
	resetVisibleCubes();
	tempMap(x,y,z) = true;
	getVisibleNeighbors((int)x, (int)y, (int)z, stack);
	
	if( map(x, y, z, MapChannels::BLOC) < CubeTypes::SIZE_CT && map(x, y, z, MapChannels::BLOC) != CubeTypes::AIR ){
		cubes[ map(x, y, z, MapChannels::BLOC) ].addInstance( glm::vec4(x*voxelSize[0], y*voxelSize[1], z*voxelSize[2],1.0f) );
	}
	
	//Parcours
	while( !stack.empty() ){
		for( PixelCoord& pix : stack ){
			getVisibleNeighbors((int)pix.x, (int)pix.y, (int)pix.z, stackBuffer);
			if( map(x, y, z, MapChannels::BLOC) < CubeTypes::SIZE_CT && map(x, y, z, MapChannels::BLOC) != CubeTypes::AIR ){
				cubes[ map(pix.x, pix.y, pix.z, MapChannels::BLOC) ].addInstance( glm::vec4(pix.x*voxelSize[0], pix.y*voxelSize[1], pix.z*voxelSize[2],1.0f) );
			}
		}
		stack.clear();
		stack = stackBuffer;
		stackBuffer.clear();
	}

	//for( size_t i = 0; i < CubeTypes::SIZE_CT; ++i ){ cubes[i].updateInstanceSSBO(); }
}

void VoxMap::fillSSBO(){
	instances.clear();
	GLuint offset = 0;
	for( size_t i = 1; i < CubeTypes::SIZE_CT; ++i ){
		cubes[i].ssboOffset = offset;
		for( InstanceInfos& ref : cubes[i].instances ){
			instances.push_back( ref );
			++offset;
		}
		cubes[i].updateOffsetUBO();
	}
	updateInstanceSSBO();
}

void VoxMap::createInstanceSSBO(){
	GLuint size = map.width()*map.height()*map.depth();
	std::cout << "Allocation of \e[1;33m" << (GLfloat)size*sizeof(InstanceInfos)/1000000.0f << "\e[0m MB of vram"<< std::endl;
	
	instances.resize(size);
	glGenBuffers(1, &mapSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mapSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(InstanceInfos)*size, instances.data(), GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	instances.clear();
}

void VoxMap::updateInstanceSSBO(){
	void* data;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mapSSBO);
	data = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(InstanceInfos)*instances.size(), GL_MAP_WRITE_BIT);
	memcpy(data, instances.data(), sizeof(InstanceInfos)*instances.size());
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}
