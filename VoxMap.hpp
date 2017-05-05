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


template<class T>
class VoxImage{
private:
  T* data;
  uint32_t width;
  uint32_t height;
  uint32_t depth;
public:
  VoxImage():data(), width(), height(), depth(){}
  VoxImage(uint32_t w, uint32_t h, uint32_t d, T defaultValue):data(new T[w * h * d]), width(w), height(h), depth(d){}
  ~VoxImage(){ delete[] data; }

  void save(std::string filePath){
    FILE* file = fopen(filePath.c_str(), "wb");
    //writing image dimensions.
    fwrite( &width, sizeof(uint32_t), 1, file );
    fwrite( &height, sizeof(uint32_t), 1, file );
    fwrite( &depth, sizeof(uint32_t), 1, file );

    //saving image content.
    fwrite( data, sizeof(T), width * height * depth, file );

    //closing file.
    fclose(file);
  }
  void load(std::string filePath){
    FILE* file = fopen(filePath.c_str(), "rb");
    //reading image dimensions.
    fread( &width, sizeof(uint32_t), 1, file );
    fread( &height, sizeof(uint32_t), 1, file );
    fread( &depth, sizeof(uint32_t), 1, file );

    //space allocation.
    if( data != NULL){ delete[] data; }
    data = new T[width * height * depth];

    //recovering image content.
    fread( data, sizeof(T), width * height * depth, file );

    //closing file.
    fclose(file);
  }
  T& operator()(uint32_t x, uint32_t y, uint32_t z){ return data[ (z*depth) + (y*width) + x]; }
  uint32_t getWidth(){ return width; }
  uint32_t getHeight(){ return height; }
  uint32_t getDepth(){ return depth; }
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
