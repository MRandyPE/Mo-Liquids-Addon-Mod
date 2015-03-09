#pragma once
#include "TilePos.h"
#include "FullTile.h"

class TileSource {
public:
	virtual void setTileAndData(TilePos const&, FullTile, int);
	virtual int getTile(int, int, int);
	virtual int getData(int, int, int);
};