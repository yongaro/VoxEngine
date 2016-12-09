#include "SnowValleyGenerator.hpp"
#include "VoxMap.hpp"

SnowValleyGenerator::~SnowValleyGenerator() {}

  int SnowValleyGenerator::getNbBloc(int nb) const {
  if (nb < 1) {
    return -1;
  } else if (nb > 3) {
    return 2;
  } else {
    return 0;
  }
}


  double SnowValleyGenerator::getProbaAir(int height, int maxHeight) const {
	return std::max((double(height) / maxHeight) - 0.5 * maxHeight, 0.0);
}

  double SnowValleyGenerator::getProbaAdminium(int height, int maxHeight) const {
	double u = double(height) / maxHeight;
	if (u < 0.2) {
		return 0.1;
	} else {
		return 0.0;
	}
}

  double SnowValleyGenerator::getProbaCactus(int height, int maxHeight) const {
	double u = double(height) / maxHeight;
	if (u > 0.9) {
		return 0.0;
	} else {
		return 0.0;
	}
}

  double SnowValleyGenerator::getProbaCoal(int height, int maxHeight) const {
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


  double SnowValleyGenerator::getProbaDirt(int height, int maxHeight) const {
	double u = double(height) / maxHeight;
	if (u > 0.7) {
		return 0.2;
	} else {
		return 0.15;
	}
}

  double SnowValleyGenerator::getProbaFolliage(int height, int maxHeight) const {
	return 0.0;
}

  double SnowValleyGenerator::getProbaGlowstone(int height, int maxHeight) const {
	return 0.0;
}

  double SnowValleyGenerator::getProbaGold(int height, int maxHeight) const {
	return getProbaCoal(height * 0.5, maxHeight) * 0.1;
}

  double SnowValleyGenerator::getProbaDiamond(int height, int maxHeight) const {
	return getProbaGold(height * 0.7, maxHeight);
}

  double SnowValleyGenerator::getProbaGrass(int height, int maxHeight) const {
	return 0;
}

  double SnowValleyGenerator::getProbaIron(int height, int maxHeight) const {
	return getProbaCoal(height, maxHeight) * 0.5;
}

  double SnowValleyGenerator::getProbaSand(int height, int maxHeight) const {
	double u = double(height) / maxHeight;
	if (u > 0.9) {
		return 0.0;
	} else {
		return 0;
	}
}

  double SnowValleyGenerator::getProbaSnow(int height, int maxHeight) const {
	double u = double(height) / maxHeight;
	if (height == maxHeight) {
		return 0.8;
	} else if (u > 0.8) {
		return 0.7;
	} else {
		return 0.0;
	}
}

  double SnowValleyGenerator::getProbaSnowBlock(int height, int maxHeight) const {
	double u = double(height) / maxHeight;
	if (height == maxHeight) {
		return 0.2;
	} else if (u > 0.9) {
		return 0.01;
	} else {
		return 0.0;
	}
}

  double SnowValleyGenerator::getProbaStone(int height, int maxHeight) const {
	double u = double(height) / maxHeight;
	if (u > 0.9) {
		return 0.005;
	} else {
		return 0.1;
	}
}

  double SnowValleyGenerator::getProbaWater(int height, int maxHeight, int superMaxHeight) const {
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


  double SnowValleyGenerator::getProbaWood(int height, int maxHeight) const {
	double u = double(height) / maxHeight;
	if (u > 0.9) {
		return 0.01;
	} else {
		return 0;
	}
}

 std::vector<double> SnowValleyGenerator::getProbaBloc(int depth, int maxHeight, int superMaxHeight) const {
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

  void SnowValleyGenerator::growRiver(cimg_library::CImg<unsigned char>& SnowValley) const {
	int hSize = SnowValley.height();
	int wSize = SnowValley.width();
	int dSize = SnowValley.depth();

	for (int y = hSize-1; y > 1; --y ) {
	  	for (int z = 1; z < (dSize - 1); ++z ) {
	    	for (int x = 1; x < (wSize - 1); ++x ) {
	    		if (SnowValley(x, y, z, MapChannels::BLOC) == CubeTypes::WATER) {
	    			for (int w = 0; w < 9; ++w) {
    					if (SnowValley(x - 1 + (w / 3), y - 1,  z - 1 + (w %3), MapChannels::BLOC) == CubeTypes::AIR) {
	    					SnowValley(x - 1 + (w / 3), y - 1,  z - 1 + (w %3), MapChannels::BLOC) = CubeTypes::WATER;
	    				}	
    				}
	    		}
			}
		}
	}
}

   void SnowValleyGenerator::diffuseFolliage(cimg_library::CImg<unsigned char>& SnowValley) const {
	int hSize = SnowValley.height();
	int wSize = SnowValley.width();
	int dSize = SnowValley.depth();
	cimg_library::CImg<bool> maskFolliage = cimg_library::CImg<bool>(wSize, hSize, dSize, 1, false);

   	for (int i = 0; i < wSize; ++i) {
   		for (int j = 0; j < hSize; ++j) {
   			for (int k = 0; k < dSize; ++k) {
   				if (SnowValley(i,j,k, MapChannels::BLOC) == CubeTypes::FOLLIAGE) {
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
    					if ((SnowValley(x - 1 + (w / 3), y - 1 + (w / 9),  z - 1 + (w %3), MapChannels::BLOC) == CubeTypes::AIR) && (random < 45)) {
	    					SnowValley(x - 1 + (w / 3), y - 1 + (w / 9),  z - 1 + (w %3), MapChannels::BLOC) = CubeTypes::FOLLIAGE;
	    				}	
    				}
	    		}
	    	}
	    }
	}
}

   void SnowValleyGenerator::growTree (cimg_library::CImg<unsigned char>& SnowValley) const {
	int hSize = SnowValley.height();
	int wSize = SnowValley.width();
	int dSize = SnowValley.depth();

	for (int y = 0; y < (hSize-1); ++y ) {
	  	for (int z = 1; z < (dSize - 1); ++z ) {
	    	for (int x = 1; x < (wSize - 1); ++x ) {
	    		if (SnowValley(x, y, z, MapChannels::BLOC) == CubeTypes::WOOD && SnowValley(x, y + 1, z, MapChannels::BLOC) == CubeTypes::AIR) {
	    			int random = rand() % 100;
	    			if (SnowValley(x, y - 1, z, MapChannels::BLOC) != CubeTypes::WOOD) {
	    				random = 90;
	    			}

	    			if (random < 35) {
	    				SnowValley(x, y + 1, z, MapChannels::BLOC) = CubeTypes::FOLLIAGE;
	    				
	    				random = rand() % 4;
	    				if (SnowValley(x + 1, y + 1,  z + 1, MapChannels::BLOC) == CubeTypes::AIR && random < 3) {
	    					SnowValley(x + 1, y + 1, z + 1, MapChannels::BLOC) = CubeTypes::FOLLIAGE;
	    				}

	    				random = rand() % 4;
	    				if (SnowValley(x + 1, y + 1,  z - 1, MapChannels::BLOC) == CubeTypes::AIR && random < 2) {
	    					SnowValley(x + 1, y + 1, z - 1, MapChannels::BLOC) = CubeTypes::FOLLIAGE;
	    				}
	    				
	    				if (SnowValley(x + 1, y + 1,  z , MapChannels::BLOC) == CubeTypes::AIR) {
	    					SnowValley(x + 1, y + 1, z, MapChannels::BLOC) = CubeTypes::FOLLIAGE;
	    				}


	    				// 
	    				if (SnowValley(x , y + 1,  z, MapChannels::BLOC) == CubeTypes::AIR) {
	    					SnowValley(x , y + 1, z, MapChannels::BLOC) = CubeTypes::FOLLIAGE;
	    				}
	    				
	    				if (SnowValley(x - 1, y + 1,  z , MapChannels::BLOC) == CubeTypes::AIR) {
	    					SnowValley(x - 1, y + 1, z, MapChannels::BLOC) = CubeTypes::FOLLIAGE;
	    				}

	    				random = rand() % 4;
	    				if (SnowValley(x - 1, y + 1,  z + 1, MapChannels::BLOC) == CubeTypes::AIR && random < 3) {
	    					SnowValley(x - 1, y + 1, z + 1, MapChannels::BLOC) = CubeTypes::FOLLIAGE;
	    				}
	    				

	    			} else {
	    				SnowValley(x, y + 1, z, MapChannels::BLOC) = CubeTypes::WOOD;
	    				for (int w = 0; w < 9; ++w) {
	    					random = rand() % 4;
		    				if (SnowValley(x - 1 + (w / 3), y + 1,  z - 1 + (w %3), MapChannels::BLOC) == CubeTypes::AIR && random < 2) {
		    					SnowValley(x - 1 + (w / 3), y + 1,  z - 1 + (w %3), MapChannels::BLOC) = CubeTypes::FOLLIAGE;
		    				}	
	    				}
	    			}
	    			//diffuseFolliage(SnowValley);
	    		}

	    	}
	    }
	}
}


 void SnowValleyGenerator::fill(cimg_library::CImg<unsigned char>& SnowValley) {
	int hSize = SnowValley.height();
	int wSize = SnowValley.width();
	int dSize = SnowValley.depth();


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
      		SnowValley(x, y, z, MapChannels::BLOC) = CubeTypes::ADMINIUM; 
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
		

		    SnowValley(x, y, z, MapChannels::BLOC) = numeroBlocSuppose;
        } else { 

          SnowValley(x, y,z,MapChannels::BLOC) = CubeTypes::AIR;
        }
        SnowValley(x,y,z,MapChannels::LIGHT) = 0xFF; 
      }
    }
  }


  // grow three
  growTree(SnowValley);
  growRiver(SnowValley);
}