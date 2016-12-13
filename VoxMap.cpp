#include "VoxMap.hpp"
#include <assert.h>

#include <vector>
#include <string>

#include "biome.hpp"


VoxMap::VoxMap(){
	voxelSize[0] = 2.0f;
	voxelSize[1] = 2.0f;
	voxelSize[2] = 2.0f;
	maxLightLevel = 15;


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
	fillSSBO();
}


void VoxMap::genereMap(MapGenerator* biome, std::string name) {
	biome->fill(map);
	save(name);
}

void VoxMap::testMap(std::vector<std::string>& args){
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
				genereMap(bioms[0], args[1]);
			}
		} else {
			genereMap(bioms[0], "maMap");
		}
	} else {
		genereMap(bioms[0], "maMap");
	}
	
	fillVisibleCubes(map.width()-20, map.height()-20, map.depth()-20 );
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
						//tempMap(x+i, y+j, z+k) = true;
						stack.push_back( PixelCoord(x+i,y+j,z+k) );
					}
					if( map(x+i, y+j, z+k, MapChannels::BLOC) != CubeTypes::AIR && tempMap(x+i, y+j, z+k) == false ){
						//tempMap(x+i, y+j, z+k) = true;
						cubes[ map(x+i, y+j, z+k, MapChannels::BLOC) ].addInstance( glm::vec4((x+i)*voxelSize[0], (y+j)*voxelSize[1], (z+k)*voxelSize[2],1.0f) );
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
	std::cout << stack.size() << std::endl;
	
	if( map(x, y, z, MapChannels::BLOC) < CubeTypes::SIZE_CT && map(x, y, z, MapChannels::BLOC) != CubeTypes::AIR ){
		cubes[ map(x, y, z, MapChannels::BLOC) ].addInstance( glm::vec4(x*voxelSize[0], y*voxelSize[1], z*voxelSize[2],1.0f) );
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
