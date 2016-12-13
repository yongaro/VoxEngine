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
};


class VoxMap{
public:
	cimg_library::CImg<unsigned char> map;
	cimg_library::CImg<bool> tempMap;
	float voxelSize[3];
	unsigned char maxLightLevel;
	glInstancedMesh cubes[CubeTypes::SIZE_CT];
	std::vector< InstanceInfos > instances;
	GLuint mapSSBO;
	

	std::vector<MapGenerator*> bioms;
	std::vector<std::string> biomName;

	VoxMap();
	virtual ~VoxMap();
	virtual void save(std::string&);
	virtual void load(std::string&);
	virtual void newMap(int,int,int);
	virtual cimg_library::CImg<bool> getMapObjects();
	virtual void getMapOutline(cimg_library::CImg<bool>&,cimg_library::CImg<bool>&);
	virtual void testMap(std::vector<std::string>&);
	virtual void genereMap(MapGenerator*, std::string);
	virtual void render();
	virtual glm::vec3 getCamPos();
	virtual void loadVoxel(std::string&,std::string&);
	virtual void resetVisibleCubes();
	virtual bool seeThroughCubeType(size_t);
	virtual void getVisibleNeighbors(int, int, int, std::vector<PixelCoord>&);
	virtual void fillVisibleCubes(size_t, size_t, size_t);

	virtual void fillSSBO();
	virtual void createInstanceSSBO();
	virtual void updateInstanceSSBO();

};

#endif
