#ifndef VOXMAP_HPP
#define VOXMAP_HPP

#include "glGeometry.hpp"

#define cimg_display 0
#include "CImg.h"

//Regroupement des cubes par matériaux
enum CubeTypes{ ADMINIUM, AIR, DIRT, FOLLIAGE, GLOWSTONE, GRASS, SAND, SNOW, STONE, WATER, WOOD, SIZE_CT };
enum MapChannels{ BLOC, LIGHT, SIZE_MC };

struct PixelCoord{
	unsigned int x;
	unsigned int y;
	unsigned int z;

	PixelCoord():x(0),y(0),z(0){}
	PixelCoord(unsigned int ix, unsigned int iy, unsigned int iz):x(ix),y(iy),z(iz){}
};


class VoxMap{
public:
	cimg_library::CImg<unsigned char> map;
	cimg_library::CImg<bool> tempMap;
	float voxelSize[3];
	unsigned char maxLightLevel;
	glm::vec4 cubeColors[CubeTypes::SIZE_CT];
	glInstancedMesh cubes[CubeTypes::SIZE_CT];
	
	
	VoxMap();
	virtual ~VoxMap();
	virtual void save(std::string&);
	virtual void load(std::string&);
	virtual void newMap(int,int,int);
	virtual void testMap();
	virtual void render();
	virtual glm::vec3 getCamPos();
	virtual void loadVoxel(std::string&,std::string&);
	virtual void diffuseLight(int,int,int,unsigned char,std::vector<PixelCoord>&);
};

#endif
