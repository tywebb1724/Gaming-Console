#include "games.h"
#include "raylib.h"
#include "ui.h"
#include <stdio.h>
#include <unistd.h>
#include "categories.h"
#include "states.h"
#include <pthread.h>
#include <stdlib.h>
#include "ui_config.h"
#include "emulators/libretro/retro_bridge.h"
#include "emulators/libretro/libretro.h"
#include <string.h>
#include <math.h>
#include <unistd.h>

//Arrays to hold all games, the games displayed, and the new games displayed
Game gameLibrary[MAX_GAMES];
Game* gamesDisplayed[GAMES_ON_SCREEN + 2];
Game* newGamesDisplayed[GAMES_ON_SCREEN];
//Array to hold loaded images during boot up
Image LoadedImages[GAMES_LEN];
//Indexes and range to keep track of current and new games
int start_index;
int end_index;
int new_start_index;
int gamesIndex;
int gamesRange;

float maxLen = 0;

//Variables to keep track of which images and textures are loaded
bool isLoaded[GAMES_LEN] = { false };
bool isTextureUploaded[GAMES_LEN] = { false };


int your_key_map[RETRO_DEVICE_ID_JOYPAD_MASK];
int your_pad_map[RETRO_DEVICE_ID_JOYPAD_MASK];
Texture2D emulator_texture;
float saveTimeElapsed = 0.0f;

double accumulator;
double core_fps;

bool is_game_running = false;

RenderTexture2D hw_target = {0}; 


// File scope in games.c   // -1 = none found

void Games_DetectController(void) {
    g_controllerIndex = -1;
    for (int i = 0; i < 8; i++) {
        if (IsGamepadAvailable(i)) {
            const char *name = GetGamepadName(i);
            if (name && (strstr(name, "GameSir") || strstr(name, "Xbox") ||
                         strstr(name, "X-Box")   || strstr(name, "Microsoft") ||
                         strstr(name, "Controller"))) {
                g_controllerIndex = i;
                return;
            }
        }
    }
}

//Update the indexes for the new game category
void Game_New_Indexes() {
    Categories categ;
    //If scrolling to the left
    if (scrollCategories == SCROLL_LEFT) {
        //If it is the last category in the array, the new category starts at the beginning
        if (end_index == GAMES_LEN - 1) {
            new_start_index = 1;
        }
        //If not, update the new start index
        else {
            new_start_index = end_index + 2;
        }
    }
    //If scrolling to the right
    else {
        //If the first category in the array, new category is the last one
        if (start_index == 0) {
            categ = gameLibrary[GAMES_LEN - 1].category;
        }
        //If not, it's the previous category
        else {
            categ = gameLibrary[start_index - 1].category;
        }
        //Search the games for the start of the new category
        for (int i = 0; i < GAMES_LEN; i++) {
            //If you find the start fo the new category, update the start index
            if (gameLibrary[i].category == categ) {
                new_start_index = i + 1;
                return;
            }
        }
    }
}

//Update the indexes of the current game category
void Games_UpdateIndexes(Categories categ) {
    //Search through all the games
    for (int i = 0; i < GAMES_LEN; i++) {
        //When beginning of category is found, update start index
        if (gameLibrary[i].category == categ) {
            start_index = i;
            break;
        }
    }
    //Search starting at the start index
    for (int i = start_index + 1; i < GAMES_LEN; i++) {
        //If get to end of array, that's the end of the category
        if (i == GAMES_LEN - 1) {
            end_index = i;
            break;
        }
        //Once you enter another category, update the end of the category as previous index
        if (gameLibrary[i].category != categ) {
            end_index = i - 1;
            break;
        }
    }
    //Update range and current index
    gamesRange = end_index - start_index + 1;
    gamesIndex = start_index + 2;
}

//Update the array of the new games displayed
void Games_NewRefresh() {
    for (int i = 0; i < GAMES_ON_SCREEN; i++) {
        newGamesDisplayed[i] = &gameLibrary[i + new_start_index];
    }
}

//Update the games displayed for the new category
void Games_Refresh() {
    int offset;
    int targetIndex;
    for (int i = 0; i < GAMES_ON_SCREEN + 2; i++) {
        offset = i - 2;
        targetIndex = start_index + (gamesIndex - start_index + offset + gamesRange) % (gamesRange);
        gamesDisplayed[i] = &gameLibrary[targetIndex];
    }
}

//Initialize arcade games
void Games_Arcade_Init() {
    gameLibrary[0].title = "Metal Slug 3";
    gameLibrary[0].coverPath = "./assets/covers/arcade/metal_slug_3.png";
    gameLibrary[0].cover.id = 0;
    gameLibrary[0].category = ARCADE;
    gameLibrary[0].console = "Arcade";
    gameLibrary[0].corePath = PATH_ARCADE;
    gameLibrary[0].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/arcade/mslug3.zip";
    gameLibrary[0].libRetro = true;
    gameLibrary[0].save = NONE;
    
    gameLibrary[1].title = "Pac-Man";
    gameLibrary[1].coverPath = "./assets/covers/arcade/pac-man.png";
    gameLibrary[1].cover.id = 0;
    gameLibrary[1].category = ARCADE;
    gameLibrary[1].console = "Arcade";
    gameLibrary[1].corePath = PATH_ARCADE;
    gameLibrary[1].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/arcade/pacman.zip";
    gameLibrary[1].libRetro = true;
    gameLibrary[1].save = NONE;

    gameLibrary[2].title = "Simpsons Arcade Game";
    gameLibrary[2].coverPath = "./assets/covers/arcade/simpsons_arcade.png";
    gameLibrary[2].cover.id = 0;
    gameLibrary[2].category = ARCADE;
    gameLibrary[2].console = "Arcade";
    gameLibrary[2].corePath = PATH_ARCADE;
    gameLibrary[2].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/arcade/simpsons2p.zip";
    gameLibrary[2].libRetro = true;
    gameLibrary[2].save = NONE;

    gameLibrary[3].title = "Street Fighter Alpha 3";
    gameLibrary[3].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/arcade/sf_alpha_3.png";
    gameLibrary[3].cover.id = 0;
    gameLibrary[3].category = ARCADE;
    gameLibrary[3].console = "Arcade";
    gameLibrary[3].corePath = PATH_ARCADE;
    gameLibrary[3].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/arcade/sfa3.zip";
    gameLibrary[3].libRetro = true;
    gameLibrary[3].save = NONE;

    gameLibrary[4].title = "Teenage Mutant Ninja Turtles: Turtles in Time";
    gameLibrary[4].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/arcade/tmnt_turt_in_time.png";
    gameLibrary[4].cover.id = 0;
    gameLibrary[4].category = ARCADE;
    gameLibrary[4].console = "Arcade";
    gameLibrary[4].corePath = PATH_ARCADE;
    gameLibrary[4].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/arcade/tmnt22pu.zip";
    gameLibrary[4].libRetro = true;
    gameLibrary[4].save = NONE;

    gameLibrary[5].title = "The Punisher Arcade";
    gameLibrary[5].coverPath = "./assets/covers/arcade/punisher_arcade.png";
    gameLibrary[5].cover.id = 0;
    gameLibrary[5].category = ARCADE;
    gameLibrary[5].console = "Arcade";
    gameLibrary[5].corePath = PATH_ARCADE;
    gameLibrary[5].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/arcade/punisher.zip";
    gameLibrary[5].libRetro = true;
    gameLibrary[5].save = NONE;

    gameLibrary[6].title = "X-Men Arcade";
    gameLibrary[6].coverPath = "./assets/covers/arcade/x-men_arcade.png";
    gameLibrary[6].cover.id = 0;
    gameLibrary[6].category = ARCADE;
    gameLibrary[6].console = "Arcade";
    gameLibrary[6].corePath = PATH_ARCADE;
    gameLibrary[6].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/arcade/xmen2pa.zip";
    gameLibrary[6].libRetro = true;
    gameLibrary[6].save = NONE;
}

//Initialize handheld classics games
void Games_Handheld_Init() {
    gameLibrary[7].title = "Grand Theft Auto: Liberty City Stories";
    gameLibrary[7].coverPath = "./assets/covers/handheld/gta_liberty.png";
    gameLibrary[7].cover.id = 0;
    gameLibrary[7].category = HANDHELD;
    gameLibrary[7].console = "Sony PlayStation Portable";
    gameLibrary[7].corePath = PATH_PSP;
    gameLibrary[7].romPath = "assets/roms/psp/GTALibertyCity.iso";
    gameLibrary[7].libRetro = false;
    gameLibrary[7].save = EXTERNAL;
    gameLibrary[7].serial = "ULUS10041S0";

    gameLibrary[8].title = "Mario Kart DS";
    gameLibrary[8].coverPath = "./assets/covers/handheld/mario_kart_ds.png";
    gameLibrary[8].cover.id = 0;
    gameLibrary[8].category = HANDHELD;
    gameLibrary[8].console = "Nintendo DS";
    gameLibrary[8].corePath = PATH_DS;
    gameLibrary[8].romPath = "assets/roms/ds/MarioKartDS.nds";
    gameLibrary[8].libRetro = false;
    gameLibrary[8].save = EXTERNAL;

    gameLibrary[9].title = "Pokemon HeartGold";
    gameLibrary[9].coverPath = "./assets/covers/handheld/pokemon_heartgold.png";
    gameLibrary[9].cover.id = 0;
    gameLibrary[9].category = HANDHELD;
    gameLibrary[9].console = "Nintendo DS";
    gameLibrary[9].corePath = PATH_DS;
    gameLibrary[9].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/ds/PokemonHeartGold.nds";
    gameLibrary[9].libRetro = false;
    gameLibrary[9].save = EXTERNAL;

    gameLibrary[10].title = "Pokemon SoulSilver";
    gameLibrary[10].coverPath = "./assets/covers/handheld/pokemon_soulsilver.png";
    gameLibrary[10].cover.id = 0;
    gameLibrary[10].category = HANDHELD;
    gameLibrary[10].console = "Nintendo DS";
    gameLibrary[10].corePath = PATH_DS;
    gameLibrary[10].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/ds/PokemonSoulSilver.nds";
    gameLibrary[10].libRetro = false;
    gameLibrary[10].save = EXTERNAL;

    gameLibrary[11].title = "The Legend of Zelda: The Minish Cap";
    gameLibrary[11].coverPath = "./assets/covers/handheld/zelda_minish_cap.png";
    gameLibrary[11].cover.id = 0;
    gameLibrary[11].category = HANDHELD;
    gameLibrary[11].console = "Game Boy Advance";
    gameLibrary[11].corePath = PATH_GBA;
    gameLibrary[11].romPath = "assets/roms/gba/MinishChap.gba";
    gameLibrary[11].libRetro = true;
    gameLibrary[11].save = BATTERY;

    gameLibrary[12].title = "Chrono Trigger";
    gameLibrary[12].coverPath = "./assets/covers/handheld/chrono_trigger.png";
    gameLibrary[12].cover.id = 0;
    gameLibrary[12].category = HANDHELD;
    gameLibrary[12].console = "Nintendo DS";
    gameLibrary[12].corePath = PATH_DS;
    gameLibrary[12].romPath = "assets/roms/ds/ChronoTrigger.nds";\
    gameLibrary[12].libRetro = false;
    gameLibrary[12].save = EXTERNAL;

    gameLibrary[13].title = "Monster Hunter Freedom Unite";
    gameLibrary[13].coverPath = "./assets/covers/handheld/monst_hunt_free_unite.png";
    gameLibrary[13].cover.id = 0;
    gameLibrary[13].category = HANDHELD;
    gameLibrary[13].console = "Sony PlayStation Portable";
    gameLibrary[13].corePath = PATH_PSP;
    gameLibrary[13].romPath = "assets/roms/psp/MonsterHunterFreeUnite.iso";
    gameLibrary[13].libRetro = false;
    gameLibrary[13].save = EXTERNAL;
    gameLibrary[13].serial = "ULUS10391";

    gameLibrary[14].title = "Super Mario Bros. Deluxe";
    gameLibrary[14].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/handheld/mario_bros_deluxe.png";
    gameLibrary[14].cover.id = 0;
    gameLibrary[14].category = HANDHELD;
    gameLibrary[14].console = "Game Boy Color";
    gameLibrary[14].corePath = PATH_GAMEBOY;
    gameLibrary[14].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/gbc/SupMarBrosDeluxe.gbc";
    gameLibrary[14].libRetro = true;
    gameLibrary[14].save = BATTERY;

    gameLibrary[15].title = "Metal Slug - 2nd Mission";
    gameLibrary[15].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/handheld/met_slug_2nd.png";
    gameLibrary[15].cover.id = 0;
    gameLibrary[15].category = HANDHELD;
    gameLibrary[15].console = "Neo Geo Pocket Color";
    gameLibrary[15].corePath = PATH_NGPC;
    gameLibrary[15].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/neogeoc/MetalSlug2ndMission.ngc";
    gameLibrary[15].libRetro = true;
    gameLibrary[15].save = NONE;

    gameLibrary[16].title = "Ninja Gaiden";
    gameLibrary[16].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/handheld/ninja_gaiden.png";
    gameLibrary[16].cover.id = 0;
    gameLibrary[16].category = HANDHELD;
    gameLibrary[16].console = "Atari Lynx";
    gameLibrary[16].corePath = PATH_LYNX;
    gameLibrary[16].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/lynx/NinjaGaiden.lyx";
    gameLibrary[16].libRetro = true;
    gameLibrary[16].save = NONE;

    gameLibrary[17].title = "Sonic Blast";
    gameLibrary[17].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/handheld/sonic_blast.png";
    gameLibrary[17].cover.id = 0;
    gameLibrary[17].category = HANDHELD;
    gameLibrary[17].console = "Sega Game Gear";
    gameLibrary[17].corePath = PATH_GENESIS;
    gameLibrary[17].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/gamegear/SonicBlast.gg";
    gameLibrary[17].libRetro = true;
    gameLibrary[17].save = NONE;
}

//Initialize Nintendo 3D games
void Games_Nint3D_Init() {
    gameLibrary[18].title = "Super Mario 64";
    gameLibrary[18].coverPath = "./assets/covers/nint_3d/mario_64.png";
    gameLibrary[18].cover.id = 0;
    gameLibrary[18].category = NINTENDO_3D;
    gameLibrary[18].console = "Nintendo 64";
    gameLibrary[18].corePath = PATH_N64;
    gameLibrary[18].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/n64/SuperMario64.z64";
    gameLibrary[18].libRetro = true;
    gameLibrary[18].save = BATTERY;

    gameLibrary[19].title = "Super Mario Sunshine";
    gameLibrary[19].coverPath = "./assets/covers/nint_3d/mario_sunshine.png";
    gameLibrary[19].cover.id = 0;
    gameLibrary[19].category = NINTENDO_3D;
    gameLibrary[19].console = "Nintendo GameCube";
    gameLibrary[19].corePath = PATH_GAMECUBE;
    gameLibrary[19].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/gamecube/MarioSunshine.ciso";
    gameLibrary[19].libRetro = false;
    gameLibrary[19].save = EXTERNAL;
    gameLibrary[19].serial = "01-GMSE-super_mario_sunshine";

    gameLibrary[20].title = "Starfox 64";
    gameLibrary[20].coverPath = "./assets/covers/nint_3d/starfox_64.png";
    gameLibrary[20].cover.id = 0;
    gameLibrary[20].category = NINTENDO_3D;
    gameLibrary[20].console = "Nintendo 64";
    gameLibrary[20].corePath = PATH_N64;
    gameLibrary[20].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/n64/StarFox64.z64";
    gameLibrary[20].libRetro = true;
    gameLibrary[20].save = BATTERY;

    gameLibrary[21].title = "The Legend of Zelda: Ocarina of Time";
    gameLibrary[21].coverPath = "./assets/covers/nint_3d/zelda_ocarina.png";
    gameLibrary[21].cover.id = 0;
    gameLibrary[21].category = NINTENDO_3D;
    gameLibrary[21].console = "Nintendo 64";
    gameLibrary[21].corePath = PATH_N64;
    gameLibrary[21].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/n64/OcarinaOfTime.z64";
    gameLibrary[21].libRetro = true;
    gameLibrary[21].save = BATTERY;

    gameLibrary[22].title = "The Legend of Zelda: The Wind Waker";
    gameLibrary[22].coverPath = "./assets/covers/nint_3d/zelda_windwaker.png";
    gameLibrary[22].cover.id = 0;
    gameLibrary[22].category = NINTENDO_3D;
    gameLibrary[22].console = "Nintendo GameCube";
    gameLibrary[22].corePath = PATH_GAMECUBE;
    gameLibrary[22].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/gamecube/WindWaker.ciso";
    gameLibrary[22].libRetro = false;
    gameLibrary[22].save = EXTERNAL;
    gameLibrary[22].serial = "01-GZLE-gczelda";

    gameLibrary[23].title = "Super Smash Bros: Melee";
    gameLibrary[23].coverPath = "./assets/covers/nint_3d/smash_bros_melee.png";
    gameLibrary[23].cover.id = 0;
    gameLibrary[23].category = NINTENDO_3D;
    gameLibrary[23].console = "Nintendo GameCube";
    gameLibrary[23].corePath = PATH_GAMECUBE;
    gameLibrary[23].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/gamecube/SmashBrosMelee.ciso";
    gameLibrary[23].libRetro = false;
    gameLibrary[23].save = EXTERNAL;
    gameLibrary[23].serial = "01-GALE-SuperSmashBros0110290334";

    gameLibrary[24].title = "The Legend of Zelda: Twilight Princess";
    gameLibrary[24].coverPath = "./assets/covers/nint_3d/zelda_twilight.png";
    gameLibrary[24].cover.id = 0;
    gameLibrary[24].category = NINTENDO_3D;
    gameLibrary[24].console = "Nintendo GameCube";
    gameLibrary[24].corePath = PATH_GAMECUBE;
    gameLibrary[24].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/gamecube/TwilightPrincess.ciso";
    gameLibrary[24].libRetro = false;
    gameLibrary[24].save = EXTERNAL;
    gameLibrary[24].serial = "01-GZ2E-gczelda2";
}

//Initialize retro Nintendo games
void Games_NintRet_Init() {
    gameLibrary[25].title = "Donkey Kong Country 2: Diddy's Kong Quest";
    gameLibrary[25].coverPath = "./assets/covers/nint_ret/dk_country_2.png";
    gameLibrary[25].cover.id = 0;
    gameLibrary[25].category = NINTENDO_RETRO;
    gameLibrary[25].console = "Super Nintendo Entertainment System";
    gameLibrary[25].corePath = PATH_SNES;
    gameLibrary[25].romPath = "assets/roms/snes/DK2.smc";
    gameLibrary[25].libRetro = true;
    gameLibrary[25].save = BATTERY;

    gameLibrary[26].title = "Mike Tyson's Punch-Out!!";
    gameLibrary[26].coverPath = "./assets/covers/nint_ret/punch-out.png";
    gameLibrary[26].cover.id = 0;
    gameLibrary[26].category = NINTENDO_RETRO;
    gameLibrary[26].console = "Nintendo Entertainment System";
    gameLibrary[26].corePath = PATH_NES;
    gameLibrary[26].romPath = "assets/roms/snes/Punch-Out.nes";
    gameLibrary[26].libRetro = true;
    gameLibrary[26].save = NONE;

    gameLibrary[27].title = "Super Mario World";
    gameLibrary[27].coverPath = "./assets/covers/nint_ret/super_mario_world.png";
    gameLibrary[27].cover.id = 0;
    gameLibrary[27].category = NINTENDO_RETRO;
    gameLibrary[27].console = "Super Nintendo Entertainment System";
    gameLibrary[27].corePath = PATH_SNES;
    gameLibrary[27].romPath = "assets/roms/snes/SuperMarioWorld.smc";
    gameLibrary[27].libRetro = true;
    gameLibrary[27].save = BATTERY;

    gameLibrary[28].title = "Super Metroid";
    gameLibrary[28].coverPath = "./assets/covers/nint_ret/super_metroid.png";
    gameLibrary[28].cover.id = 0;
    gameLibrary[28].category = NINTENDO_RETRO;
    gameLibrary[28].console = "Super Nintendo Entertainment System";
    gameLibrary[28].corePath = PATH_SNES;
    gameLibrary[28].romPath = "assets/roms/snes/SuperMetroid.smc";
    gameLibrary[28].libRetro = true;
    gameLibrary[28].save = BATTERY;

    gameLibrary[29].title = "The Legend of Zelda";
    gameLibrary[29].coverPath = "./assets/covers/nint_ret/zelda.png";
    gameLibrary[29].cover.id = 0;
    gameLibrary[29].category = NINTENDO_RETRO;
    gameLibrary[29].console = "Nintendo Entertainment System";
    gameLibrary[29].corePath = PATH_NES;
    gameLibrary[29].romPath = "assets/roms/nes/LegendOfZelda.nes";
    gameLibrary[29].libRetro = true;
    gameLibrary[29].save = BATTERY;

    gameLibrary[30].title = "EarthBound";
    gameLibrary[30].coverPath = "./assets/covers/nint_ret/earth_bound.png";
    gameLibrary[30].cover.id = 0;
    gameLibrary[30].category = NINTENDO_RETRO;
    gameLibrary[30].console = "Super Nintendo Entertainment System";
    gameLibrary[30].corePath = PATH_SNES;
    gameLibrary[30].romPath = "assets/roms/snes/EarthBound.sfc";
    gameLibrary[30].libRetro = true;
    gameLibrary[30].save = BATTERY;

    gameLibrary[31].title = "Megaman X2";
    gameLibrary[31].coverPath = "./assets/covers/nint_ret/megaman_x2.png";
    gameLibrary[31].cover.id = 0;
    gameLibrary[31].category = NINTENDO_RETRO;
    gameLibrary[31].console = "Super Nintendo Entertainment System";
    gameLibrary[31].corePath = PATH_SNES;
    gameLibrary[31].romPath = "assets/roms/snes/MegamanX2.sfc";
    gameLibrary[31].libRetro = true;
    gameLibrary[31].save = BATTERY;
}

//*********** */
//Initialize retro PC & Indie games
void Games_PCIndie_Init() {
    gameLibrary[32].title = "Counter Strike 1.6";
    gameLibrary[32].coverPath = "./assets/covers/pc_ind/counter_strike_1.6.png";
    gameLibrary[32].cover.id = 0;
    gameLibrary[32].category = PC_INDIE;
    gameLibrary[32].console = "PC";
    gameLibrary[32].libRetro = false;

    gameLibrary[33].title = "Doom";
    gameLibrary[33].coverPath = "./assets/covers/pc_ind/doom_2.png";
    gameLibrary[33].cover.id = 0;
    gameLibrary[33].category = PC_INDIE;
    gameLibrary[33].console = "PC";
    gameLibrary[33].libRetro = false;

    gameLibrary[34].title = "Doom 2";
    gameLibrary[34].coverPath = "./assets/covers/pc_ind/doom.png";
    gameLibrary[34].cover.id = 0;
    gameLibrary[34].category = PC_INDIE;
    gameLibrary[34].console = "PC";
    gameLibrary[34].libRetro = false;

    gameLibrary[35].title = "Half-Life";
    gameLibrary[35].coverPath = "./assets/covers/pc_ind/half-life.png";
    gameLibrary[35].cover.id = 0;
    gameLibrary[35].category = PC_INDIE;
    gameLibrary[35].console = "PC";
    gameLibrary[35].libRetro = false;

    gameLibrary[36].title = "Stardew Valley";
    gameLibrary[36].coverPath = "./assets/covers/pc_ind/stardew_valley.png";
    gameLibrary[36].cover.id = 0;
    gameLibrary[36].category = PC_INDIE;
    gameLibrary[36].console = "PC";
    gameLibrary[36].libRetro = false;

    gameLibrary[37].title = "Diablo";
    gameLibrary[37].coverPath = "./assets/covers/pc_ind/diablo.png";
    gameLibrary[37].cover.id = 0;
    gameLibrary[37].category = PC_INDIE;
    gameLibrary[37].console = "PC";
    gameLibrary[37].libRetro = false;

    gameLibrary[38].title = "Hollow Knight";
    gameLibrary[38].coverPath = "./assets/covers/pc_ind/hollow_knight.png";
    gameLibrary[38].cover.id = 0;
    gameLibrary[38].category = PC_INDIE;
    gameLibrary[38].console = "PC";
    gameLibrary[38].libRetro = false;

    gameLibrary[39].title = "Splatterhouse";
    gameLibrary[39].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/pc_ind/splatterhouse.png";
    gameLibrary[39].cover.id = 0;
    gameLibrary[39].category = PC_INDIE;
    gameLibrary[39].console = "TurboGrafx-16";
    gameLibrary[39].corePath = PATH_TG16;
    gameLibrary[39].romPath = "assets/roms/trbogrfx/Splatterhouse.pce";
    gameLibrary[39].libRetro = true;
    gameLibrary[39].save = NONE;

    gameLibrary[40].title = "Cosmic Fantasy 2";
    gameLibrary[40].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/pc_ind/cos_fant_2.png";
    gameLibrary[40].cover.id = 0;
    gameLibrary[40].category = PC_INDIE;
    gameLibrary[40].console = "TurboGrafx-CD";
    gameLibrary[40].corePath = PATH_TG16;
    gameLibrary[40].romPath = "assets/roms/trbogrfx/CosmicFantasy2.chd";
    gameLibrary[40].libRetro = true;
    gameLibrary[40].save = EXTERNAL;
    
}

//Initialize Sega games
void Games_Sega_Init() {
    gameLibrary[41].title = "Crazy Taxi";
    gameLibrary[41].coverPath = "./assets/covers/sega/crazy_taxi.png";
    gameLibrary[41].cover.id = 0;
    gameLibrary[41].category = SEGA;
    gameLibrary[41].console = "Sega Dreamcast";
    gameLibrary[41].corePath = PATH_DREAMCAST;
    gameLibrary[41].romPath = "assets/roms/dreamcast/CrazyTaxi.chd";
    gameLibrary[41].libRetro = false;
    gameLibrary[41].save = EXTERNAL;

    gameLibrary[42].title = "Fighting Vipers";
    gameLibrary[42].coverPath = "./assets/covers/sega/fighting_vipers.png";
    gameLibrary[42].cover.id = 0;
    gameLibrary[42].category = SEGA;
    gameLibrary[42].console = "Sega Saturn";
    gameLibrary[42].corePath = PATH_SATURN;
    gameLibrary[42].romPath = "assets/roms/saturn/FightingVipers.chd";
    gameLibrary[42].libRetro = false;
    gameLibrary[42].save = EXTERNAL;
    gameLibrary[42].serial = "B1334F8949ED55769416D83D7A19F1C5";

    gameLibrary[43].title = "Sonic the Hedgehog 3 & Knuckles";
    gameLibrary[43].coverPath = "./assets/covers/sega/sonic_3_and_knuckles.png";
    gameLibrary[43].cover.id = 0;
    gameLibrary[43].category = SEGA;
    gameLibrary[43].console = "Sega Genesis";
    gameLibrary[43].corePath = PATH_GENESIS;
    gameLibrary[43].romPath = "assets/roms/genesis/Sonic3&Knuckles.md";
    gameLibrary[43].libRetro = true;
    gameLibrary[43].save = BATTERY;

    gameLibrary[44].title = "Sonic CD";
    gameLibrary[44].coverPath = "./assets/covers/sega/sonic_cd.png";
    gameLibrary[44].cover.id = 0;
    gameLibrary[44].category = SEGA;
    gameLibrary[44].console = "Sega CD";
    gameLibrary[44].corePath = PATH_GENESIS;
    gameLibrary[44].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/cd/SonicCD.chd";
    gameLibrary[44].libRetro = true;
    gameLibrary[44].save = EXTERNAL;
    
    gameLibrary[45].title = "NBA Jam: Tournament Edition";
    gameLibrary[45].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/sega/nba_jam_te.png";
    gameLibrary[45].cover.id = 0;
    gameLibrary[45].category = SEGA;
    gameLibrary[45].console = "Sega Saturn";
    gameLibrary[45].corePath = PATH_SATURN;
    gameLibrary[45].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/saturn/NBAJam.chd";
    gameLibrary[45].libRetro = false;
    gameLibrary[45].save = EXTERNAL;
    gameLibrary[45].serial = "DBA9351F1D9322E87F3D7F511992DEB3";

    gameLibrary[46].title = "Daytona USA";
    gameLibrary[46].coverPath = "./assets/covers/sega/daytona_usa.png";
    gameLibrary[46].cover.id = 0;
    gameLibrary[46].category = SEGA;
    gameLibrary[46].console = "Sega Saturn";
    gameLibrary[46].corePath = PATH_SATURN;
    gameLibrary[46].romPath = "assets/roms/saturn/DaytonaUSA.chd";
    gameLibrary[46].libRetro = false;
    gameLibrary[46].save = EXTERNAL;
    gameLibrary[46].serial = "C8355C918C5A97B9E4AB9322DDAFDB1E";

    gameLibrary[47].title = "Gunstar Heroes";
    gameLibrary[47].coverPath = "./assets/covers/sega/gunstar_heroes.png";
    gameLibrary[47].cover.id = 0;
    gameLibrary[47].category = SEGA;
    gameLibrary[47].console = "Sega Genesis";
    gameLibrary[47].corePath = PATH_GENESIS;
    gameLibrary[47].romPath = "assets/roms/genesis/GunstarHeroes.md";
    gameLibrary[47].libRetro = true;
    gameLibrary[47].save = NONE;

    gameLibrary[48].title = "Wonder Boy";
    gameLibrary[48].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/sega/wonder_boy.png";
    gameLibrary[48].cover.id = 0;
    gameLibrary[48].category = SEGA;
    gameLibrary[48].console = "Sega Master System";
    gameLibrary[48].corePath = PATH_GENESIS;
    gameLibrary[48].romPath = "assets/roms/mastsys/WonderBoy.sms";
    gameLibrary[48].libRetro = true;
    gameLibrary[48].save = NONE;
}

//DONE//
//Initialize Playstation games
void Games_Playstation_Init() {
    gameLibrary[49].title = "Mega Man X4";
    gameLibrary[49].coverPath = "./assets/covers/playstation/megaman_x4.png";
    gameLibrary[49].cover.id = 0;
    gameLibrary[49].category = PLAYSTATION;
    gameLibrary[49].console = "Sony PlayStation";
    gameLibrary[49].corePath = PATH_PS1;
    gameLibrary[49].romPath = "assets/roms/ps1/MegaManX4.chd";
    gameLibrary[49].libRetro = true;
    gameLibrary[49].save = EXTERNAL;
    gameLibrary[49].serial = "SLUS-00561_1";


    gameLibrary[50].title = "Pac-Man World";
    gameLibrary[50].coverPath = "./assets/covers/playstation/pac-man_world.png";
    gameLibrary[50].cover.id = 0;
    gameLibrary[50].category = PLAYSTATION;
    gameLibrary[50].console = "Sony PlayStation";
    gameLibrary[50].corePath = PATH_PS1;
    gameLibrary[50].romPath = "assets/roms/ps1/Pac-ManWorld.chd";
    gameLibrary[50].libRetro = true;
    gameLibrary[50].save = EXTERNAL;
    gameLibrary[50].serial = "SLUS-00439_1";

    gameLibrary[51].title = "Soul Blade";
    gameLibrary[51].coverPath = "./assets/covers/playstation/soulblade.png";
    gameLibrary[51].cover.id = 0;
    gameLibrary[51].category = PLAYSTATION;
    gameLibrary[51].console = "Sony PlayStation";
    gameLibrary[51].corePath = PATH_PS1;
    gameLibrary[51].romPath = "assets/roms/ps1/SoulBlade.chd";
    gameLibrary[51].libRetro = true;
    gameLibrary[51].save = EXTERNAL;
    gameLibrary[51].serial = "SLUS-00240_1";

    gameLibrary[52].title = "Spider-Man (2000)";
    gameLibrary[52].coverPath = "./assets/covers/playstation/spider-man.png";
    gameLibrary[52].cover.id = 0;
    gameLibrary[52].category = PLAYSTATION;
    gameLibrary[52].console = "Sony PlayStation";
    gameLibrary[52].corePath = PATH_PS1;
    gameLibrary[52].romPath = "assets/roms/ps1/Spider-Man.chd";
    gameLibrary[52].libRetro = true;
    gameLibrary[52].save = EXTERNAL;
    gameLibrary[52].serial = "SLUS-00875_1";

    gameLibrary[53].title = "Street Fighter: Alpha 3";
    gameLibrary[53].coverPath = "./assets/covers/playstation/street_fighter_alpha_3.png";
    gameLibrary[53].cover.id = 0;
    gameLibrary[53].category = PLAYSTATION;
    gameLibrary[53].console = "Sony PlayStation";
    gameLibrary[53].corePath = PATH_PS1;
    gameLibrary[53].romPath = "assets/roms/ps1/StreetFighterAlpha3.chd";
    gameLibrary[53].libRetro = true;
    gameLibrary[53].save = EXTERNAL;
    gameLibrary[53].serial = "SLUS-00821_1";

    gameLibrary[54].title = "Metal Gear Solid";
    gameLibrary[54].coverPath = "./assets/covers/playstation/met_gear_solid.png";
    gameLibrary[54].cover.id = 0;
    gameLibrary[54].category = PLAYSTATION;
    gameLibrary[54].console = "Sony PlayStation";
    gameLibrary[54].corePath = PATH_PS1;
    gameLibrary[54].romPath = "assets/roms/ps1/MetalGearSolid.m3u";
    gameLibrary[54].libRetro = true;
    gameLibrary[54].save = EXTERNAL;
    gameLibrary[54].serial = "SLUS-00594_1";

    gameLibrary[55].title = "Twisted Metal 2";
    gameLibrary[55].coverPath = "./assets/covers/playstation/twist_met_2.png";
    gameLibrary[55].cover.id = 0;
    gameLibrary[55].category = PLAYSTATION;
    gameLibrary[55].console = "Sony PlayStation";
    gameLibrary[55].corePath = PATH_PS1;
    gameLibrary[55].romPath = "assets/roms/ps1/TwistedMetal2.chd";
    gameLibrary[55].libRetro = true;
    gameLibrary[55].save = EXTERNAL;
    gameLibrary[55].serial = "SCUS-94306_1";
}

//Initialize game library
void Games_Init() {
    //Arcade games
    Games_Arcade_Init();
    //Handheld classics games
    Games_Handheld_Init();
    //Nintendo 3D games
    Games_Nint3D_Init();
    //Retro Nintendo games
    Games_NintRet_Init();
    //Retro PC & indie games
    Games_PCIndie_Init();
    //Sega central games
    Games_Sega_Init();
    //Sony Playstation games
    Games_Playstation_Init();

    Games_UpdateIndexes(NINTENDO_3D);
    Games_Refresh();

    float tempLen1;
    float tempLen2;
    for (int i = 0; i < GAMES_LEN; i++) {
        tempLen1 = MeasureTextEx(fontBold, gameLibrary[i].title, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE).x;
        tempLen2 = MeasureTextEx(fontBold, gameLibrary[i].console, BOTTOM_TXT_SIZE, BOTTOM_TXT_SPACE).x;
        if (tempLen1 > maxLen) {
            maxLen = tempLen1;
        }
        if (tempLen2 > maxLen) {
            maxLen = tempLen2;
        }
    }
    Games_DetectController();
}

//Load game cover textures
void* Games_LoadImages(void *args) {
    //Detach the thread
    pthread_detach(pthread_self());
    //Load the images of the current category first
    for (int i = start_index; i <= end_index; i++) {
        LoadedImages[i] = LoadImage(gameLibrary[i].coverPath);
        isLoaded[i] = true;
    }
    //Load the images at beginning of array
    for (int i = 0; i < start_index; i++) {
        LoadedImages[i] = LoadImage(gameLibrary[i].coverPath);
        isLoaded[i] = true;
    }
    //Load then images at end of array
    for (int i = end_index + 1; i < GAMES_LEN; i++) {
        LoadedImages[i] = LoadImage(gameLibrary[i].coverPath);
        isLoaded[i] = true;
    }
}

//Shift the order of the games to the right
void Games_ScrollRight() {
    gamesIndex = start_index + (gamesIndex - start_index + 1) % gamesRange;
    Games_Refresh();
}

//Shift the order of the games to the left
void Games_ScrollLeft() {
    gamesIndex = start_index + (gamesIndex - start_index - 1 + gamesRange) % gamesRange;
    Games_Refresh();
}
    
//Unload game cover textures
void Games_UnloadTextures() {
    //Unload all covers
    for (int i = 0; i <= GAMES_LEN; i++) {
        UnloadTexture(gameLibrary[i].cover);
    }
}

bool Games_IsLibRetro(Game game) {
    if (game.libRetro == true) {
        return true;
    }
    else {
        return false;
    }
}

void Games_ApplyKeyMap(char* corePath) {
    for (int i = 0; i < 32; i++) { your_key_map[i] = 0; your_pad_map[i] = 0; }

    // ---- COMMON (D-pad + Start/Select) ----
    your_key_map[RETRO_DEVICE_ID_JOYPAD_UP]     = KEY_UP;
    your_key_map[RETRO_DEVICE_ID_JOYPAD_DOWN]   = KEY_DOWN;
    your_key_map[RETRO_DEVICE_ID_JOYPAD_LEFT]   = KEY_LEFT;
    your_key_map[RETRO_DEVICE_ID_JOYPAD_RIGHT]  = KEY_RIGHT;
    your_key_map[RETRO_DEVICE_ID_JOYPAD_START]  = KEY_ENTER;
    your_key_map[RETRO_DEVICE_ID_JOYPAD_SELECT] = KEY_RIGHT_SHIFT;
    your_pad_map[RETRO_DEVICE_ID_JOYPAD_UP]     = GAMEPAD_BUTTON_LEFT_FACE_UP;
    your_pad_map[RETRO_DEVICE_ID_JOYPAD_DOWN]   = GAMEPAD_BUTTON_LEFT_FACE_DOWN;
    your_pad_map[RETRO_DEVICE_ID_JOYPAD_LEFT]   = GAMEPAD_BUTTON_LEFT_FACE_LEFT;
    your_pad_map[RETRO_DEVICE_ID_JOYPAD_RIGHT]  = GAMEPAD_BUTTON_LEFT_FACE_RIGHT;
    your_pad_map[RETRO_DEVICE_ID_JOYPAD_START]  = GAMEPAD_BUTTON_MIDDLE_RIGHT;
    your_pad_map[RETRO_DEVICE_ID_JOYPAD_SELECT] = GAMEPAD_BUTTON_MIDDLE_LEFT;

    // ---- NES / Game Boy / Lynx (2-button: B=primary, A=secondary) ----
    // Native NES: B (left) and A (right). A is usually jump/confirm.
    // Natural: put A (primary) on Xbox A (bottom), B on Xbox X (left) — keeps
    // the "A is the main button" feel with A under the thumb.
    if (strcmp(corePath, PATH_NES) == 0 ||
        strcmp(corePath, PATH_GAMEBOY) == 0 ||
        strcmp(corePath, PATH_LYNX) == 0)
    {
        your_key_map[RETRO_DEVICE_ID_JOYPAD_A] = KEY_X;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_B] = KEY_Z;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_A] = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;  // A = Xbox A (bottom, primary)
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_B] = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT;  // B = Xbox X (left, secondary)
    }
    // ---- SNES / GBA (the classic diamond — Nintendo's native layout) ----
    // SNES face: Y(left) X(top) B(bottom) A(right). Xbox is the SAME diamond
    // physically, so map 1:1 by POSITION for perfect muscle memory:
    // SNES B(bottom)->Xbox A(bottom), A(right)->Xbox B(right),
    // Y(left)->Xbox X(left), X(top)->Xbox Y(top).
    else if (strcmp(corePath, PATH_SNES) == 0 ||
             strcmp(corePath, PATH_GBA) == 0 ||
             strcmp(corePath, PATH_TG16) == 0 ||
             strcmp(corePath, PATH_NGPC) == 0 ||
             strcmp(corePath, PATH_DS) == 0)
    {
        your_key_map[RETRO_DEVICE_ID_JOYPAD_B] = KEY_Z;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_A] = KEY_X;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_Y] = KEY_A;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_X] = KEY_S;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_L] = KEY_Q;
        your_key_map[RETRO_DEVICE_ID_JOYPAD_R] = KEY_W;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_B] = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;  // SNES B -> Xbox A (bottom)
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_A] = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT; // SNES A -> Xbox B (right)
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_Y] = GAMEPAD_BUTTON_RIGHT_FACE_LEFT;  // SNES Y -> Xbox X (left)
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_X] = GAMEPAD_BUTTON_RIGHT_FACE_UP;    // SNES X -> Xbox Y (top)
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_L] = GAMEPAD_BUTTON_LEFT_TRIGGER_1;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_R] = GAMEPAD_BUTTON_RIGHT_TRIGGER_1;
    }
    // ---- Genesis / Saturn (6-button: ABC bottom row, XYZ top row) ----
    // Native 6-button: A B C (bottom), X Y Z (top). Most natural on Xbox is
    // bottom row ABC -> Xbox A,B,RB-ish and top row XYZ -> X,Y,LB-ish.
    // Common comfortable convention: A->X(left), B->A(bottom), C->B(right)
    // so the main row sits across the face; X,Y,Z to top + shoulders.
    else if (strcmp(corePath, PATH_GENESIS) == 0 || strcmp(corePath, PATH_SATURN) == 0) {
    // Map Xbox face buttons to libretro's standard button IDs (the core
    // translates these to the Genesis's A/B/C/X/Y/Z internally).
    // Standard diamond mapping (same as SNES positional):
    your_key_map[RETRO_DEVICE_ID_JOYPAD_B] = KEY_Z;
    your_key_map[RETRO_DEVICE_ID_JOYPAD_A] = KEY_X;
    your_key_map[RETRO_DEVICE_ID_JOYPAD_Y] = KEY_A;
    your_key_map[RETRO_DEVICE_ID_JOYPAD_X] = KEY_S;
    your_key_map[RETRO_DEVICE_ID_JOYPAD_L] = KEY_Q;
    your_key_map[RETRO_DEVICE_ID_JOYPAD_R] = KEY_W;

    your_pad_map[RETRO_DEVICE_ID_JOYPAD_B] = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;  // Xbox A (bottom)
    your_pad_map[RETRO_DEVICE_ID_JOYPAD_A] = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT; // Xbox B (right)
    your_pad_map[RETRO_DEVICE_ID_JOYPAD_Y] = GAMEPAD_BUTTON_RIGHT_FACE_LEFT;  // Xbox X (left)
    your_pad_map[RETRO_DEVICE_ID_JOYPAD_X] = GAMEPAD_BUTTON_RIGHT_FACE_UP;    // Xbox Y (top)
    your_pad_map[RETRO_DEVICE_ID_JOYPAD_L] = GAMEPAD_BUTTON_LEFT_TRIGGER_1;   // LB
    your_pad_map[RETRO_DEVICE_ID_JOYPAD_R] = GAMEPAD_BUTTON_RIGHT_TRIGGER_1;  // RB

    if (strcmp(corePath, PATH_SATURN) == 0) {
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_L2] = GAMEPAD_BUTTON_LEFT_TRIGGER_2;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_R2] = GAMEPAD_BUTTON_RIGHT_TRIGGER_2;
    }
}
    // ---- PS1 (Sony layout maps almost 1:1 to Xbox by position) ----
    // PS1: Cross(bottom) Circle(right) Square(left) Triangle(top) = same diamond.
    // Map by position: Cross->A, Circle->B, Square->X, Triangle->Y.
    else if (strcmp(corePath, PATH_PS1) == 0 || strcmp(corePath, PATH_ARCADE) == 0) {
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_B]  = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;  // Cross -> Xbox A
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_A]  = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT; // Circle -> Xbox B
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_Y]  = GAMEPAD_BUTTON_RIGHT_FACE_LEFT;  // Square -> Xbox X
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_X]  = GAMEPAD_BUTTON_RIGHT_FACE_UP;    // Triangle -> Xbox Y
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_L]  = GAMEPAD_BUTTON_LEFT_TRIGGER_1;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_R]  = GAMEPAD_BUTTON_RIGHT_TRIGGER_1;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_L2] = GAMEPAD_BUTTON_LEFT_TRIGGER_2;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_R2] = GAMEPAD_BUTTON_RIGHT_TRIGGER_2;
        // (add keyboard keys here too if you want K/B for these)
    }
    // ---- N64 (unique — analog + C-buttons) ----
    else if (strcmp(corePath, PATH_N64) == 0) {
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_A]  = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;  // N64 A -> Xbox A (primary)
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_B]  = GAMEPAD_BUTTON_RIGHT_FACE_LEFT;  // N64 B -> Xbox X (left of A, like N64's B is left of A)
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_L]  = GAMEPAD_BUTTON_LEFT_TRIGGER_1;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_R]  = GAMEPAD_BUTTON_RIGHT_TRIGGER_1;
        your_pad_map[RETRO_DEVICE_ID_JOYPAD_L2] = GAMEPAD_BUTTON_LEFT_TRIGGER_2;   // Z-trigger -> LT
        // C-buttons -> right stick (handled in input_state_cb, see note)
    }
}

void Games_LaunchInit(Game game) {
    // Inside your main initialization code:
    // Create a blank texture. 640x480 covers most retro systems (NES, Genesis, SNES, etc.)
    // Find where you load your texture in main.c and add the filter line right below it:

    if (Games_IsLibRetro(game)) {
        SetGameRotation(0);
        UnloadTexture(emulator_texture);
        Image blank = GenImageColor(640, 480, BLACK);
        emulator_texture = LoadTextureFromImage(blank);
        UnloadImage(blank);
        SetTextureFilter(emulator_texture, TEXTURE_FILTER_POINT);
        Games_ApplyKeyMap(game.corePath);
        if (LoadRetroCore(game.corePath)) {
            if (LoadGame(game.romPath)) {
                is_game_running = true;
                if (hw_target.id == 0) {
                    hw_target = LoadRenderTexture(640, 480);
                }
                SetHWFramebuffer(hw_target.id);     // hand the FBO id to retro_bridge.c

                TriggerContextReset();

                if (game.save == BATTERY) {
                    LoadBattery(game.romPath);
                }

                core_fps = GetCoreTargetFPS();         // e.g. 75.0 for many Lynx games
                printf("DEBUG: core_fps = %f\n", core_fps);
                StartRetroAudio();
                accumulator = 0.0;
            }
            else {
                //printf("CRITICAL: LoadGame() returned false. Aborting launch.\n");
                is_game_running = false; // Prevent the run loop
                // Optionally: unload core here
            }
        }
        saveTimeElapsed = 0;
    }
    else {

        if (strcmp(game.corePath, PATH_DS) == 0) {
            system("flatpak run io.github.antimicrox.antimicrox --profile /home/tywebb1724/Desktop/Gaming-Console/assets/antimicro/micro_ds.gamecontroller.amgp --hidden &");
            usleep(4000000);
        }
        else if (strcmp(game.corePath, PATH_SATURN) == 0) {
            system("flatpak run io.github.antimicrox.antimicrox --profile /home/tywebb1724/Desktop/Gaming-Console/assets/antimicro/micro_saturn.gamecontroller.amgp --hidden &");
            usleep(4000000);
        }
        char command[1024];
        snprintf(command, sizeof(command), "flatpak run %s \"%s\"", game.corePath, game.romPath);
        system(command);
        if (strcmp(game.corePath, PATH_DS) == 0) {
            system("flatpak kill io.github.antimicrox.antimicrox");
        }
        else if (strcmp(game.corePath, PATH_SATURN) == 0) {
            system("flatpak kill io.github.antimicrox.antimicrox");
        }
        SetWindowFocused();
    }
}


 
float GetConsoleAspect(const char* console) {
    if (strcmp(console, "Game Boy") == 0)         return 10.0f / 9.0f;
    if (strcmp(console, "Game Boy Color") == 0)   return 10.0f / 9.0f;
    if (strcmp(console, "Game Boy Advance") == 0) return 3.0f / 2.0f;
    if (strcmp(console, "Game Gear") == 0)        return 6.0f / 5.0f;   // verify
    if (strcmp(console, "Neo Geo Pocket Color") == 0) return 20.0f / 19.0f;  // verify
    // DS needs special dual-screen handling, not a single ratio — handle separately
    return 4.0f / 3.0f;   // all home consoles + default
}

void Games_StopGame(Game game) {
    printf("DEBUG: Game_StopGame called, is_game_running=%d\n", is_game_running);
    if (is_game_running) {
        if (game.save == BATTERY) {
            SaveBattery(game.romPath);
        }          // harmless for PS1, needed for cartridge
        printf("DEBUG: calling core_unload_game\n");
        StopRetroAudio();
        if (core_unload_game) core_unload_game();
        printf("DEBUG: calling CloseRetroCore (deinit)\n");
        TriggerContextDestroy();
        CloseRetroCore();
        is_game_running = false;
         printf("DEBUG: Game_StopGame finished\n");
    }
}

bool Games_LaunchRefresh(Game game) {
    // --- emulator timing (unchanged) ---
    double step = 1.0 / core_fps;
    double frameTime = GetFrameTime();
    if (frameTime > 0.25) frameTime = 0.25;
    accumulator += frameTime;
    while (accumulator >= step) {
        if (is_game_running && core_run) core_run();
        accumulator -= step;
    }
    PresentFrame();

    ClearBackground(BLACK);

    // --- rotation-aware draw ---
    // game_rotation: 0,1,2,3 = number of 90-degree CCW turns the core requested.
    // For 90/270 turns, the displayed width/height swap.
    unsigned game_rotation = GetGameRotation();
    bool swapped = (game_rotation == 1 || game_rotation == 3);

    float texW = (float)emulator_texture.width;
    float texH = (float)emulator_texture.height;

    float targetAspect;
    // Use your per-console display aspect (the earlier fix), not raw texture dims.
    if (strcmp(game.console, "Arcade") == 0) {
        targetAspect = texW / texH;
    }
    else {
        targetAspect = GetConsoleAspect(game.console);
    }   
    // 4:3 default, 10:9 GB, etc.
// For arcade, you may want the core's actual aspect instead — see note below.

// If rotated 90/270, the display aspect inverts (portrait <-> landscape):
if (swapped) {
    targetAspect = 1.0f / targetAspect;
}

    // Fit that aspect inside the screen (letterbox/pillarbox), centered.
    float destHeight = monitorHeight;
    float destWidth  = destHeight * targetAspect;
    if (destWidth > monitorWidth) {
        destWidth  = monitorWidth;
        destHeight = destWidth / targetAspect;
    }

    // Rotation angle for DrawTexturePro (raylib rotates CLOCKWISE; libretro turns are CCW).
    // game_rotation * 90 gives CCW degrees; raylib wants CW, so negate.
    // >>> IF PAC-MAN COMES OUT ROTATED THE WRONG WAY, change this to +(game_rotation * 90.0f) <
    float rot = -(game_rotation * 90.0f);

    // With rotation, place dest at screen CENTER and set origin to half the
    // *unrotated* dest size, so it pivots around its own center.
    // For swapped rotations, the dest rect's width/height are the pre-rotation
    // texture proportions; the rotation then turns it into the displayed shape.
    float drawW = swapped ? destHeight : destWidth;   // pre-rotation footprint
    float drawH = swapped ? destWidth  : destHeight;

    Rectangle src  = { 0.0f, 0.0f, texW, texH };
    Rectangle dest = { monitorWidth / 2.0f, monitorHeight / 2.0f, drawW, drawH };
    Vector2  origin = { drawW / 2.0f, drawH / 2.0f };

    DrawTexturePro(emulator_texture, src, dest, origin, rot, WHITE);

    // --- save timer (unchanged) ---
    saveTimeElapsed += GetFrameTime();
    if (saveTimeElapsed >= 60.0f) {
        if (game.save == BATTERY) {
            SaveBattery(game.romPath);
        }
        saveTimeElapsed = 0.0f;
    }

    if (IsKeyPressed(KEY_P) || HOME_PRESS) {
        Games_StopGame(game);
        return true;
    }
    return false;
}

