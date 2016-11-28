#include "VoxMap.hpp"
#include <assert.h>

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

int getNbBloc (int nb) {
  if (nb < 1) {
    return -1;
  } else if (nb > 3) {
    return 2;
  } else {
    return 0;
  }
}


double getProbaAir (int height, int maxHeight) {
	return std::max((double(height) / maxHeight) - 0.5 * maxHeight, 0.0);
}

double getProbaAdminium (int height, int maxHeight) {
	double u = double(height) / maxHeight;
	if (u < 0.2) {
		return 0.1;
	} else {
		return 0.0;
	}
}

double getProbaCactus (int height, int maxHeight) {
	double u = double(height) / maxHeight;
	if (u > 0.9) {
		return 0.0;
	} else {
		return 0.0;
	}
}

double getProbaCoal (int height, int maxHeight) {
	double u = 1 - double(height) / maxHeight;

	if (u < 0.09) {
		return 0.012 * u / 0.09;
	} else if (u < 0.5) {
		return 0.08 + double(rand() / 4) / 10.0;
	} else if (u < 0.73) {
		return 0.05 - u / (0.72 - 0.5);
	} else {
		return double(rand() % 10) / 1000.0;
	}
}


double getProbaDirt (int height, int maxHeight) {
	double u = double(height) / maxHeight;
	if (u > 0.7) {
		return 0.2;
	} else {
		return 0.15;
	}
}

double getProbaFolliage (int height, int maxHeight) {
	return 0.0;
}

double getProbaGlowstone (int height, int maxHeight) {
	return 0.0;
}

double getProbaGold (int height, int maxHeight) {
	return getProbaCoal(height * 0.5, maxHeight) * 0.1;
}

double getProbaDiamond (int height, int maxHeight) {
	return getProbaGold(height * 0.7, maxHeight);
}

double getProbaGrass (int height, int maxHeight) {
	double u = double(height) / maxHeight;
	if (height == maxHeight) {
		return 0.8;
	} else if (u > 0.8) {
		return 0.7;
	} else {
		return 0.0;
	}
}

double getProbaIron (int height, int maxHeight) {
	return getProbaCoal(height, maxHeight) * 0.5;
}

double getProbaSand (int height, int maxHeight) {
	double u = double(height) / maxHeight;
	if (u > 0.9) {
		return 0.0;
	} else {
		return 0;
	}
}

double getProbaSnow (int height, int maxHeight) {
	return 0;
}

double getProbaSnowBlock (int height, int maxHeight) {
	return 0;
}

double getProbaStone (int height, int maxHeight) {
	double u = double(height) / maxHeight;
	if (u > 0.9) {
		return 0.05;
	} else {
		return 0.3;
	}
}

double getProbaWater (int height, int maxHeight, int superMaxHeight) {
	double u = double(height) / maxHeight;
	double v = double(height) / superMaxHeight;
	if ((u > 0.9) && v < 0.6) {
		return 0.001;
	} else if (u > 0.7) {
		return 0.0;
	} else {
		return 0.0;
	}
}


double getProbaWood (int height, int maxHeight) {
	double u = double(height) / maxHeight;
	if (u > 0.9) {
		return 0.01;
	} else {
		return 0;
	}
}

std::vector<double> getProbaBloc (int depth, int maxHeight, int superMaxHeight) {
	std::vector<double> proba;

	proba.push_back(getProbaAir(depth, maxHeight));
	proba.push_back(getProbaAdminium(depth, maxHeight));
	proba.push_back(getProbaCactus(depth, maxHeight));
	proba.push_back(getProbaCoal(depth, maxHeight));
	proba.push_back(getProbaDiamond(depth, maxHeight));
	proba.push_back(getProbaDirt(depth, maxHeight));
	proba.push_back(getProbaFolliage(depth, maxHeight));
	proba.push_back(getProbaGlowstone(depth, maxHeight));
	proba.push_back(getProbaGold(depth, maxHeight));
	proba.push_back(getProbaGrass(depth, maxHeight));
	proba.push_back(getProbaIron(depth, maxHeight));
	proba.push_back(getProbaSand(depth, maxHeight));
	proba.push_back(getProbaSnow(depth, maxHeight));
	proba.push_back(getProbaSnowBlock(depth, maxHeight));
	proba.push_back(getProbaStone(depth, maxHeight));
	proba.push_back(getProbaWater(depth, maxHeight, superMaxHeight));
	proba.push_back(getProbaWood(depth, maxHeight));

	return proba;
}

void growRiver(cimg_library::CImg<unsigned char>& map) {
	int hSize = map.height();
	int wSize = map.width();
	int dSize = map.depth();

	for (int y = hSize-1; y > 1; --y ) {
	  	for (int z = 1; z < (dSize - 1); ++z ) {
	    	for (int x = 1; x < (wSize - 1); ++x ) {
	    		if (map(x, y, z, MapChannels::BLOC) == CubeTypes::WATER) {
	    			for (int w = 0; w < 9; ++w) {
    					if (map(x - 1 + (w / 3), y - 1,  z - 1 + (w %3), MapChannels::BLOC) == CubeTypes::AIR) {
	    					map(x - 1 + (w / 3), y - 1,  z - 1 + (w %3), MapChannels::BLOC) = CubeTypes::WATER;
	    				}	
    				}
	    		}
			}
		}
	}
}

void diffuseFolliage (cimg_library::CImg<unsigned char>& map) {
	int hSize = map.height();
	int wSize = map.width();
	int dSize = map.depth();
	cimg_library::CImg<bool> maskFolliage = cimg_library::CImg<bool>(wSize, hSize, dSize, 1, false);

   	for (int i = 0; i < wSize; ++i) {
   		for (int j = 0; j < hSize; ++j) {
   			for (int k = 0; k < dSize; ++k) {
   				if (map(i,j,k, MapChannels::BLOC) == CubeTypes::FOLLIAGE) {
   					maskFolliage(i, j, k, 0) = true;
   				}
   			}
   		}
   	}

	

	for (int y = 1; y < (hSize-1); ++y ) {
	  	for (int z = 1; z < (dSize - 1); ++z ) {
	    	for (int x = 1; x < (wSize - 1); ++x ) {
	    		// Dilatation
	    		if (maskFolliage(x, y, z, 0)) {
	    			for (int w = 0; w < 27; ++w) {
	    				int random = rand() % 100;
    					if ((map(x - 1 + (w / 3), y - 1 + (w / 9),  z - 1 + (w %3), MapChannels::BLOC) == CubeTypes::AIR) && (random < 45)) {
	    					map(x - 1 + (w / 3), y - 1 + (w / 9),  z - 1 + (w %3), MapChannels::BLOC) = CubeTypes::FOLLIAGE;
	    				}	
    				}
	    		}
	    	}
	    }
	}
}

void growTree (cimg_library::CImg<unsigned char>& map) {
	int hSize = map.height();
	int wSize = map.width();
	int dSize = map.depth();

	for (int y = 0; y < (hSize-1); ++y ) {
	  	for (int z = 1; z < (dSize - 1); ++z ) {
	    	for (int x = 1; x < (wSize - 1); ++x ) {
	    		if (map(x, y, z, MapChannels::BLOC) == CubeTypes::WOOD && map(x, y + 1, z, MapChannels::BLOC) == CubeTypes::AIR) {
	    			int random = rand() % 100;
	    			if (map(x, y - 1, z, MapChannels::BLOC) != CubeTypes::WOOD) {
	    				random = 90;
	    			}

	    			if (random < 35) {
	    				map(x, y + 1, z, MapChannels::BLOC) = CubeTypes::FOLLIAGE;
	    				
	    				random = rand() % 4;
	    				if (map(x + 1, y + 1,  z + 1, MapChannels::BLOC) == CubeTypes::AIR && random < 3) {
	    					map(x + 1, y + 1, z + 1, MapChannels::BLOC) = CubeTypes::FOLLIAGE;
	    				}

	    				random = rand() % 4;
	    				if (map(x + 1, y + 1,  z - 1, MapChannels::BLOC) == CubeTypes::AIR && random < 2) {
	    					map(x + 1, y + 1, z - 1, MapChannels::BLOC) = CubeTypes::FOLLIAGE;
	    				}
	    				
	    				if (map(x + 1, y + 1,  z , MapChannels::BLOC) == CubeTypes::AIR) {
	    					map(x + 1, y + 1, z, MapChannels::BLOC) = CubeTypes::FOLLIAGE;
	    				}


	    				// 
	    				if (map(x , y + 1,  z, MapChannels::BLOC) == CubeTypes::AIR) {
	    					map(x , y + 1, z, MapChannels::BLOC) = CubeTypes::FOLLIAGE;
	    				}
	    				
	    				if (map(x - 1, y + 1,  z , MapChannels::BLOC) == CubeTypes::AIR) {
	    					map(x - 1, y + 1, z, MapChannels::BLOC) = CubeTypes::FOLLIAGE;
	    				}

	    				random = rand() % 4;
	    				if (map(x - 1, y + 1,  z + 1, MapChannels::BLOC) == CubeTypes::AIR && random < 3) {
	    					map(x - 1, y + 1, z + 1, MapChannels::BLOC) = CubeTypes::FOLLIAGE;
	    				}
	    				

	    			} else {
	    				map(x, y + 1, z, MapChannels::BLOC) = CubeTypes::WOOD;
	    				for (int w = 0; w < 9; ++w) {
	    					random = rand() % 4;
		    				if (map(x - 1 + (w / 3), y + 1,  z - 1 + (w %3), MapChannels::BLOC) == CubeTypes::AIR && random < 2) {
		    					map(x - 1 + (w / 3), y + 1,  z - 1 + (w %3), MapChannels::BLOC) = CubeTypes::FOLLIAGE;
		    				}	
	    				}
	    			}
	    			//diffuseFolliage(map);
	    		}

	    	}
	    }
	}
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


	std::vector<std::vector<double>> probaBlocLevel;


	int **hauteur = new int* [wSize];
	for (int i = 0; i < wSize; ++i) {
		hauteur[i] = new int [dSize];
		for (int j = 0; j < dSize; ++j) {
			hauteur[i][j] = hSize / 2;
		}
	}


	//genererPlaine(hauteur);
	/*
	  for (int i = 0; i < wSize; ++i) {
	  	for (int j = 0; j < dSize; ++j) {
	    	hauteur[i][j] = (Get2DPerlinNoiseValue((float)i/wSize, (float)j/dSize, (float)0.2) +1) * 0.5 * hSize;
		}
	  }
	  */
   for (int i = 2; i < wSize; ++i) {
      for (int j = 2; j < dSize; ++j) {
        int h = ((hauteur[i-1][j] + hauteur[i][j - 1] + hauteur[i-1][j-1])  / 3) + getNbBloc(rand() % 6);
        hauteur[i][j] = (h < hSize) ? h : hSize - 1;
        //hauteur[i][j] = (Get2DPerlinNoiseValue((float)i/wSize, (float)j/dSize, (float)0.2) +1) * 0.5 * hSize;
    }
  }
  
  int **copie = new int* [wSize];
  for (int i = 0; i < wSize; ++i) {
    copie[i] = new int [dSize];
    for (int j = 0; j < dSize; ++j) {
      copie[i][j] = hauteur[i][j];
    }
  }


   for (int i = 2; i < (wSize-2); i++) {
     for (int j = 2; j < (dSize-2); j++) {
      
   int p1 = copie[i-1][j-1];
   int p2 = copie[i-1][j];
   int p3 = copie[i-1][j+1];
   int p4 = copie[i][j-1];
   int p5 = copie[i][j+1];
   int p6 = copie[i+1][j];
   int p7 = copie[i+1][j];
   int p8 = copie[i+1][j+1];

   		hauteur[i][j] = (1.0/8) * (p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8);//(couleur[i*3][j*3 + k] + pixel) / 2;
   
     }
   }  

	


for( int y = 0; y < hSize; ++y ){
  for( int z = 0; z < dSize; ++z ){
    for( int x = 0; x < wSize; ++x ){
      	if (y == 0) {
      		map(x, y, z, MapChannels::BLOC) = CubeTypes::ADMINIUM; 
      	} else if (y <= hauteur[x][z]) {

      		// On déclare la variable qui déterminera le type de bloc
		    int numeroBlocSuppose = 8;
			
			double sum = 0;
		    // On demande un chiffre entre 0 et 100, on s'en servira pour fixer le type de cube une fois qu'on a calculé les probas.
		    int random = rand() % 100;
			      
		  
			    
	        double proba [CubeTypes::SIZE_CT];
	        std::vector<double> probaByHeight = getProbaBloc(y, hauteur[x][z], hSize);

	        for (int w = 0; w < CubeTypes::SIZE_CT; ++w) {
	        	sum += probaByHeight[w];
	        }
	        for (int w = 0; w < CubeTypes::SIZE_CT; ++w) {
	        	probaByHeight[w] /= sum;
	        }

	        for (int k = 0; (k < CubeTypes::SIZE_CT); ++k) {
				proba[k] = probaByHeight[k];
		    }
		
		    sum = 0;

		    // On donne un type de bloc probable
		    bool trouve = false;
		    for (int k = 0; (k < CubeTypes::SIZE_CT) && (!trouve); ++k) {
		        sum += proba[k] * 100;

			    	//std::cout << "random :" << random << " --  sum :" << sum << std::endl;
			    if (random < sum) {
		            numeroBlocSuppose = k;
		            //std::cout << "je devrais etre " << k << std::endl;
		            trouve = true;
			        break;
			    }
		    }  
		

		    map(x, y, z, MapChannels::BLOC) = numeroBlocSuppose;
        } else { 

          map(x, y,z,MapChannels::BLOC) = CubeTypes::AIR;
        }
        map(x,y,z,MapChannels::LIGHT) = 0xFF; 
      }
    }
  }


  // grow three
  growTree(map);
  growRiver(map);
	

  std::string name = "maMap";
  save(name);
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
	cimg_library::CImg<bool> mapObjects = getMapObjects();
	cimg_library::CImg<bool> dilate = mapObjects.get_dilate(3);
	cimg_library::CImg<bool> erode = mapObjects.get_erode(3);
	getMapOutline(dilate,erode);
	//
	//fillVisibleCubes(120, hauteur[119][119], 120 );
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
