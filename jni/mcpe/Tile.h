#pragma once
#include <string>
#include <vector>
#include <functional>
#include <GLES/gl.h>
#include "TileSource.h"
#include "TilePos.h"
#include "Material.h"
#include "ItemInstance.h"

class AABB;
class Random;
class Player;
class Entity;
class TextureUVCoordinateSet;
class Vec3;
class Mob;
class Level;

class Tile {
public:
	// inner classes
	enum SoundType {
	};

	// fields
	char filler[0x8c];

	// constructors
	Tile(int, Material const*);
	Tile(int, TextureUVCoordinateSet, Material const*);
	Tile(int, std::string, Material const*);
	// virtual functions
	virtual ~Tile(); // 2
	virtual void onFertilized(TileSource*, int, int, int); // 4
	virtual void getShape(TileSource*, int, int, int, AABB&, bool); // 5
	virtual void getShape(unsigned char, AABB&, bool); // 6
	virtual bool isObstructingChests(TileSource*, int, int, int); // 7
	virtual void shouldRenderFace(TileSource*, int, int, int, signed char, AABB const&) const; // 8
	virtual void getTexture(signed char); // 9
	virtual void getTexture(signed char, int); // 10
	virtual void getTexture(TileSource*, int, int, int, signed char); // 11
	virtual void getTessellatedUVs(); // 12
	virtual void getCarriedTexture(signed char, int); // 13
	virtual void getAABB(TileSource*, int, int, int, AABB&, int, bool, int); // 14
	virtual void addAABBs(TileSource*, int, int, int, AABB const*, std::vector<AABB, std::allocator<AABB> >&); // 15
	virtual void getTileAABB(TileSource*, int, int, int, AABB&); // 16
	virtual bool isPathfindable(TileSource*, int, int, int); // 17
	virtual bool mayPick(); // 18
	virtual bool mayPick(int, bool); // 19
	virtual bool mayPlace(TileSource*, int, int, int, signed char); // 20
	virtual bool mayPlace(TileSource*, int, int, int); // 21
	virtual void getTickDelay(); // 22
	virtual void tick(TileSource*, int, int, int, Random*); // 23
	virtual void animateTick(TileSource*, int, int, int, Random*); // 24
	virtual void destroy(TileSource*, int, int, int, int); // 25
	virtual void playerWillDestroy(Player*, int, int, int, int); // 26
	virtual void neighborChanged(TileSource*, int, int, int, int, int, int); // 27
	virtual void onPlace(TileSource*, int, int, int); // 28
	virtual void onRemove(TileSource*, int, int, int); // 29
	virtual void getSecondPart(TileSource&, TilePos const&, TilePos&); // 30
	virtual void onGraphicsModeChanged(bool, bool); // 31
	virtual void getResource(int, Random*); // 32
	virtual void getResourceCount(Random*); // 33
	virtual void getDestroyProgress(Player*); // 34
	virtual void spawnResources(TileSource*, int, int, int, int, float); // 35
	virtual void spawnBurnResources(TileSource*, float, float, float); // 36
	virtual float getExplosionResistance(Entity*); // 37
	virtual void clip(TileSource*, int, int, int, Vec3 const&, Vec3 const&, bool, int); // 38
	virtual void wasExploded(TileSource*, int, int, int); // 39
	virtual void use(Player*, int, int, int); // 40
	virtual void stepOn(Entity*, int, int, int); // 41
	virtual void fallOn(TileSource*, int, int, int, Entity*, float); // 42
	virtual void getPlacementDataValue(Mob*, int, int, int, signed char, float, float, float, int); // 43
	virtual void prepareRender(TileSource*, int, int, int); // 44
	virtual void attack(Player*, int, int, int); // 45
	virtual void handleEntityInside(TileSource*, int, int, int, Entity*, Vec3&); // 46
	virtual int getColor(int); // 47
	virtual int getColor(TileSource*, int, int, int); // 48
	virtual bool isSignalSource(); // 49
	virtual int getSignal(TileSource*, int, int, int); // 50
	virtual int getSignal(TileSource*, int, int, int, int); // 51
	virtual int getDirectSignal(TileSource*, int, int, int, int); // 52
	virtual bool entityInside(TileSource*, int, int, int, Entity*); // 53
	virtual void playerDestroy(Player*, int, int, int, int); // 54
	virtual bool canSurvive(TileSource*, int, int, int); // 55
	virtual std::string getName() const; // 56
	virtual std::string getDescriptionId() const; // 57
	virtual std::string getDescriptionId(ItemInstance const*) const; // 58
	virtual std::string getTypeDescriptionId(int); // 59
	virtual void setDescriptionId(std::string const&); // 60
	virtual void triggerEvent(TileSource*, int, int, int, int, int); // 61
	virtual TextureUVCoordinateSet getTextureNum(int); // 62
	virtual void getMobToSpawn(TileSource&, TilePos const&) const; // 63
	virtual int getIconYOffset() const; // 64
	virtual void setCategory(int);
	virtual void setShape(float, float, float, float, float, float); // 65
	virtual void setSoundType(Tile::SoundType const&); // 66
	virtual void setLightBlock(int); // 67
	virtual void setLightEmission(float); // 68
	virtual void setExplodeable(float); // 69
	virtual void setDestroyTime(float); // 70
	virtual void setTicking(bool); // 71
	virtual void getSpawnResourcesAuxValue(int); // 72
	virtual void init(); // 73
	virtual bool isLiquidTile();

	// static fields
	static Tile* tiles[255];

	// static methods
	static void initTiles();
};