#include "games.h"
#include <pthread.h>
#include "config.h"
#include <stdio.h>
#include <string.h>


//Arrays to hold all games, the games displayed, and the new games displayed
static game_t gamesLibrary[MAX_GAMES];
static game_t* gamesDisplayed[GAMES_ON_SCREEN + 2];
static game_t* newGamesDisplayed[GAMES_ON_SCREEN];
//Array to hold loaded images during boot up
static Image LoadedImages[GAMES_LEN];
//Indexes and range to keep track of current and new games
static int games_index;
static int start_index;
static int end_index;
static int games_range;
static int start_index_new;

//Variables to keep track of which images and textures are loaded
static bool isLoaded[GAMES_LEN] = { false };


//Get a game from the main array
game_t* Games_Get(int i) {
    return &gamesLibrary[i];
}

//Get a game from the displayed array
game_t* Games_GetDisplayed(int i) {
    return gamesDisplayed[i];
}

//Get a game from the new array
game_t* Games_GetNew(int i) {
    return newGamesDisplayed[i];
}

//Update the indexes for the new game category
void Games_UpdateNewIndexes(int direction) {
    char categ[30];
    //If scrolling to the left
    if (direction == LEFT) {
        //If it is the last category in the array, the new category starts at the beginning
        if (end_index == GAMES_LEN - 1) {
            start_index_new = 1;
        }
        //If not, update the new start index
        else {
            start_index_new = end_index + 2;
        }
    }
    //If scrolling to the right
    else {
        //If the first category in the array, new category is the last one
        if (start_index == 0) {
            sprintf(categ, "%s", gamesLibrary[GAMES_LEN - 1].category);
        }
        //If not, it's the previous category
        else {
            sprintf(categ, "%s", gamesLibrary[start_index - 1].category);
        }
        //Search the games for the start of the new category
        for (int i = 0; i < GAMES_LEN; i++) {
            //If you find the start fo the new category, update the start index
            if (strcmp(gamesLibrary[i].category, categ) == 0) {
                start_index_new = i + 1;
                return;
            }
        }
    }
}

//Update the indexes of the current game category
void Games_UpdateIndexes(const char *categ) {
    //Search through all the games
    for (int i = 0; i < GAMES_LEN; i++) {
        //When beginning of category is found, update start index
        if (strcmp(gamesLibrary[i].category, categ) == 0) {
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
        if (strcmp(gamesLibrary[i].category, categ) != 0) {
            end_index = i - 1;
            break;
        }
    }
    //Update range and current index
    games_range = end_index - start_index + 1;
    games_index = start_index + 2;
}

//Update the array of the new games displayed
void Games_NewRefresh() {
    //Update each of the new displayed games depending on the new start index
    for (int i = 0; i < GAMES_ON_SCREEN; i++) {
        newGamesDisplayed[i] = &gamesLibrary[i + start_index_new];
    }
}

//Update the games displayed for the new category
void Games_Refresh() {
    int offset;
    int targetIndex;
    for (int i = 0; i < GAMES_ON_SCREEN + 2; i++) {
        offset = i - 2;
        targetIndex = start_index + (games_index - start_index + offset + games_range) % (games_range);
        gamesDisplayed[i] = &gamesLibrary[targetIndex];
    }
}

//Clear game data
void Games_ClearData(game_t game) {
    //If saves through battery method
    if (game.save == BATTERY) {
        //Get file path
        char save_path[512];
        snprintf(save_path, sizeof(save_path), "%s.srm", game.romPath);
        //Remove save file
        if (remove(save_path) == 0) {
            printf("Cleared save data: %s\n", save_path);
        } 
        else {
            printf("No save data to clear (or delete failed): %s\n", save_path);
        }
    }
    //If saves through other method
    else if (game.save == EXTERNAL) {
        //Check which console
        if (strcmp(game.console, "Sony PlayStation") == 0) {
            //Get file path
            char save_path[512];
            snprintf(save_path, sizeof(save_path), "/home/tywebb1724/Desktop/Gaming-Console/assets/saves/%s.mcd", game.serial);
            //Remove save file
            if (remove(save_path) == 0) {
                printf("Cleared save data: %s\n", save_path);
            } 
            else {
                printf("No save data to clear (or delete failed): %s\n", save_path);
            }
        }
        else if (strcmp(game.console, "Sega CD") == 0) {
            //Get file path
            char save_path[512];
            char path[256];
            snprintf(path, sizeof(path), "%s", game.romPath);
            const char* filename = strrchr(path, '/');
            filename = filename ? filename + 1 : path; 
            char* dot = strrchr(filename, '.');
            if (dot) {
                *dot = '\0';
            }
            snprintf(save_path, sizeof(save_path), "/home/tywebb1724/Desktop/Gaming-Console/assets/saves/%s.brm", filename);
            //Remove save file
            if (remove(save_path) == 0) {
                printf("Cleared save data: %s\n", save_path);
            } 
            else {
                printf("No save data to clear (or delete failed): %s\n", save_path);
            }
        }
        else if (strcmp(game.console, "Sony PlayStation Portable") == 0) {
            //Create command for removing file
            char command[1024] = "";
            snprintf(command, sizeof(command), "rm -rf \"/home/tywebb1724/.var/app/org.ppsspp.PPSSPP/config/ppsspp/PSP/SAVEDATA/%s\"", game.serial);
            //Run command
            int result = system(command);
            //Check if command worked
            if (result == 0) {
                printf("Cleared PSP save data for %s\n", game.serial);
            } 
            else {
                printf("Failed to clear PSP save (or none existed)\n");
            }
        }
        else if (strcmp(game.console, "Sega Saturn") == 0) {
            //Create command for removing file
            char command[1024] = "";
            snprintf(command, sizeof(command), "rm -rf \"/home/tywebb1724/.var/app/io.github.strikerx3.ymir/data/StrikerX3/Ymir/savestates/%s\"", game.serial);
            //Run command
            int result = system(command);
            //Check if command worked
            if (result == 0) {
                printf("Cleared Saturn save data for %s\n", game.serial);
            } 
            else {
                printf("Failed to clear Saturn save (or none existed)\n");
            }
        }
        else if (strcmp(game.console, "Nintendo DS") == 0) {
            //Get file path
            char save_path[512];
            char path[256];
            snprintf(path, sizeof(path), "%s", game.romPath);
            const char* filename = strrchr(path, '/');
            filename = filename ? filename + 1 : path; 
            char* dot = strrchr(filename, '.');
            if (dot) {
                *dot = '\0';
            }
            snprintf(save_path, sizeof(save_path), "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/ds/%s.sav", filename);
            //Remove save file
            if (remove(save_path) == 0) {
                printf("Cleared save data: %s\n", save_path);
            } 
            else {
                printf("No save data to clear (or delete failed): %s\n", save_path);
            }
        }
        else if (strcmp(game.console, "Nintendo GameCube") == 0) {
            //Get file path
            char save_path[512];
            snprintf(save_path, sizeof(save_path), "/home/tywebb1724/.var/app/org.DolphinEmu.dolphin-emu/data/dolphin-emu/GC/USA/Card A/%s.gci", game.serial);
            //Remove save file
            if (remove(save_path) == 0) {
                printf("Cleared save data: %s\n", save_path);
            } 
            else {
                printf("No save data to clear (or delete failed): %s\n", save_path);
            }
        }
        else if (strcmp(game.console, "Sega Dreamcast") == 0) {
            //Get file path
            char save_path[512];
            snprintf(save_path, sizeof(save_path), "/home/tywebb1724/.var/app/org.flycast.Flycast/data/flycast/%s_vmu_save_A1.bin", game.serial);
            //Remove save file
            if (remove(save_path) == 0) {
                printf("Cleared save data: %s\n", save_path);
            } 
            else {
                printf("No save data to clear (or delete failed): %s\n", save_path);
            }
        }
    }
}

//Initialize arcade games
static void Games_Arcade_Init() {
    //Title, cover, and rom for each game
    gamesLibrary[0].title = "Metal Slug 3";
    gamesLibrary[0].coverPath = "./assets/covers/arcade/metal_slug_3.png";
    gamesLibrary[0].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/arcade/mslug3.zip";
    
    gamesLibrary[1].title = "Pac-Man";
    gamesLibrary[1].coverPath = "./assets/covers/arcade/pac-man.png";
    gamesLibrary[1].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/arcade/pacman.zip";

    gamesLibrary[2].title = "Simpsons Arcade Game";
    gamesLibrary[2].coverPath = "./assets/covers/arcade/simpsons_arcade.png";
    gamesLibrary[2].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/arcade/simpsons2p.zip";

    gamesLibrary[3].title = "Street Fighter Alpha 3";
    gamesLibrary[3].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/arcade/sf_alpha_3.png";
    gamesLibrary[3].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/arcade/sfa3.zip";

    gamesLibrary[4].title = "Teenage Mutant Ninja Turtles: Turtles in Time";
    gamesLibrary[4].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/arcade/tmnt_turt_in_time.png";
    gamesLibrary[4].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/arcade/tmnt22pu.zip";

    gamesLibrary[5].title = "The Punisher Arcade";
    gamesLibrary[5].coverPath = "./assets/covers/arcade/punisher_arcade.png";
    gamesLibrary[5].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/arcade/punisher.zip";

    gamesLibrary[6].title = "X-Men Arcade";
    gamesLibrary[6].coverPath = "./assets/covers/arcade/x-men_arcade.png";
    gamesLibrary[6].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/arcade/xmen2pa.zip";

    //All arcade games have same save type, path, console, and category. All are libretro as well
    for (int i = 0; i < 7; i++) {
        gamesLibrary[i].save = NONE;
        gamesLibrary[i].libRetro = true;
        gamesLibrary[i].corePath = PATH_ARCADE;
        gamesLibrary[i].console = "Arcade";
        gamesLibrary[i].category = "Arcade";
    }
}

//Initialize handheld classics games
static void Games_Handheld_Init() {
    //Title, cover, console, rom, and (if applys) serial for each game
    gamesLibrary[7].title = "Grand Theft Auto: Liberty City Stories";
    gamesLibrary[7].coverPath = "./assets/covers/handheld/gta_liberty.png";
    gamesLibrary[7].console = "Sony PlayStation Portable";
    gamesLibrary[7].romPath = "assets/roms/psp/GTALibertyCity.iso";
    gamesLibrary[7].serial = "ULUS10041S0";

    gamesLibrary[8].title = "Mario Kart DS";
    gamesLibrary[8].coverPath = "./assets/covers/handheld/mario_kart_ds.png";
    gamesLibrary[8].console = "Nintendo DS";
    gamesLibrary[8].romPath = "assets/roms/ds/MarioKartDS.nds";

    gamesLibrary[9].title = "Pokemon HeartGold";
    gamesLibrary[9].coverPath = "./assets/covers/handheld/pokemon_heartgold.png";
    gamesLibrary[9].console = "Nintendo DS";
    gamesLibrary[9].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/ds/PokemonHeartGold.nds";

    gamesLibrary[10].title = "Pokemon SoulSilver";
    gamesLibrary[10].coverPath = "./assets/covers/handheld/pokemon_soulsilver.png";
    gamesLibrary[10].console = "Nintendo DS";
    gamesLibrary[10].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/ds/PokemonSoulSilver.nds";

    gamesLibrary[11].title = "The Legend of Zelda: The Minish Cap";
    gamesLibrary[11].coverPath = "./assets/covers/handheld/zelda_minish_cap.png";
    gamesLibrary[11].console = "Game Boy Advance";
    gamesLibrary[11].romPath = "assets/roms/gba/MinishChap.gba";

    gamesLibrary[12].title = "Chrono Trigger";
    gamesLibrary[12].coverPath = "./assets/covers/handheld/chrono_trigger.png";
    gamesLibrary[12].console = "Nintendo DS";
    gamesLibrary[12].romPath = "assets/roms/ds/ChronoTrigger.nds";

    gamesLibrary[13].title = "Monster Hunter Freedom Unite";
    gamesLibrary[13].coverPath = "./assets/covers/handheld/monst_hunt_free_unite.png";
    gamesLibrary[13].console = "Sony PlayStation Portable";
    gamesLibrary[13].romPath = "assets/roms/psp/MonsterHunterFreeUnite.iso";
    gamesLibrary[13].serial = "ULUS10391";

    gamesLibrary[14].title = "Super Mario Bros. Deluxe";
    gamesLibrary[14].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/handheld/mario_bros_deluxe.png";
    gamesLibrary[14].console = "Game Boy Color";
    gamesLibrary[14].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/gbc/SupMarBrosDeluxe.gbc";

    gamesLibrary[15].title = "Metal Slug - 2nd Mission";
    gamesLibrary[15].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/handheld/met_slug_2nd.png";
    gamesLibrary[15].console = "Neo Geo Pocket Color";
    gamesLibrary[15].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/neogeoc/MetalSlug2ndMission.ngc";

    gamesLibrary[16].title = "Ninja Gaiden";
    gamesLibrary[16].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/handheld/ninja_gaiden.png";
    gamesLibrary[16].console = "Atari Lynx";
    gamesLibrary[16].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/lynx/NinjaGaiden.lyx";

    gamesLibrary[17].title = "Sonic Blast";
    gamesLibrary[17].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/handheld/sonic_blast.png";
    gamesLibrary[17].console = "Sega Game Gear";
    gamesLibrary[17].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/gamegear/SonicBlast.gg";

    //All games have same category. The paths and save type and whether they are libretro depends on the game
    for (int i = 7; i < 18; i++) {
        gamesLibrary[i].category = "Handheld Classics";
        //Depending on the console, set the path, save type, and whether it is libretro
        if (strcmp(gamesLibrary[i].console, "Sega Game Gear") == 0) {
            gamesLibrary[i].corePath = PATH_GENESIS;
            gamesLibrary[i].libRetro = true;
            gamesLibrary[i].save = NONE;
        }
        else if (strcmp(gamesLibrary[i].console, "Atari Lynx") == 0) {
            gamesLibrary[i].corePath = PATH_LYNX;
            gamesLibrary[i].libRetro = true;
            gamesLibrary[i].save = NONE;
        }
        else if (strcmp(gamesLibrary[i].console, "Neo Geo Pocket Color") == 0) {
            gamesLibrary[i].corePath = PATH_NGPC;
            gamesLibrary[i].libRetro = true;
            gamesLibrary[i].save = NONE;
        }
        else if (strcmp(gamesLibrary[i].console, "Game Boy Color") == 0) {
            gamesLibrary[i].corePath = PATH_GAMEBOY;
            gamesLibrary[i].libRetro = true;
            gamesLibrary[i].save = BATTERY;
        }
        else if (strcmp(gamesLibrary[i].console, "Sony PlayStation Portable") == 0) {
            gamesLibrary[i].corePath = PATH_PSP;
            gamesLibrary[i].libRetro = false;
            gamesLibrary[i].save = EXTERNAL;
        }
        else if (strcmp(gamesLibrary[i].console, "Nintendo DS") == 0) {
            gamesLibrary[i].corePath = PATH_DS;
            gamesLibrary[i].libRetro = false;
            gamesLibrary[i].save = EXTERNAL;
        }
        else if (strcmp(gamesLibrary[i].console, "Game Boy Advance") == 0) {
            gamesLibrary[i].corePath = PATH_GBA;
            gamesLibrary[i].libRetro = true;
            gamesLibrary[i].save = BATTERY;
        }
    }
}

//Initialize Nintendo 3D games
static void Games_Nint3D_Init() {
    //Titles, covers, consoles, and roms for each game
    gamesLibrary[18].title = "Super Mario 64";
    gamesLibrary[18].coverPath = "./assets/covers/nint_3d/mario_64.png";
    gamesLibrary[18].console = "Nintendo 64";
    gamesLibrary[18].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/n64/SuperMario64.z64";

    gamesLibrary[19].title = "Super Mario Sunshine";
    gamesLibrary[19].coverPath = "./assets/covers/nint_3d/mario_sunshine.png";
    gamesLibrary[19].console = "Nintendo GameCube";
    gamesLibrary[19].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/gamecube/MarioSunshine.ciso";
    gamesLibrary[19].serial = "01-GMSE-super_mario_sunshine";

    gamesLibrary[20].title = "Starfox 64";
    gamesLibrary[20].coverPath = "./assets/covers/nint_3d/starfox_64.png";
    gamesLibrary[20].console = "Nintendo 64";
    gamesLibrary[20].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/n64/StarFox64.z64";

    gamesLibrary[21].title = "The Legend of Zelda: Ocarina of Time";
    gamesLibrary[21].coverPath = "./assets/covers/nint_3d/zelda_ocarina.png";
    gamesLibrary[21].console = "Nintendo 64";
    gamesLibrary[21].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/n64/OcarinaOfTime.z64";

    gamesLibrary[22].title = "The Legend of Zelda: The Wind Waker";
    gamesLibrary[22].coverPath = "./assets/covers/nint_3d/zelda_windwaker.png";
    gamesLibrary[22].console = "Nintendo GameCube";
    gamesLibrary[22].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/gamecube/WindWaker.ciso";
    gamesLibrary[22].serial = "01-GZLE-gczelda";

    gamesLibrary[23].title = "Super Smash Bros: Melee";
    gamesLibrary[23].coverPath = "./assets/covers/nint_3d/smash_bros_melee.png";
    gamesLibrary[23].console = "Nintendo GameCube";
    gamesLibrary[23].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/gamecube/SmashBrosMelee.ciso";
    gamesLibrary[23].serial = "01-GALE-SuperSmashBros0110290334";

    gamesLibrary[24].title = "The Legend of Zelda: Twilight Princess";
    gamesLibrary[24].coverPath = "./assets/covers/nint_3d/zelda_twilight.png";
    gamesLibrary[24].console = "Nintendo GameCube";
    gamesLibrary[24].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/gamecube/TwilightPrincess.ciso";
    gamesLibrary[24].serial = "01-GZ2E-gczelda2";

    //All have same category. Path and save type and whether it is libretro depends on the console
    for (int i = 18; i < 25; i++) {
        gamesLibrary[i].category = "Nintendo 3D";
        //Depending on the console, set the path, save type, and whether it is libretro
        if (strcmp(gamesLibrary[i].console, "Nintendo GameCube") == 0) {
            gamesLibrary[i].corePath = PATH_GAMECUBE;
            gamesLibrary[i].libRetro = false;
            gamesLibrary[i].save = EXTERNAL;
        }
        else if (strcmp(gamesLibrary[i].console, "Nintendo 64") == 0) {
            gamesLibrary[i].corePath = PATH_N64;
            gamesLibrary[i].libRetro = true;
            gamesLibrary[i].save = BATTERY;
        }
    }
}

//Initialize retro Nintendo games
static void Games_NintRet_Init() {
    //Titles, covers, consoles, and roms for each game
    gamesLibrary[25].title = "Donkey Kong Country 2: Diddy's Kong Quest";
    gamesLibrary[25].coverPath = "./assets/covers/nint_ret/dk_country_2.png";
    gamesLibrary[25].console = "Super Nintendo Entertainment System";
    gamesLibrary[25].romPath = "assets/roms/snes/DK2.smc";

    gamesLibrary[26].title = "Mike Tyson's Punch-Out!!";
    gamesLibrary[26].coverPath = "./assets/covers/nint_ret/punch-out.png";
    gamesLibrary[26].console = "Nintendo Entertainment System";
    gamesLibrary[26].romPath = "assets/roms/snes/Punch-Out.nes";

    gamesLibrary[27].title = "Super Mario World";
    gamesLibrary[27].coverPath = "./assets/covers/nint_ret/super_mario_world.png";
    gamesLibrary[27].console = "Super Nintendo Entertainment System";
    gamesLibrary[27].romPath = "assets/roms/snes/SuperMarioWorld.smc";

    gamesLibrary[28].title = "Super Metroid";
    gamesLibrary[28].coverPath = "./assets/covers/nint_ret/super_metroid.png";
    gamesLibrary[28].console = "Super Nintendo Entertainment System";
    gamesLibrary[28].romPath = "assets/roms/snes/SuperMetroid.smc";

    gamesLibrary[29].title = "The Legend of Zelda";
    gamesLibrary[29].coverPath = "./assets/covers/nint_ret/zelda.png";
    gamesLibrary[29].console = "Nintendo Entertainment System";
    gamesLibrary[29].romPath = "assets/roms/nes/LegendOfZelda.nes";

    gamesLibrary[30].title = "EarthBound";
    gamesLibrary[30].coverPath = "./assets/covers/nint_ret/earth_bound.png";
    gamesLibrary[30].console = "Super Nintendo Entertainment System";
    gamesLibrary[30].romPath = "assets/roms/snes/EarthBound.sfc";

    gamesLibrary[31].title = "Megaman X2";
    gamesLibrary[31].coverPath = "./assets/covers/nint_ret/megaman_x2.png";
    gamesLibrary[31].console = "Super Nintendo Entertainment System";
    gamesLibrary[31].romPath = "assets/roms/snes/MegamanX2.sfc";

    //All have same category and save type and all are libretro. Path depends on console
    for (int i = 25; i < 32; i++) {
        gamesLibrary[i].libRetro = true;
        gamesLibrary[i].save = BATTERY;
        gamesLibrary[i].category = "Retro Nintendo";
        //Depending on the console, set the path
        if (strcmp(gamesLibrary[i].console, "Super Nintendo Entertainment System") == 0) {
            gamesLibrary[i].corePath = PATH_SNES;
        }
        else if (strcmp(gamesLibrary[i].console, "Nintendo Entertainment System") == 0) {
            gamesLibrary[i].corePath = PATH_NES;
        }
    }
}

//Initialize retro PC & Indie games
static void Games_PCIndie_Init() {
    //Titles, covers, consoles, and roms for each game
    gamesLibrary[32].title = "Blazing Lazers";
    gamesLibrary[32].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/pc_ind/blaz_laz.png";
    gamesLibrary[32].console = "TurboGrafx-16";
    gamesLibrary[32].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/pc/BlazingLazers.pce";

    gamesLibrary[33].title = "Doom";
    gamesLibrary[33].coverPath = "./assets/covers/pc_ind/doom_2.png";
    gamesLibrary[33].console = "PC";
    gamesLibrary[33].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/pc/DOOM.WAD";

    gamesLibrary[34].title = "Doom 2";
    gamesLibrary[34].coverPath = "./assets/covers/pc_ind/doom.png";
    gamesLibrary[34].console = "PC";
    gamesLibrary[34].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/pc/DOOM2.WAD";

    gamesLibrary[35].title = "Ys Book I & II";
    gamesLibrary[35].coverPath = "./assets/covers/pc_ind/ys.png";
    gamesLibrary[35].console = "TurboGrafx-CD";
    gamesLibrary[35].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/pc/YsBook.chd";

    gamesLibrary[36].title = "Lords of Thunder";
    gamesLibrary[36].coverPath = "./assets/covers/pc_ind/lord_thund.png";
    gamesLibrary[36].console = "TurboGrafx-CD";
    gamesLibrary[36].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/pc/LordofThund.chd";

    gamesLibrary[37].title = "Air Zonk";
    gamesLibrary[37].coverPath = "./assets/covers/pc_ind/air_zonk.png";
    gamesLibrary[37].console = "TurboGrafx-16";
    gamesLibrary[37].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/pc/AirZonk.pce";

    gamesLibrary[38].title = "Dragon Slayer - The Legend of Heroes";
    gamesLibrary[38].coverPath = "./assets/covers/pc_ind/drag_slay.png";
    gamesLibrary[38].console = "TurboGrafx-CD";
    gamesLibrary[38].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/pc/DragSlay.chd";

    gamesLibrary[39].title = "Splatterhouse";
    gamesLibrary[39].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/pc_ind/splatterhouse.png";
    gamesLibrary[39].console = "TurboGrafx-16";
    gamesLibrary[39].romPath = "assets/roms/trbogrfx/Splatterhouse.pce";

    gamesLibrary[40].title = "Cosmic Fantasy 2";
    gamesLibrary[40].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/pc_ind/cos_fant_2.png";
    gamesLibrary[40].console = "TurboGrafx-CD";
    gamesLibrary[40].romPath = "assets/roms/trbogrfx/CosmicFantasy2.chd";

    //All have same category and save type and all are libretro. Path depends on console
    for (int i = 32; i < 41; i++) {
        gamesLibrary[i].category = "TurboGrafx/PC/Other";
        gamesLibrary[i].save = BATTERY;
        gamesLibrary[i].libRetro = true;
        //Depending on the console, set the path
        if (strcmp(gamesLibrary[i].console, "TurboGrafx-16") == 0 || strcmp(gamesLibrary[i].console, "TurboGrafx-CD") == 0) {
            gamesLibrary[i].corePath = PATH_TG16;
        }
        else if (strcmp(gamesLibrary[i].console, "PC") == 0) {
            gamesLibrary[i].corePath = PATH_PRBOOM;
        }
    }
}

//Initialize Sega games
static void Games_Sega_Init() {
    //Titles, covers, consoles, roms, and (if applys) serials for each game
    gamesLibrary[41].title = "Crazy Taxi";
    gamesLibrary[41].coverPath = "./assets/covers/sega/crazy_taxi.png";
    gamesLibrary[41].console = "Sega Dreamcast";
    gamesLibrary[41].romPath = "assets/roms/dreamcast/CrazyTaxi.chd";
    gamesLibrary[41].serial = "MK-51035";

    gamesLibrary[42].title = "Fighting Vipers";
    gamesLibrary[42].coverPath = "./assets/covers/sega/fighting_vipers.png";
    gamesLibrary[42].console = "Sega Saturn";
    gamesLibrary[42].romPath = "assets/roms/saturn/FightingVipers.chd";
    gamesLibrary[42].serial = "B1334F8949ED55769416D83D7A19F1C5";

    gamesLibrary[43].title = "Sonic the Hedgehog 3 & Knuckles";
    gamesLibrary[43].coverPath = "./assets/covers/sega/sonic_3_and_knuckles.png";
    gamesLibrary[43].console = "Sega Genesis";
    gamesLibrary[43].romPath = "assets/roms/genesis/Sonic3&Knuckles.md";

    gamesLibrary[44].title = "Sonic CD";
    gamesLibrary[44].coverPath = "./assets/covers/sega/sonic_cd.png";
    gamesLibrary[44].console = "Sega CD";
    gamesLibrary[44].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/cd/SonicCD.chd";
    
    gamesLibrary[45].title = "NBA Jam: Tournament Edition";
    gamesLibrary[45].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/sega/nba_jam_te.png";
    gamesLibrary[45].console = "Sega Saturn";
    gamesLibrary[45].romPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/roms/saturn/NBAJam.chd";
    gamesLibrary[45].serial = "DBA9351F1D9322E87F3D7F511992DEB3";

    gamesLibrary[46].title = "Daytona USA";
    gamesLibrary[46].coverPath = "./assets/covers/sega/daytona_usa.png";
    gamesLibrary[46].console = "Sega Saturn";
    gamesLibrary[46].romPath = "assets/roms/saturn/DaytonaUSA.chd";
    gamesLibrary[46].serial = "C8355C918C5A97B9E4AB9322DDAFDB1E";

    gamesLibrary[47].title = "Gunstar Heroes";
    gamesLibrary[47].coverPath = "./assets/covers/sega/gunstar_heroes.png";
    gamesLibrary[47].console = "Sega Genesis";
    gamesLibrary[47].romPath = "assets/roms/genesis/GunstarHeroes.md";

    gamesLibrary[48].title = "Wonder Boy";
    gamesLibrary[48].coverPath = "/home/tywebb1724/Desktop/Gaming-Console/assets/covers/sega/wonder_boy.png";
    gamesLibrary[48].console = "Sega Master System";
    gamesLibrary[48].romPath = "assets/roms/mastsys/WonderBoy.sms";

    //All have same category. Path, save type, and whether it is libretro depends on console
    for (int i = 41; i < 49; i++) {
        gamesLibrary[i].category = "Sega";
        //Depending on the console, set the path, save type, and whether it is libretro
        if (strcmp(gamesLibrary[i].console, "Sega Master System") == 0) {
            gamesLibrary[i].libRetro = true;
            gamesLibrary[i].save = NONE;
            gamesLibrary[i].corePath = PATH_GENESIS;
        }
        else if (strcmp(gamesLibrary[i].console, "Sega Genesis") == 0) {
            gamesLibrary[i].libRetro = true;
            gamesLibrary[i].save = BATTERY;
            gamesLibrary[i].corePath = PATH_GENESIS;
        }
        else if (strcmp(gamesLibrary[i].console, "Sega Saturn") == 0) {
            gamesLibrary[i].libRetro = false;
            gamesLibrary[i].save = EXTERNAL;
            gamesLibrary[i].corePath = PATH_SATURN;
        }
        else if (strcmp(gamesLibrary[i].console, "Sega CD") == 0) {
            gamesLibrary[i].libRetro = true;
            gamesLibrary[i].save = EXTERNAL;
            gamesLibrary[i].corePath = PATH_GENESIS;
        }
        else if (strcmp(gamesLibrary[i].console, "Sega Dreamcast") == 0) {
            gamesLibrary[i].libRetro = false;
            gamesLibrary[i].save = EXTERNAL;
            gamesLibrary[i].corePath = PATH_DREAMCAST;
        }
    }
}

//Initialize Playstation games
static void Games_Playstation_Init() {
    //Titles, covers, roms, and serials for each game
    gamesLibrary[49].title = "Mega Man X4";
    gamesLibrary[49].coverPath = "./assets/covers/playstation/megaman_x4.png";
    gamesLibrary[49].romPath = "assets/roms/ps1/MegaManX4.chd";
    gamesLibrary[49].serial = "SLUS-00561_1";

    gamesLibrary[50].title = "Pac-Man World";
    gamesLibrary[50].coverPath = "./assets/covers/playstation/pac-man_world.png";
    gamesLibrary[50].romPath = "assets/roms/ps1/Pac-ManWorld.chd";
    gamesLibrary[50].serial = "SLUS-00439_1";

    gamesLibrary[51].title = "Soul Blade";
    gamesLibrary[51].coverPath = "./assets/covers/playstation/soulblade.png";
    gamesLibrary[51].romPath = "assets/roms/ps1/SoulBlade.chd";
    gamesLibrary[51].serial = "SLUS-00240_1";

    gamesLibrary[52].title = "Spider-Man (2000)";
    gamesLibrary[52].coverPath = "./assets/covers/playstation/spider-man.png";
    gamesLibrary[52].romPath = "assets/roms/ps1/Spider-Man.chd";
    gamesLibrary[52].serial = "SLUS-00875_1";

    gamesLibrary[53].title = "Street Fighter: Alpha 3";
    gamesLibrary[53].coverPath = "./assets/covers/playstation/street_fighter_alpha_3.png";
    gamesLibrary[53].romPath = "assets/roms/ps1/StreetFighterAlpha3.chd";
    gamesLibrary[53].serial = "SLUS-00821_1";

    gamesLibrary[54].title = "Metal Gear Solid";
    gamesLibrary[54].coverPath = "./assets/covers/playstation/met_gear_solid.png";
    gamesLibrary[54].romPath = "assets/roms/ps1/MetalGearSolid.m3u";
    gamesLibrary[54].serial = "SLUS-00594_1";

    gamesLibrary[55].title = "Twisted Metal 2";
    gamesLibrary[55].coverPath = "./assets/covers/playstation/twist_met_2.png";
    gamesLibrary[55].romPath = "assets/roms/ps1/TwistedMetal2.chd";
    gamesLibrary[55].serial = "SCUS-94306_1";

    //All have same category, save type, path, and console and all are libretro
    for (int i = 49; i < 56; i++) {
        gamesLibrary[i].category = "PlayStation";
        gamesLibrary[i].libRetro = true;
        gamesLibrary[i].save = EXTERNAL;
        gamesLibrary[i].console = "Sony PlayStation";
        gamesLibrary[i].corePath = PATH_PS1;
    }
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

    //Start with the Nintendo 3D games
    Games_UpdateIndexes("Nintendo 3D");
    Games_Refresh();
}

//Shift the order of the games to the right
void Games_ScrollRight() {
    games_index = start_index + (games_index - start_index + 1) % games_range;
    Games_Refresh();
}

//Shift the order of the games to the left
void Games_ScrollLeft() {
    games_index = start_index + (games_index - start_index - 1 + games_range) % games_range;
    Games_Refresh();
}

//Get one of the loaded images
Image* Games_GetLoadedImage(int i) {
    return &LoadedImages[i];
}

//Get whether an image is loaded
bool Games_GetIsLoaded(int i) {
    return isLoaded[i];
}

//Load game cover textures
void* Games_LoadImages(void *args) {
    //Detach the thread
    pthread_detach(pthread_self());
    //Load the images of the current category first
    for (int i = start_index; i <= end_index; i++) {
        LoadedImages[i] = LoadImage(gamesLibrary[i].coverPath);
        isLoaded[i] = true;
    }
    //Load the images at beginning of array
    for (int i = 0; i < start_index; i++) {
        LoadedImages[i] = LoadImage(gamesLibrary[i].coverPath);
        isLoaded[i] = true;
    }
    //Load then images at end of array
    for (int i = end_index + 1; i < GAMES_LEN; i++) {
        LoadedImages[i] = LoadImage(gamesLibrary[i].coverPath);
        isLoaded[i] = true;
    }
    return NULL;
}

//Unload game cover textures
void Games_UnloadTextures() {
    //Unload all covers
    for (int i = 0; i < GAMES_LEN; i++) {
        UnloadTexture(gamesLibrary[i].cover);
    }
}