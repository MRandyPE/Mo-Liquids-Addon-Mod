#pragma once 
#include "Tile.h"

class MilkTile: public Tile {
public:
	MilkTile(int id, std::string textureName, Material const* material): Tile(id, textureName, material) {
	} 
	virtual bool isLiquidTile();
};

bool MilkTile::isLiquidTile(){
	return true;
}

