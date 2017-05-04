#ifndef VOXMAP_HPP
#define VOXMAP_HPP

#include "glGeometry.hpp"

#define cimg_display 0
#include "CImg.h"
#include "MapGenerator.hpp"

#include <vector>
#include <string>

//Regroupement des cubes par matériaux
enum CubeTypes{  AIR, ADMINIUM, CACTUS, COAL, DIAMOND, DIRT, FOLLIAGE, GLOWSTONE, GOLD,
                 GRASS, IRON, SAND, SNOW, SNOW_BLOCK, STONE, WATER, WOOD, SIZE_CT };
enum MapChannels{ BLOC, LIGHT, SIZE_MC };

struct PixelCoord{
	size_t x;
	size_t y;
	size_t z;

	PixelCoord():x(0),y(0),z(0){}
	PixelCoord(size_t ix, size_t iy, size_t iz):x(ix),y(iy),z(iz){}
	PixelCoord(const PixelCoord& ref):x(ref.x),y(ref.y),z(ref.z){}
};


class VoxMap{
public:
	cimg_library::CImg<unsigned char> map;
	cimg_library::CImg<bool> tempMap;
	float voxelSize[3];
	glm::vec3 position;
	glInstancedMesh cubes[CubeTypes::SIZE_CT];
	std::vector< InstanceInfos > instances;
	GLuint mapSSBO;
	

	std::vector<MapGenerator*> bioms;
	std::vector<std::string> biomName;

	VoxMap(glm::vec3);
	virtual ~VoxMap();
	virtual void save(std::string&);
	virtual void load(std::string&);
	virtual void newMap(int,int,int);
	virtual cimg_library::CImg<bool> getMapObjects();
	virtual void testMap(std::vector<std::string>&);
	virtual void genereMap(MapGenerator*, std::string);
	virtual void render();
	virtual void loadVoxel(std::string&,std::string&);
	virtual void resetVisibleCubes();
	virtual bool seeThroughCubeType(size_t);
	virtual void getVisibleNeighbors(int, int, int, std::vector<PixelCoord>&);
	virtual void fillVisibleCubes(size_t, size_t, size_t);

	virtual void fillSSBO();
	virtual void createInstanceSSBO();
	virtual void updateInstanceSSBO();

	virtual void addBlock(size_t, size_t, size_t, CubeTypes, glDeferredRenderer&, glContext*);
	virtual void removeBlock(size_t, size_t, size_t, glDeferredRenderer&, glContext*);
	virtual bool isInMap(glm::vec3);
	virtual PixelCoord mapCoord(glm::vec3);
};

class VoxMapManager{
public:
	std::vector< VoxMap* > mapList;

	VoxMapManager();
	~VoxMapManager();

	virtual void addBlock(glm::vec3, CubeTypes, glDeferredRenderer&, glContext*);
	virtual void removeBlock(glm::vec3, glDeferredRenderer&,glContext*);
	virtual CubeTypes cubeAt(glm::vec3);
};



#endif
