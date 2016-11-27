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
	fillSSBO();
}

float Get2DPerlinNoiseValue(float x, float y, float res)

{

    float tempX,tempY;

    int x0,y0,ii,jj,gi0,gi1,gi2,gi3;

    float unit = 1.0f/sqrt(2);

    float tmp,s,t,u,v,Cx,Cy,Li1,Li2;

    float gradient2[][2] = {{unit,unit},{-unit,unit},{unit,-unit},{-unit,-unit},{1,0},{-1,0},{0,1},{0,-1}};


    unsigned int perm[] =

       {151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,

        142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,

        203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,

        74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,

        105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,

        187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,

        64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,

        47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,

        153,101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,

        112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,

        235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,

        127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,

        156,180};


    //Adapter pour la résolution

    x /= res;

    y /= res;


    //On récupère les positions de la grille associée à (x,y)

    x0 = (int)(x);

    y0 = (int)(y);


    //Masquage

    ii = x0 & 255;

    jj = y0 & 255;


    //Pour récupérer les vecteurs

    gi0 = perm[ii + perm[jj]] % 8;

    gi1 = perm[ii + 1 + perm[jj]] % 8;

    gi2 = perm[ii + perm[jj + 1]] % 8;

    gi3 = perm[ii + 1 + perm[jj + 1]] % 8;


    //on récupère les vecteurs et on pondère

    tempX = x-x0;

    tempY = y-y0;

    s = gradient2[gi0][0]*tempX + gradient2[gi0][1]*tempY;


    tempX = x-(x0+1);

    tempY = y-y0;

    t = gradient2[gi1][0]*tempX + gradient2[gi1][1]*tempY;


    tempX = x-x0;

    tempY = y-(y0+1);

    u = gradient2[gi2][0]*tempX + gradient2[gi2][1]*tempY;


    tempX = x-(x0+1);

    tempY = y-(y0+1);

    v = gradient2[gi3][0]*tempX + gradient2[gi3][1]*tempY;



    //Lissage

    tmp = x-x0;

    Cx = 3 * tmp * tmp - 2 * tmp * tmp * tmp;


    Li1 = s + Cx*(t-s);

    Li2 = u + Cx*(v-u);


    tmp = y - y0;

    Cy = 3 * tmp * tmp - 2 * tmp * tmp * tmp;


    return Li1 + Cy*(Li2-Li1);

}
void VoxMap::testMap(){
	createInstanceSSBO();
	for( size_t i = 0; i < CubeTypes::SIZE_CT; ++i ){
		cubes[i].maxNbInstances = map.width()*map.height()*map.depth();
		cubes[i].instanceSSBO = mapSSBO;
		cubes[i].createOffsetUBO();
	}
	
	int hSize = map.height();
	int wSize = map.width();
	int dSize = map.depth();

	int **hauteur = new int* [wSize];
	for (int i = 0; i < wSize; ++i) {
		hauteur[i] = new int [dSize];
		for (int j = 0; j < dSize; ++j) {
			hauteur[i][j] = 0;
		}
	}


	//genererPlaine(hauteur);
	  for (int i = 0; i < wSize; ++i) {
	  	for (int j = 0; j < dSize; ++j) {
	    	hauteur[i][j] = (Get2DPerlinNoiseValue((float)i/wSize, (float)j/dSize, (float)0.2) +1) * 0.5 * hSize;
		}
	  }
	  

	for( int z = 0; z < map.depth(); ++z ){
		for( int y = map.height() - 1; y > 0; --y ){
			for( int x = 0; x < map.width(); ++x ){
				if (y < hauteur[x][z]) {
					//map(x,y,z,MapChannels::BLOC) = y % CubeTypes::SIZE_CT;
					map(x, y,z,MapChannels::BLOC) = (map.height()-y) % CubeTypes::SIZE_CT;	
				} else { 
					map(x, y,z,MapChannels::BLOC) = CubeTypes::AIR;
				}
				map(x,y,z,MapChannels::LIGHT) = 0xFF;	
			}
		}
	}
	

/*

	for( int z = 0; z < map.depth(); ++z ){
		for( int y = 0; y < map.height(); ++y ){
			for( int x = 0; x < map.width(); ++x ){
				if( (size_t)y < CubeTypes::SIZE_CT ){
					map(x,y,z,MapChannels::BLOC) = y;
				}
				else{ map(x,y,z,MapChannels::BLOC) = CubeTypes::AIR; }
				map(x,y,z,MapChannels::LIGHT) = 0xFF;	
			}
		}
	}
*/
	//cimg_library::CImg<bool> mapObjects = getMapObjects();
	//cimg_library::CImg<bool> dilate = mapObjects.get_dilate(3);
	//cimg_library::CImg<bool> erode = mapObjects.get_erode(3);
	//getMapOutline(dilate,erode);
	fillVisibleCubes(120, hauteur[119][119], 120 );
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
