#include "VoxMap.hpp"
#include <assert.h>

#include <vector>
#include <string>

#include "biome.hpp"


VoxMap::VoxMap(glm::vec3 pos){
	voxelSize[0] = 2.0f;
	voxelSize[1] = 2.0f;
	voxelSize[2] = 2.0f;
	position = glm::vec3(pos);
	

	biomName.push_back("default");
	bioms.push_back(new MapGenerator());
	biomName.push_back("valley");
	bioms.push_back(new MapGenerator());
	biomName.push_back("desert");
	bioms.push_back(new DesertGenerator());
	biomName.push_back("archipel");
	bioms.push_back(new ArchipelGenerator());
	biomName.push_back("mangrove");
	bioms.push_back(new MangroveGenerator());
	biomName.push_back("prairie");
	bioms.push_back(new PrairieGenerator());
	biomName.push_back("snowPrairie");
	bioms.push_back(new SnowPrairieGenerator());
	biomName.push_back("snowValley");
	bioms.push_back(new SnowValleyGenerator());
}

VoxMap::~VoxMap(){
	int size = bioms.size();
	for (int i = 0; i < size; ++i) {
		delete bioms[i];
	}
}

void VoxMap::save(std::string& path) { 
	std::string repositoryPath = "Map/" + path;
	std::cout << repositoryPath << std::endl;
	map.save_analyze(repositoryPath.c_str(),voxelSize);
}

void VoxMap::load(std::string& path) {
	map.load_analyze(path.c_str(), voxelSize);
}

void VoxMap::newMap(int width, int height, int depth){
	map = cimg_library::CImg<unsigned char>(width,height,depth,1,0x0);
	voxelSize[0] = 2.0f;
	voxelSize[1] = 2.0f;
	voxelSize[2] = 2.0f;
}

cimg_library::CImg<bool> VoxMap::getMapObjects(){
	cimg_library::CImg<bool> res = cimg_library::CImg<bool>(map.width(), map.height(), map.depth(), 1, false);

	for( int z = 0; z < map.depth(); ++z ){
		for( int y = 0; y < map.height(); ++y ){
			for( int x = 0; x < map.width(); ++x ){
				if( map(x,y,z) != CubeTypes::AIR && tempMap(x,y,z) == true){
					res(x,y,z) = true;
				}
			}
		}
	}
	res.save_analyze("visible");
	return res;
}

void VoxMap::genereMap(MapGenerator* biome, std::string name) {
	biome->fill(map);
	save(name);
}

void VoxMap::testMap(std::vector<std::string>& args) {
	createInstanceSSBO();
	for( size_t i = 0; i < CubeTypes::SIZE_CT; ++i ){
		cubes[i].maxNbInstances = map.width()*map.height()*map.depth();
		cubes[i].instanceSSBO = mapSSBO;
		cubes[i].createOffsetUBO();
	}
	
	
	if ((args.size() >= 2)) { 
		if (args[0] == "load") {
			try {
				args[1] += ".hdr";
				args[1].insert(0, "Map/");
				map.load(args[1].c_str());
			} catch (cimg_library::CImgIOException e) {
				genereMap(bioms[0], "maMap");
			}
		} else if (args[0] == "new") {
			if (args.size() >= 3) {
				// Get the standard biom
				MapGenerator* biom = bioms[0];
				int size = bioms.size();
				bool match = false;

				for (int i = 1; (i < size) && !match; ++i) {
					if (biomName[i] == args[2]) {
						biom = bioms[i];
						match = true;
					}
				}

				genereMap(biom, args[1]);
			} else {
				int indexBiom = rand() % bioms.size();
				genereMap(bioms[indexBiom], args[1]);
			}
		} else {

			int indexBiom = rand() % bioms.size();
			genereMap(bioms[indexBiom], "maMap");
		}
	} else {

		int indexBiom = rand() % bioms.size();
		genereMap(bioms[indexBiom], "maMap");
	}
	
	fillVisibleCubes(map.width()-20, map.height()-10, map.depth()-20 );
} 

void VoxMap::render(){
	for( size_t i = 1; i < CubeTypes::SIZE_CT; ++i ){
		cubes[i].render();
	}
}

void VoxMap::loadVoxel(std::string& path, std::string& name){
	std::vector< std::string > cubesPath;
	std::string cubeName = "cube.obj";
	cubesPath.push_back("./assets/cubes/");
	cubesPath.push_back("./assets/cubes/adminium/");
	cubesPath.push_back("./assets/cubes/cactus/");
	cubesPath.push_back("./assets/cubes/coal/");
	cubesPath.push_back("./assets/cubes/diamond/");
	cubesPath.push_back("./assets/cubes/dirt/");
	cubesPath.push_back("./assets/cubes/folliage/");
	cubesPath.push_back("./assets/cubes/glowstone/");
	cubesPath.push_back("./assets/cubes/gold/");
	cubesPath.push_back("./assets/cubes/grass/");
	cubesPath.push_back("./assets/cubes/iron/");
	cubesPath.push_back("./assets/cubes/sand/");
	cubesPath.push_back("./assets/cubes/snow/");
	cubesPath.push_back("./assets/cubes/snow_block/");
	cubesPath.push_back("./assets/cubes/stone/");
	cubesPath.push_back("./assets/cubes/water/");
	cubesPath.push_back("./assets/cubes/wood/");

	cubes[0].loadMesh(path,name);
	for( size_t i = 1; i < CubeTypes::SIZE_CT; ++i ){
		cubes[i].loadMesh(cubesPath.at(i),cubeName);
	}
}


void VoxMap::resetVisibleCubes(){
	for( size_t i = 0; i < CubeTypes::SIZE_CT; ++i ){
		cubes[i].instances.clear();
	}
}


bool VoxMap::seeThroughCubeType(size_t cubeType){
	return cubeType == CubeTypes::AIR || cubeType == CubeTypes::WATER; 
}

void VoxMap::getVisibleNeighbors(int x, int y, int z, std::vector<PixelCoord>& stack){
	for( int i = -1; i < 2; ++i ){
		for( int j = -1; j < 2; ++j ){
			for( int k = -1; k < 2; ++k ){
				if( ((x+i >= 0 && y+j >= 0) && z+k >= 0) && ((x+i != map.width() && y+j != map.height()) && z+k != map.depth()) ){
					if( seeThroughCubeType( map(x+i, y+j, z+k, MapChannels::BLOC) ) && tempMap(x+i, y+j, z+k) == false ){
						stack.push_back( PixelCoord(x+i,y+j,z+k) );
					}
					if( map(x+i, y+j, z+k, MapChannels::BLOC) != CubeTypes::AIR && tempMap(x+i, y+j, z+k) == false ){
						cubes[ map(x+i, y+j, z+k, MapChannels::BLOC) ].addInstance( glm::vec4((x+i)*voxelSize[0]+position.x, (y+j)*voxelSize[1]+position.y, (z+k)*voxelSize[2]+position.z, 1.0f) );
					}
					tempMap(x+i, y+j, z+k) = true;
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
		cubes[ map(x, y, z, MapChannels::BLOC) ].addInstance( glm::vec4(x*voxelSize[0]+position.x, y*voxelSize[1]+position.y, z*voxelSize[2]+position.z, 1.0f) );
	}
	
	//Parcours
	while( !stack.empty() ){
		for( PixelCoord& pix : stack ){
			getVisibleNeighbors((int)pix.x, (int)pix.y, (int)pix.z, stackBuffer);
		}
		stack.clear();
		stack = stackBuffer;
		stackBuffer.clear();
	}
	fillSSBO();
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


void VoxMap::addBlock(size_t x, size_t y, size_t z, CubeTypes type, glDeferredRenderer& renderer, glContext* context){
	if( map(x,y,z) == CubeTypes::AIR ){
		cubes[type].addInstance( glm::vec4(x*voxelSize[0]+position.x, y*voxelSize[1]+position.y, z*voxelSize[2]+position.z, 1.0f) );
		if( type == CubeTypes::GLOWSTONE ){
			renderer.addLight( glm::vec4(x*voxelSize[0]+position.x, y*voxelSize[1]+position.y, z*voxelSize[2]+position.z, 1.0f) );
			context->addLight( glm::vec4(x*voxelSize[0]+position.x, y*voxelSize[1]+position.y, z*voxelSize[2]+position.z, 1.0f) );
		}
		map(x,y,z) = type;
		fillSSBO();
		//fillVisibleCubes(x,y,z);
	}	
}
void VoxMap::removeBlock(size_t x, size_t y, size_t z, glDeferredRenderer& renderer, glContext* context){
	if( map(x,y,z) != CubeTypes::AIR ){
		cubes[ map(x,y,z) ].removeInstance( glm::vec4(x*voxelSize[0]+position.x, y*voxelSize[1]+position.y, z*voxelSize[2]+position.z, 1.0f) );
		if( map(x,y,z) == CubeTypes::GLOWSTONE ){
			renderer.removeLight( glm::vec4(x*voxelSize[0]+position.x, y*voxelSize[1]+position.y, z*voxelSize[2]+position.z, 1.0f) );
			context->removeLight( glm::vec4(x*voxelSize[0]+position.x, y*voxelSize[1]+position.y, z*voxelSize[2]+position.z, 1.0f) );
		}
		map(x,y,z) = CubeTypes::AIR;
		//fillSSBO();
		fillVisibleCubes(map.width()-20, map.height()-10, map.depth()-20 );
	}
}


bool VoxMap::isInMap(glm::vec3 pos){
	glm::vec3 maxPos = glm::vec3(position.x + map.width()*voxelSize[0],
	                             position.y + map.height()*voxelSize[1],
	                             position.z + map.depth()*voxelSize[2]
	                             );
	if( (pos.x >= position.x && pos.x < maxPos.x) && (pos.y >= position.y && pos.y < maxPos.y) && (pos.z >= position.z && pos.z < maxPos.z) ){
		return true;
	}
	return false;
}
PixelCoord VoxMap::mapCoord(glm::vec3 pos){
	glm::vec3 newPos = glm::vec3(pos.x - position.x, pos.y - position.y, pos.z - position.z);
	newPos.x /= voxelSize[0];
	newPos.y /= voxelSize[1];
	newPos.z /= voxelSize[2];
	newPos.x = std::round(newPos.x);
	newPos.y = std::round(newPos.y);
	newPos.z = std::round(newPos.z);
	return PixelCoord((size_t)newPos.x, (size_t)newPos.y, (size_t)newPos.z);
}

//######################## VoxMapManager ##########################################################
VoxMapManager::VoxMapManager(){}
VoxMapManager::~VoxMapManager(){
	for( size_t i = 0; i < mapList.size(); ++i ){
		//delete mapList[i];
	}
}
void VoxMapManager::addBlock(glm::vec3 pos, CubeTypes type, glDeferredRenderer& renderer, glContext* context){
	for( VoxMap* map : mapList ){
		if( map->isInMap(pos) ){
			PixelCoord coord = map->mapCoord(pos);
			map->addBlock(coord.x,coord.y,coord.z,type,renderer, context);
			break;
		}
	}
}
void VoxMapManager::removeBlock(glm::vec3 pos, glDeferredRenderer& renderer, glContext* context){
	for( VoxMap* map : mapList ){
		if( map->isInMap(pos) ){
			PixelCoord coord = map->mapCoord(pos);
			map->removeBlock(coord.x,coord.y,coord.z,renderer,context);
			break;
		}
	}
}
CubeTypes VoxMapManager::cubeAt(glm::vec3 pos){
	for( VoxMap* map : mapList ){
		if( map->isInMap(pos) ){
			PixelCoord coord = map->mapCoord(pos);
			return (CubeTypes)map->map(coord.x, coord.y, coord.z); 
		}
	}
	return (CubeTypes)0x0;
}
