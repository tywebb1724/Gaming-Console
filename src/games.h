#ifndef GAMES_H
#define GAMES_H

#include "raylib.h"

//Max amount of games console can hold
#define MAX_GAMES 80
//Size of the current games library
#define GAMES_LEN Games_GetLength()
//Macros for games
#define GAMES_ON_SCREEN 5
#define TITLE_MAX_LEN 128
#define MIN_GAMES_IN_CATEG 7
//String lengths
#define CATEG_STR_LEN 30
#define SAVE_PATH_LEN 512
#define TEMP_PATH_LEN 256
#define COMMAND_STR_LEN 1024
//Index for the current game
#define CURRENT_GAME 3
//Paths for libretro consoles console
#define PATH_GBA "assets/cores/mgba_libretro.so"
#define PATH_NES "assets/cores/fceumm_libretro.so"
#define PATH_GENESIS "assets/cores/genesis_plus_gx_libretro.so"
#define PATH_SNES "assets/cores/bsnes_libretro.so"
#define PATH_PS1 "assets/cores/swanstation_libretro.so"
#define PATH_GAMEBOY "assets/cores/gambatte_libretro.so"
#define PATH_N64 "assets/cores/parallel_n64_libretro.so"
#define PATH_LYNX "assets/cores/handy_libretro.so"
#define PATH_NGPC "assets/cores/mednafen_ngp_libretro.so"
#define PATH_TG16 "assets/cores/mednafen_pce_fast_libretro.so"
#define PATH_ARCADE "assets/cores/fbneo_libretro.so"
#define PATH_PRBOOM "assets/cores/prboom_libretro.so"
//Commands for external consoles
#define PATH_GAMECUBE "org.DolphinEmu.Adolphin-emu"
#define PATH_PSP "org.ppsspp.PPSSPP"
#define PATH_DS "net.kuribo64.melonDS"
#define PATH_DREAMCAST "org.flycast.Flycast"
#define PATH_SATURN "io.github.strikerx3.ymir"\
//Process names for external consoles
#define PROCESS_MELON "melonDS"
#define PROCESS_DOLPHIN "dolphin-emu"
#define PROCESS_YMIR "ymir-sdl3"
#define PROCESS_FLYCAST "Flycast-rend"
#define PROCESS_PPSSPP "PPSSPPSDL"
//M3U Playlist Scanner
#define MAX_M3U_SKIP_ENTRIES 64
#define M3U_LINE_LEN 512
#define M3U_MAX_FILE_LEN 256
//Save Mapping & Snapshots
#define MAP_FILE_LINE_LEN 600
#define MAX_SNAPSHOT_ENTRIES 256
#define SNAPSHOT_NAME_LEN 256
//ISO9660 File System Definitions
#define ISO_PVD_OFFSET 0x8000
#define ISO_SECTOR_SIZE 2048
#define ISO_PVD_ROOT_RECORD_OFFSET 156
#define ISO_DIR_REC_LBA_OFFSET 2
#define ISO_DIR_REC_LEN_OFFSET 10
#define ISO_DIR_REC_NAMELEN_OFFSET 32
#define ISO_DIR_REC_NAME_OFFSET 33
#define ISO_FILENAME_STAGE_LEN 64
//PARAM.SFO Format Specifications
#define SFO_MAGIC 0x00465350
#define SFO_MIN_HEADER_SIZE 20
#define SFO_HEADER_KEY_OFFSET 8
#define SFO_HEADER_DATA_OFFSET 12
#define SFO_HEADER_COUNT_OFFSET 16
#define SFO_INDEX_ENTRY_SIZE 16
#define SFO_ENTRY_KEY_OFFSET 0
#define SFO_ENTRY_DATALEN_OFFSET 4
#define SFO_ENTRY_DATA_OFFSET 12
#define SFO_MAX_ALLOWED_SIZE 65536
//Compressed ISO (CISO) Format Specifications
#define CISO_HEADER_SIZE 0x8000
#define CISO_MAGIC_LEN 4
#define CISO_MAP_OFFSET 8
#define CISO_BLOCK0_MASK 0x01
//Console Header Offsets & Lengths
#define SNES_COPIER_HEADER_SIZE 512
#define SNES_BLOCK_ALIGNMENT 1024
#define SNES_LOROM_OFFSET 0x7FC0
#define SNES_HIROM_OFFSET 0xFFC0
#define SNES_RAW_LEN 21
#define GC_OFFSET 0x20
#define GC_RAW_LEN 65
#define GC_GAME_CODE_LEN 4
#define GC_MAKER_CODE_LEN 2
#define PSP_ISO_DISC_ID_OFFSET 0x8373
#define PSP_RAW_SERIAL_LEN 10
#define PSP_CLEAN_SERIAL_MIN_LEN 9
#define N64_OFFSET 0x020
#define N64_RAW_LEN 21
#define GENESIS_OFFSET 0x150
#define GENESIS_RAW_LEN 49
#define DS_OFFSET 0x0000
#define DS_RAW_LEN 13
#define NGPC_OFFSET 0x0024
#define NGPC_RAW_LEN 13
#define LYNX_OFFSET 0x000A
#define LYNX_RAW_LEN 33

//Types of game saves
typedef enum {
    BATTERY,
    EXTERNAL,
    NONE
} save_t;

//Game struct
typedef struct {
    char title[TITLE_MAX_LEN];
    char coverPath[256];
    Texture2D cover;
    char console[512];
    float x;
    float y;
    float w;
    float h;
    char *category;
    char romPath[256];
    char *corePath;
    bool libRetro;
    save_t save;
    char serial[256];
    char *processName;
} game_t;



int Games_GetLength();

void Games_SnapshotSaveFolder(const char* savestatesDir);
void Games_DetectNewYmirSaveFolder(game_t* game, const char* savestatesDir);
void Games_DetectNewPS1SaveFile(game_t* game, const char* savesDir);
void Games_DetectNewFlycastSaveFile(game_t* game, const char* saveDir);

//Get a game from the main array
game_t* Games_Get(int i);
//Get a game from the displayed array
game_t* Games_GetDisplayed(int i);
//Get a game from the new array
game_t* Games_GetNew(int i);
//Update the indexes for the new game category
void Games_UpdateNewIndexes(int direction);
//Update the indexes of the current game category
void Games_UpdateIndexes(const char *categ);
//Update the array of the new games displayed
void Games_NewRefresh(void);
//Update the games displayed for the new category
void Games_Refresh(void);
//Clear game data
bool Games_ClearData(const game_t* game);
//Initialize game library
void Games_Init(void);
//Shift the order of the games to the right
void Games_ScrollRight(void);
//Shift the order of the games to the left
void Games_ScrollLeft(void);
//Get one of the loaded images
Image* Games_GetLoadedImage(int i);
//Get whether an image is loaded
bool Games_GetIsLoaded(int i);
//Load game cover textures
void* Games_LoadImages(void *args);
//Unload game cover textures
void Games_UnloadTextures(void);


#endif