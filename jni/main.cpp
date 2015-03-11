#include <string.h>
#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include <stdlib.h>
#include <stdint.h>
#include <map>
#include <Substrate.h>
#include "mcpe/MilkTile.h"
#include "mcpe/TileSource.h"
#include "mcpe/ItemInstance.h"
#include "mcpe/Tile.h"
#include "mcpe/Minecraft.h"
#include "mcpe/Inventory.h"
#include "mcpe/GameMode.h"
#include "mcpe/TileItem.h"
#include "mcpe/Item.h"

MilkTile* Milk;
int worldLoaded = 0;
Inventory* theInventory;
GameMode* theGamemode;
TileSource* theTileSource;
const int MILK_TILE_ID = 25;

GameMode::InitPlayerReal GameMode::initPlayer_real = NULL;

std::map <std::string, std::string>* I18n$strings;
static void (*Tile$initTiles_real)();
static void (*Minecraft$selectLevel_real)(Minecraft*, std::string const&, std::string const&, LevelSettings const&);
static void (*Minecraft$tick_real)(Minecraft*, int, int);
static void (*CreativeInventoryScreen$populateTile_real)(void*, Tile*, int, int);
static void (*Inventory$setupDefault_real)(Inventory*);
static void (*Item$initItems_real)();
static void (*Level$onSourceCreated_real)(Level*, TileSource*);
static void (*ItemInstance$useOn_real)(ItemInstance*, Player*, int, int, int, signed char, float, float, float);

static void Minecraft$selectLevel_hook(Minecraft* minecraft, std::string const& string1, std::string const& string2, LevelSettings const& settings) {
    (*I18n$strings)["tile.Milk.name"]="Milk";
    Minecraft$selectLevel_real(minecraft, string1, string2, settings);
}

static void Minecraft$tick_hook(Minecraft* minecraft, int i1, int i2) {
	Minecraft$tick_real(minecraft, i1, i2);
}

static void Inventory$setupDefault_hook(Inventory* inventory){
	theInventory = inventory;
	Inventory$setupDefault_real(inventory);
}

void GameMode::initPlayer_hook(GameMode* gm, Player* player) {
	theGamemode = gm;
	GameMode::initPlayer_real(gm, player);
}

static void Level$onSourceCreated_hook(Level* level, TileSource* ts) {
	theTileSource = ts;
	Level$onSourceCreated_real(level, ts);
}

static void Tile$initTiles_hook() {
	Tile$initTiles_real();
	Milk = new MilkTile(MILK_TILE_ID, "flowing_water", &Material::water);
	Tile::tiles[MILK_TILE_ID] = Milk;
	TileItem* tileItem = new TileItem(MILK_TILE_ID - 256);
    Milk->setDescriptionId("Milk");
	Milk->setCategory(1);
}

static void Item$initItems_hook() {
	Item$initItems_real();
}

static void CreativeInventoryScreen$populateTile_hook(void* creativeInv, Tile* tile, int count, int damage){
	//says creativeInvStarted not in scope of function
	if (worldLoaded == 0) {
		CreativeInventoryScreen$populateTile_real(creativeInv, Milk, 1, 0);
		worldLoaded = 1;
	}
	CreativeInventoryScreen$populateTile_real(creativeInv, tile, count, damage);
}

static void ItemInstance$useOn_hook(ItemInstance* itemStack, Player* player, int x, int y, int z, signed char ch1, float f1, float f2, float f3) {
	ItemInstance$useOn_real(itemStack, player, x, y, z, ch1, f1, f2, f3);
	if(itemStack->item->id == 325 && itemStack->damage == 1) {
		theTileSource->setTileAndData(x, y + 1, z, {25, 0}, 3);
		/*
		if(theGamemode->isSurvivalType() == true) {
			theInventory->removeItemInstance(itemStack);
			ItemInstance* emptyBucket = new ItemInstance(325, 1, 0);
			theInventory->add(emptyBucket);
		}*/
	}
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	void * handle = dlopen("libminecraftpe.so", RTLD_LAZY);

	void* Minecraft$selectLevel = dlsym(RTLD_DEFAULT, "_ZN9Minecraft11selectLevelERKSsS1_RK13LevelSettings");
	void* Minecraft$tick = dlsym(RTLD_DEFAULT, "_ZN9Minecraft4tickEii");
	void* populateTile = dlsym(RTLD_DEFAULT, "_ZN23CreativeInventoryScreen12populateItemEP4Tileii");
	void* Inventory$setupDefault = dlsym(RTLD_DEFAULT, "_ZN9Inventory12setupDefaultEv");
	void* Level$onSourceCreated = dlsym(RTLD_DEFAULT, "_ZN5Level15onSourceCreatedEP10TileSource");
	void* ItemInstance$useOn = dlsym(RTLD_DEFAULT, "_ZN12ItemInstance5useOnEP6Playeriiiafff");
	
	MSHookFunction((void*) &Item::initItems, (void*) &Item$initItems_hook, (void**) &Item$initItems_real);
	MSHookFunction((void*) &Tile::initTiles, (void*) &Tile$initTiles_hook, (void**) &Tile$initTiles_real);
	MSHookFunction(ItemInstance$useOn, (void*) &ItemInstance$useOn_hook, (void**) &ItemInstance$useOn_real);
	MSHookFunction(Minecraft$selectLevel, (void*) &Minecraft$selectLevel_hook, (void**) &Minecraft$selectLevel_real);
	MSHookFunction(populateTile, (void*) &CreativeInventoryScreen$populateTile_hook, (void**) &CreativeInventoryScreen$populateTile_real);
	MSHookFunction(Minecraft$tick, (void*) &Minecraft$tick_hook, (void**) &Minecraft$tick_real);
	MSHookFunction(Inventory$setupDefault, (void*) &Inventory$setupDefault_hook, (void**) &Inventory$setupDefault_real);
	MSHookFunction((void*) &GameMode::initPlayer, (void*) &GameMode::initPlayer_hook, (void**) &GameMode::initPlayer_real);
	MSHookFunction(Level$onSourceCreated, (void*) &Level$onSourceCreated_hook, (void**) &Level$onSourceCreated_real);
	
	I18n$strings = (std::map <std::string, std::string>*) dlsym(RTLD_DEFAULT, "_ZN4I18n8_stringsE");
    
	return JNI_VERSION_1_2;
}
