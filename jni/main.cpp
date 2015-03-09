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
#include "mcpe/TileItem.h"
#include "mcpe/Tile.h"
#include "mcpe/Minecraft.h"
#include "mcpe/Inventory.h"
#include "mcpe/GameMode.h"

class Bucket: public ItemInstance {
public:
	Bucket(int id, int count, int damage): ItemInstance(id, count, damage) {
	}
};

static MilkTile* Milk;
static int worldLoaded = 0;
static Inventory* theInventory;
static GameMode* theGamemode;
static TileSource* theTileSource;

static void (*GameMode$initPlayer_real)(GameMode*, Player*);
static void (*ItemInstance$useOn_real)(ItemInstance*, Player*, int, int, int, signed char, float, float, float);
std::map <std::string, std::string>* I18n$strings;
static void (*Tile$initTiles_real)();
static void (*Tile$prepareRender_real)(TileSource*, int, int, int);
static void (*Minecraft$selectLevel_real)(Minecraft*, std::string const&, std::string const&, LevelSettings const&);
static void (*CreativeInventoryScreen$populateTile_real)(void*, Tile*, int, int);
static void (*Inventory$setupDefault_real)(Inventory*);

static void Minecraft$selectLevel_hook(Minecraft* minecraft, std::string const& string1, std::string const& string2, LevelSettings const& settings) {
    (*I18n$strings)["tile.Milk.name"]="Milk";
    Minecraft$selectLevel_real(minecraft, string1, string2, settings);
}

static void Inventory$setupDefault_hook(Inventory* inventory){
	theInventory = inventory;
	Inventory$setupDefault_real(inventory);
}

static void GameMode$initPlayer_hook(GameMode* gm, Player* player) {
	theGamemode = gm;
	GameMode$initPlayer_real(gm, player);
}

static void Tile$prepareRender_hook(TileSource* tl, int i1, int i2, int i3) {
	theTileSource = tl;
	Tile$prepareRender_real(tl, i1, i2, i3);
}

static void Tile$initTiles_hook() {
	Tile$initTiles_real();
	Milk = new MilkTile(25, "flowing_water", &Material::water);
	Tile::tiles[25] = Milk;
    Milk->setDescriptionId("Milk");
}

static void CreativeInventoryScreen$populateTile_hook(void* creativeInv, Tile* tile, int count, int damage){
	//says creativeInvStarted not in scope of function
	if (worldLoaded == 0) {
		CreativeInventoryScreen$populateTile_real(creativeInv, Milk, 1, 0);
		worldLoaded = 1;
	}
	CreativeInventoryScreen$populateTile_real(creativeInv, tile, count, damage);
}

void ItemInstance$useOn_hook(ItemInstance* itemStack, Player* player, int x, int y, int z, signed char par1, float par2, float par3, float par4) {
	int itemId = 0;
	int itemDamage = 0;
	if (itemStack != NULL) {
		itemId = itemStack->getId();
		itemDamage = itemStack->getDamageValue();
	}
	int blockId = theTileSource->getTile(x, y, z);
	int blockDamage = theTileSource->getData(x, y, z);
	if (itemId == 325 && itemDamage == 1) {
		theTileSource->setTileAndData({x, y + 1, z}, {25, 0}, 3);
		if (theGamemode->isSurvivalType() == true) {
			theInventory->removeItemInstance(itemStack);
			ItemInstance* emptyBucket = new Bucket(325, 1, 0);
			theInventory->add(emptyBucket);
		}
	}
	ItemInstance$useOn_real(itemStack, player, x, y, z, par1, par2, par3, par4);
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	void * handle = dlopen("libminecraftpe.so", RTLD_LAZY);

	void* Minecraft$selectLevel = dlsym(RTLD_DEFAULT, "_ZN9Minecraft11selectLevelERKSsS1_RK13LevelSettings");
	void* populateTile = dlsym(RTLD_DEFAULT, "_ZN23CreativeInventoryScreen12populateItemEP4Tileii");
	void* Inventory$setupDefault = dlsym(RTLD_DEFAULT, "_ZN9Inventory12setupDefaultEv");
	void* ItemInstance$useOn = dlsym(RTLD_DEFAULT, "_ZN12ItemInstance5useOnEP6Playeriiiafff");
	
	MSHookFunction((void*) &Tile::initTiles, (void*) &Tile$initTiles_hook, (void**) &Tile$initTiles_real);
	MSHookFunction((void*) &Tile::prepareRender, (void*) &Tile$prepareRender_hook, (void**) &Tile$prepareRender_real);
	MSHookFunction(Minecraft$selectLevel, (void*) &Minecraft$selectLevel_hook, (void**) &Minecraft$selectLevel_real);
	MSHookFunction(populateTile, (void*) &CreativeInventoryScreen$populateTile_hook, (void**) &CreativeInventoryScreen$populateTile_real);
	MSHookFunction(ItemInstance$useOn, (void*) &ItemInstance$useOn_hook, (void**) &ItemInstance$useOn_real);
	MSHookFunction(Inventory$setupDefault, (void*) &Inventory$setupDefault_hook, (void**) &Inventory$setupDefault_real);
	MSHookFunction((void*) &GameMode::initPlayer, (void*) &GameMode$initPlayer_hook, (void**) &GameMode$initPlayer_real);
	
	I18n$strings = (std::map <std::string, std::string>*) dlsym(RTLD_DEFAULT, "_ZN4I18n8_stringsE");
    
	return JNI_VERSION_1_2;
}
