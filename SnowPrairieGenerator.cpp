#include "SnowPrairieGenerator.hpp"
#include "VoxMap.hpp"
#include <time.h>

#include <iostream>


SnowPrairieGenerator::~SnowPrairieGenerator() {}

  int SnowPrairieGenerator::getNbBloc(int nb) const {
  	int random = rand() % 100;
  if (random < 5) {
    return -1;
  } else if (random > 55) {
    return 1;
  } else {
    return 0;
  }
}


  double SnowPrairieGenerator::getProbaAir(int height, int maxHeight) const {
	return std::max((double(height) / maxHeight) - 0.5 * maxHeight, 0.0);
}

  double SnowPrairieGenerator::getProbaAdminium(int height, int maxHeight) const {
	double u = double(height) / maxHeight;
	if (u < 0.2) {
		return 0.1;
	} else {
		return 0.0;
	}
}

  double SnowPrairieGenerator::getProbaCactus(int height, int maxHeight) const {
	double u = double(height) / maxHeight;
	if (u > 0.9) {
		return 0.0;
	} else {
		return 0.0;
	}
}

  double SnowPrairieGenerator::getProbaCoal(int height, int maxHeight) const {
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


  double SnowPrairieGenerator::getProbaDirt(int height, int maxHeight) const {
	double u = double(height) / maxHeight;
	if (u > 0.7) {
		return 0.2;
	} else {
		return 0.15;
	}
}

  double SnowPrairieGenerator::getProbaFolliage(int height, int maxHeight) const {
	return 0.0;
}

  double SnowPrairieGenerator::getProbaGlowstone(int height, int maxHeight) const {
	return 0.0;
}

  double SnowPrairieGenerator::getProbaGold(int height, int maxHeight) const {
	return getProbaCoal(height * 0.5, maxHeight) * 0.1;
}

  double SnowPrairieGenerator::getProbaDiamond(int height, int maxHeight) const {
	return getProbaGold(height * 0.7, maxHeight);
}

  double SnowPrairieGenerator::getProbaGrass(int height, int maxHeight) const {
	return 0;
}

  double SnowPrairieGenerator::getProbaIron(int height, int maxHeight) const {
	return getProbaCoal(height, maxHeight) * 0.5;
}

  double SnowPrairieGenerator::getProbaSand(int height, int maxHeight) const {
	double u = double(height) / maxHeight;
	if (u > 0.9) {
		return 0.0;
	} else {
		return 0;
	}
}

  double SnowPrairieGenerator::getProbaSnow(int height, int maxHeight) const {
	double u = double(height) / maxHeight;
	if (height == maxHeight) {
		return 0.9;
	} else if (u > 0.8) {
		return 0.8;
	} else {
		return 0.0;
	}
}

  double SnowPrairieGenerator::getProbaSnowBlock(int height, int maxHeight) const {
		double u = double(height) / maxHeight;
	if (height == maxHeight) {
		return 0.2;
	} else if (u > 0.9) {
		return 0.01;
	} else {
		return 0.0;
	}
}

  double SnowPrairieGenerator::getProbaStone(int height, int maxHeight) const {
	double u = double(height) / maxHeight;
	if (u > 0.9) {
		return 0.001;
	} else {
		return 0.1;
	}
}

  double SnowPrairieGenerator::getProbaWater(int height, int maxHeight, int superMaxHeight) const {
	double u = double(height) / maxHeight;
	//double v = double(height) / superMaxHeight;
	if (u > 1) { std::cout<< u << std::endl; }
	if ((u < 8)) {
		return 0.001;
	} else {
		return 0.0;
	}
}


  double SnowPrairieGenerator::getProbaWood(int height, int maxHeight) const {
	double u = double(height) / maxHeight;
	if (u > 0.9) {
		return 0.01;
	} else {
		return 0;
	}
}

 std::vector<double> SnowPrairieGenerator::getProbaBloc(int depth, int maxHeight, int superMaxHeight) const {
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

  void SnowPrairieGenerator::growRiver(VoxImage<unsigned char>& map) const {
	int hSize = map.height();
	int wSize = map.width();
	int dSize = map.depth();

	for (int y = hSize-1; y > 1; --y ) {
	  	for (int z = 1; z < (dSize - 1); ++z ) {
	    	for (int x = 1; x < (wSize - 1); ++x ) {
	    		if (map(x, y, z) == CubeTypes::WATER) {
	    			for (int w = 0; w < 9; ++w) {
    					if (map(x - 1 + (w / 3), y - 1,  z - 1 + (w %3)) == CubeTypes::AIR) {
	    					map(x - 1 + (w / 3), y - 1,  z - 1 + (w %3)) = CubeTypes::WATER;
	    				}
	    				if ((map(x - 1 + (w / 3), y,  z - 1 + (w %3)) == CubeTypes::AIR) && ((rand() % 100 > 50))) {
	    					map(x - 1 + (w / 3), y,  z - 1 + (w %3)) = CubeTypes::WATER;
	    				}
    				}
	    		}
			}
		}
	}
}

   void SnowPrairieGenerator::diffuseFolliage(VoxImage<unsigned char>& map) const {
	int hSize = map.height();
	int wSize = map.width();
	int dSize = map.depth();
	VoxImage<bool> maskFolliage = VoxImage<bool>(wSize, hSize, dSize, 1, false);

   	for (int i = 0; i < wSize; ++i) {
   		for (int j = 0; j < hSize; ++j) {
   			for (int k = 0; k < dSize; ++k) {
   				if (map(i,j,k) == CubeTypes::FOLLIAGE) {
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
    					if ((map(x - 1 + (w / 3), y - 1 + (w / 9),  z - 1 + (w %3)) == CubeTypes::AIR) && (random < 45)) {
	    					map(x - 1 + (w / 3), y - 1 + (w / 9),  z - 1 + (w %3)) = CubeTypes::FOLLIAGE;
	    				}
    				}
	    		}
	    	}
	    }
	}
}

   void SnowPrairieGenerator::growTree (VoxImage<unsigned char>& map) const {
	int hSize = map.height();
	int wSize = map.width();
	int dSize = map.depth();

	for (int y = 0; y < (hSize-1); ++y ) {
	  	for (int z = 1; z < (dSize - 1); ++z ) {
	    	for (int x = 1; x < (wSize - 1); ++x ) {
	    		if (map(x, y, z) == CubeTypes::WOOD && map(x, y + 1, z) == CubeTypes::AIR) {
	    			int random = rand() % 100;
	    			if (map(x, y - 1, z) != CubeTypes::WOOD) {
	    				random = 90;
	    			}

	    			if (random < 35) {
	    				map(x, y + 1, z) = CubeTypes::FOLLIAGE;

	    				random = rand() % 4;
	    				if (map(x + 1, y + 1,  z + 1) == CubeTypes::AIR && random < 3) {
	    					map(x + 1, y + 1, z + 1) = CubeTypes::FOLLIAGE;
	    				}

	    				random = rand() % 4;
	    				if (map(x + 1, y + 1,  z - 1) == CubeTypes::AIR && random < 2) {
	    					map(x + 1, y + 1, z - 1) = CubeTypes::FOLLIAGE;
	    				}

	    				if (map(x + 1, y + 1,  z) == CubeTypes::AIR) {
	    					map(x + 1, y + 1, z) = CubeTypes::FOLLIAGE;
	    				}


	    				//
	    				if (map(x , y + 1,  z) == CubeTypes::AIR) {
	    					map(x , y + 1, z) = CubeTypes::FOLLIAGE;
	    				}

	    				if (map(x - 1, y + 1,  z) == CubeTypes::AIR) {
	    					map(x - 1, y + 1, z) = CubeTypes::FOLLIAGE;
	    				}

	    				random = rand() % 4;
	    				if (map(x - 1, y + 1,  z + 1) == CubeTypes::AIR && random < 3) {
	    					map(x - 1, y + 1, z + 1) = CubeTypes::FOLLIAGE;
	    				}


	    			} else {
	    				map(x, y + 1, z) = CubeTypes::WOOD;
	    				for (int w = 0; w < 9; ++w) {
	    					random = rand() % 4;
		    				if (map(x - 1 + (w / 3), y + 1,  z - 1 + (w %3)) == CubeTypes::AIR && random < 2) {
		    					map(x - 1 + (w / 3), y + 1,  z - 1 + (w %3)) = CubeTypes::FOLLIAGE;
		    				}
	    				}
	    			}
	    			//diffuseFolliage(map);
	    		}

	    	}
	    }
	}
}


 void SnowPrairieGenerator::fill(VoxImage<unsigned char>& map) {
	int hSize = map.height();
	int wSize = map.width();
	int dSize = map.depth();

	srand(time(NULL));

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
      		map(x, y, z) = CubeTypes::ADMINIUM;
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


		    map(x, y, z) = numeroBlocSuppose;
        } else {

          map(x, y,z) = CubeTypes::AIR;
        }
        //map(x,y,z) = 0xFF;
      }
    }
  }


  // grow three

  growRiver(map);
  growTree(map);
}
