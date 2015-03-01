#include <string.h>
#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include <stdlib.h>
#include <mcpe.h>
#include <map>
#include <Substrate.h>

class TntTile: public Tile {
public:
	TntTile(int id, std::string textureName, Material const* material): Tile(id, textureName, material) {
	} 
};
static TntTile* TNTx5;
static bool CreativeInvStarted = false;

std::map <std::string, std::string>* I18n$strings;
static void(*Tile$setDescriptionId)(Tile*, std::string const&);
static void(*Tile$initTiles_real)();
static void (*Minecraft$selectLevel_real)(Minecraft*, std::string const&, std::string const&, LevelSettings const&);
static void (*CreativeInventryScreen_populateTile_real)(Tile*, int, int);

static void Minecraft$selectLevel_hook(Minecraft* minecraft, std::string const& string1, std::string const& string2, LevelSettings const& settings) {
    (*I18n$strings)["tile.TNTx5.name"]="§9TNTx5";
    Minecraft$selectLevel_real(minecraft, string1, string2, settings);
}

static void Tile$initTiles_hook() {
	Tile$initTiles_real();
	TNTx5 = new TntTile(25, "cauldron_side", &Material::explosive);
	Tile::tiles[25] = TNTx5;
	TileItem* tileItem = new TileItem(25 - 256);
    Tile$setDescriptionId(TNTx5, "TNTx5");
}

static void CreativeInventryScreen_populateTile_hook(Tile* tile, int count, int damage){
	if (!creativeInvStarted) {
		CreativeInventryScreen_populateTile_real(TNTx5, 1, 0);
		creativeInvStarted = true;
	}
	CreativeInventryScreen_populateTile_real(tile, count, damage);
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
void * handle = dlopen("libminecraftpe.so", RTLD_LAZY);

void* Minecraft$selectLevel = dlsym(RTLD_DEFAULT, "_ZN9Minecraft11selectLevelERKSsS1_RK13LevelSettings");

MSHookFunction((void*) &Tile::initTiles, (void*) &Tile$initTiles_hook, (void**) &Tile$initTiles_real);
MSHookFunction(Minecraft$selectLevel, (void*) &Minecraft$selectLevel_hook, (void**) &Minecraft$selectLevel_real);
void* populateTile = dlsym(RTLD_DEFAULT, "_ZN23CreativeInventoryScreen12populateItemEP4Tileii");
MSHookFunction(populateTile, (void*) &CreativeInventryScreen_populateTile_hook, (void**) &CreativeInventryScreen_populateTile_real);

Tile$setDescriptionId = (void(*)(Tile*, std::string const&)) dlsym(RTLD_DEFAULT, "_ZN4Tile16setDescriptionIdERKSs");
I18n$strings = (std::map <std::string, std::string>*) dlsym(RTLD_DEFAULT, "_ZN4I18n8_stringsE");
    
	return JNI_VERSION_1_2;
}
