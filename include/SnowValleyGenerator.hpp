#ifndef SNOWVALLEYGENERATOR_HPP
#define SNOWVALLEYGENERATOR_HPP

#include <MapGenerator.hpp>
#include <vector>

class SnowValleyGenerator : public MapGenerator {
public:
	 virtual void fill (VoxImage<unsigned char>& SnowValley);

	virtual  int getNbBloc (int nb) const;
	virtual  double getProbaAir (int height, int maxHeight) const;
	virtual  double getProbaAdminium (int height, int maxHeight) const;
	virtual  double getProbaCactus (int height, int maxHeight) const;
	virtual  double getProbaCoal (int height, int maxHeight) const;
	virtual  double getProbaDirt (int height, int maxHeight) const;
	virtual  double getProbaFolliage (int height, int maxHeight) const;
	virtual  double getProbaGlowstone (int height, int maxHeight) const;
	virtual  double getProbaGold (int height, int maxHeight) const;
	virtual  double getProbaDiamond (int height, int maxHeight) const;
	virtual  double getProbaGrass (int height, int maxHeight) const;
	virtual  double getProbaIron (int height, int maxHeight) const;
	virtual  double getProbaSand (int height, int maxHeight) const;
	virtual  double getProbaSnow (int height, int maxHeight) const;
	virtual  double getProbaSnowBlock (int height, int maxHeight) const;
	virtual  double getProbaStone (int height, int maxHeight) const;

	virtual  double getProbaWater (int height, int maxHeight, int superMaxHeight) const;
	virtual  double getProbaWood (int height, int maxHeight) const;

	virtual std::vector<double> getProbaBloc (int depth, int maxHeight, int superMaxHeight) const;

	virtual  void growRiver(VoxImage<unsigned char>& SnowValley) const;
	virtual void diffuseFolliage (VoxImage<unsigned char>& SnowValley) const;
	virtual  void growTree (VoxImage<unsigned char>& SnowValley) const;

	virtual ~SnowValleyGenerator();
};
#endif
