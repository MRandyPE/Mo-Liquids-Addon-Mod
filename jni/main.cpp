#include <string.h>
#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include <stdlib.h>
#include <stdint.h>
#include <mcpe.h>
#include <map>
#include <Substrate.h>

class MilkTile: public Tile {
public:
	MilkTile(int id, std::string textureName, Material const* material): Tile(id, textureName, material) {
	} 
	virtual bool isLiquidTile();
};
static MilkTile* Milk;
//bugs with the CreativeInvStarted int
static int worldLoaded = 0;
static Inventory* theInventory;
static void* theGamemode;

static ItemInstance* (*Player$getCarriedItem)(Player*);
static void (*ItemInstance$setId)(ItemInstance*, int);
static bool (*GameMode$isSurvivalType)(void*);
static void (*GameMode$initPlayer_real)(void*, Player*);
static void (*ItemInstance$useOn_real)(ItemInstance*, Player*, int, int, int, signed __int8, float, float, float);
static int (*TileSourse$getTile)(int, int, int);
static int (*TileSource$getData) (int, int, int);
static void (*Level$setTileAndData) (Level*, int, int, int, int, int, int);
std::map <std::string, std::string>* I18n$strings;
static void(*Tile$setDescriptionId)(Tile*, std::string const&);
static void(*Tile$initTiles_real)();
static void (*Minecraft$selectLevel_real)(Minecraft*, std::string const&, std::string const&, LevelSettings const&);
static void (*CreativeInventryScreen$populateTile_real)(void*, Tile*, int, int);
static void (*Inventory$setupDefault_real)(Inventory*);
static void (*Inventory$removeItemInstance)(Inventory*, ItemInstance*);
static void (*Inventory$add)(Inventory*, ItemInstance*);

static void Minecraft$selectLevel_hook(Minecraft* minecraft, std::string const& string1, std::string const& string2, LevelSettings const& settings) {
    (*I18n$strings)["tile.Milk.name"]="Milk";
    Minecraft$selectLevel_real(minecraft, string1, string2, settings);
}

bool MilkTile::isLiquidTile(){
	return true;
}

static void Inventory$setupDefault_hook(Inventory* inventory){
	theInventory = inventory;
	Inventory$setupDefault_real(inventory);
}

static void GameMode$initPlayer_hook(void* gm, Player* player) {
	theGamemode = gm;
	GameMode$initPlayer_real(gm, player);
}

void setItemInstance(ItemInstance* instance, int id, int count, int damage) {
	instance->count = count;
	instance->damage = damage;
	ItemInstance$setId(instance, id);
}

ItemInstance* newItemInstance(int id, int count, int damage) {
	ItemInstance* instance = (ItemInstance*) malloc(sizeof(ItemInstance));
	setItemInstance(instance, id, count, damage);
	return instance;
}

static void Tile$initTiles_hook() {
	Tile$initTiles_real();
	Milk = new MilkTile(25, "flowing_water", &Material::water);
	Tile::tiles[25] = Milk;
    Tile$setDescriptionId(Milk, "Milk");
}

static void CreativeInventryScreen$populateTile_hook(void* creativeInv, Tile* tile, int count, int damage){
	//says creativeInvStarted not in scope of function
	if (worldLoaded == 0) {
		CreativeInventryScreen$populateTile_real(creativeInv, Milk, 1, 0);
		worldLoaded = 1;
	}
	CreativeInventryScreen$populateTile_real(creativeInv, tile, count, damage);
}

void ItemInstance$useOn_hook(ItemInstance* itemStack, Player* player, int x, int y, int z, signed __int8 par1, float par2, float par3, float par4) {
	int itemId = 0;
	int itemDamage = 0;
	if (itemStack != NULL) {
		itemId = itemStack->getId();
		itemDamage = itemStack->getDamageValue();
	}
	int blockId = TileSource$getTile(x, y, z);
	int blockDamage = TileSource$getData(x, y, z);
	if (itemId == 325 && itemDamage == 1) {
		Level$setTileAndData(level, x, y +1, z, 25);
		if (GameMode$isSurvivalType(theGamemode) == true) {
			Inventory$removeItemInstance(theInventory, itemStack);
			ItemInstance* emptyBucket = newItemInstance(325, 1, 0);
			Inventory$add(theInventory, emptyBucket);
		}
	}
	ItemInstance$useOn_real(itemStack, player, x, y, z, par1, par2, par3, par4);
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	void * handle = dlopen("libminecraftpe.so", RTLD_LAZY);

	void* Minecraft$selectLevel = dlsym(RTLD_DEFAULT, "_ZN9Minecraft11selectLevelERKSsS1_RK13LevelSettings");
	void* populateTile = dlsym(RTLD_DEFAULT, "_ZN23CreativeInventoryScreen12populateItemEP4Tileii");
	void* ItemInstance$useOn = dlsym(RTLD_DEFAULT, "_ZN12ItemInstance5useOnEP6Playeriiiafff");
	void* Inventory$setupDefault = dlsym(RTLD_DEFAULT, "_ZN9Inventory12setupDefaultEv");
	void* GameMode$initPlayer = dlsym(RTLD_DEFAULT, "_ZN8GameMode10initPlayerEP6Player");
	
	MSHookFunction((void*) &Tile::initTiles, (void*) &Tile$initTiles_hook, (void**) &Tile$initTiles_real);
	MSHookFunction(Minecraft$selectLevel, (void*) &Minecraft$selectLevel_hook, (void**) &Minecraft$selectLevel_real);
	MSHookFunction(populateTile, (void*) &CreativeInventryScreen_populateTile_hook, (void**) &CreativeInventryScreen_populateTile_real);
	MSHookFunction(ItemInstance$useOn, (void*) &ItemInstance$useOn_hook, (void**) &ItemInstance$useOn_real);
	MSHookFunction(Inventory$setupDefault, (void*) &Inventory$setupDefault_hook, (void**) &Inventory$setupDefault_real);
	MSHookFunction(GameMode$initPlayer, (void*) &GameMode$initPlayer_hook, (void**) &GameMode$initPlayer_real);
	
	GameMode$isSurvivalType = dlsym(RTLD_DEFAULT, "_ZN8GameMode14isSurvivalTypeEv")
	ItemInstance$setId = dlsym(RTLD_DEFAULT, "_ZN12ItemInstance8_setItemEi");
	Inventory$removeItemInstance = dlsym(RTLD_DEFAULT, "_ZN9Inventory18removeItemInstanceEPK12ItemInstance")
	Inventory$add = dlsym(RTLD_DEFAULT, "_ZN9Inventory3addEP12ItemInstance")
	Level$setTileAndData = dlsym(RTLD_DEFAULT, "_ZN5Level14setTileAndDataEiiiiii");
	TileSourse$getTile = dlsym(RTLD_DEFAULT, "_ZN10TileSource7getTileEiii");
	TileSource$getData = dlsym(RTLD_DEFAULT, "_ZN10TileSource7getDataEiii");
	Tile$setDescriptionId = (void(*)(Tile*, std::string const&)) dlsym(RTLD_DEFAULT, "_ZN4Tile16setDescriptionIdERKSs");
	I18n$strings = (std::map <std::string, std::string>*) dlsym(RTLD_DEFAULT, "_ZN4I18n8_stringsE");
    
	return JNI_VERSION_1_2;
}
