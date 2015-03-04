#include <string.h>
#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include <stdlib.h>
#include <mcpe.h>
#include <map>
#include <Substrate.h>

class MilkTile: public Tile {
public:
	MilkTile(int id, std::string textureName, Material const* material): Tile(id, textureName, material) {
	} 
};
static MilkTile* Milk;
//bugs with the CreativeInvStarted int
static int worldLoaded = 0;

static void (*GameMode$useItemOn_real)(void*, Player*, Level*, ItemInstance*, int, int, int, int, void*);
static int (*Level$getTile)(Level*, int, int, int);
static int (*Level$getData) (Level*, int, int, int);
static void (*Level$setTileAndData) (Level*, int, int, int, int, int, int);
std::map <std::string, std::string>* I18n$strings;
static void(*Tile$setDescriptionId)(Tile*, std::string const&);
static void(*Tile$isLiquidTile)(Tile*);
static void(*Tile$initTiles_real)();
static void (*Minecraft$selectLevel_real)(Minecraft*, std::string const&, std::string const&, LevelSettings const&);
static void (*CreativeInventryScreen$populateTile_real)(Tile*, int, int);

static void Minecraft$selectLevel_hook(Minecraft* minecraft, std::string const& string1, std::string const& string2, LevelSettings const& settings) {
    (*I18n$strings)["tile.Milk.name"]="Milk";
    Minecraft$selectLevel_real(minecraft, string1, string2, settings);
}

static void Tile$initTiles_hook() {
	Tile$initTiles_real();
	Milk = new MilkTile(25, "flowing_water", &Material::water);
	Tile::tiles[25] = Milk;
    Tile$setDescriptionId(Milk, "Milk");
    Tile$isLiquidTile(Milk);
}

static void CreativeInventryScreen$populateTile_hook(CreativeInventoryScreen* creativeInv, Tile* tile, int count, int damage){
	//says creativeInvStarted not in scope of function
	if (worldLoaded == 0) {
		CreativeInventryScreen$populateTile_real(creativeInv, Milk, 1, 0);
		worldLoaded = 1;
	}
	CreativeInventryScreen$populateTile_real(creativeInv, tile, count, damage);
}

void GameMode$useItemOn_hook(void* gamemode, Player* player, Level* level, ItemInstance* itemStack, int x, int y, int z, int side, void* vec3) {
	int itemId = 0;
	int itemDamage = 0;
	if (itemStack != NULL) {
		itemId = itemStack->getId();
		itemDamage = itemStack->getDamageValue();
	}
	int blockId = Level$getTile(level, x, y, z);
	int blockDamage = Level$getData(level, x, y, z);
	if (itemId == 325 && itemDamage == 1) {
		Level$setTileAndData(level, x, y +1, z, 25);
	}
	GameMode$useItemOn_real(gamemode, player, level, itemStack, x, y, z, side, vec3);
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	void * handle = dlopen("libminecraftpe.so", RTLD_LAZY);

	void* Minecraft$selectLevel = dlsym(RTLD_DEFAULT, "_ZN9Minecraft11selectLevelERKSsS1_RK13LevelSettings");
	void* GameMode$useItemOn = dlsym(RTLD_DEFAULT, "_ZN8GameMode9useItemOnEP6PlayerP5LevelP12ItemInstanceiiiiRK4Vec3")

	MSHookFunction((void*) &Tile::initTiles, (void*) &Tile$initTiles_hook, (void**) &Tile$initTiles_real);
	MSHookFunction(Minecraft$selectLevel, (void*) &Minecraft$selectLevel_hook, (void**) &Minecraft$selectLevel_real);
	void* populateTile = dlsym(RTLD_DEFAULT, "_ZN23CreativeInventoryScreen12populateItemEP4Tileii");
	MSHookFunction(populateTile, (void*) &CreativeInventryScreen_populateTile_hook, (void**) &CreativeInventryScreen_populateTile_real);
	MSHookFunction(GameMode$useItemOn, (void*) &GameMode$useItemOn_hook, (void**) &GameMode$useItemOn_real);
	
	Level$setTileAndData = dlsym(RTLD_DEFAULT, "_ZN5Level14setTileAndDataEiiiiii");
	Level$getTile = dlsym(RTLD_DEFAULT, "_ZN5Level7getTileEiii");
	Level$getData = dlsym(RTLD_DEFAULT, "_ZN5Level7getDataEiii");
	Tile$setDescriptionId = (void(*)(Tile*, std::string const&)) dlsym(RTLD_DEFAULT, "_ZN4Tile16setDescriptionIdERKSs");
	Tile$isLiquidTile = (void(*)(Tile*)) dlsym(RTLD_DEFAULT, "_ZNK4Tile12isLiquidTileEv");
	I18n$strings = (std::map <std::string, std::string>*) dlsym(RTLD_DEFAULT, "_ZN4I18n8_stringsE");
    
	return JNI_VERSION_1_2;
}
