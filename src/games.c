#include "games.h"
#include <pthread.h>
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdatomic.h>


//Arrays to hold all games, the games displayed, and the new games displayed
static game_t gamesLibrary[MAX_GAMES];
static game_t* gamesDisplayed[GAMES_ON_SCREEN + DISPLAY_CENTER_OFFSET];
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
static atomic_bool isLoaded[GAMES_LEN] = { false };
//Indexes for initializing the games
static int games_init_index = 0;
static int temp_start_section = 0;


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
    char categ[CATEG_STR_LEN];
    //If scrolling to the right
    if (direction == RIGHT) {
        //If it is the last category in the array, the new category starts at the beginning
        if (end_index == GAMES_LEN - 1) {
            start_index_new = 1;
        }
        //If not, update the new start index
        else {
            start_index_new = end_index + 2;
        }
    }
    //If scrolling to the left
    else {
        //If the first category in the array, new category is the last one
        if (start_index == 0) {
            snprintf(categ, sizeof(categ), "%s", gamesLibrary[GAMES_LEN - 1].category);
        }
        //If not, it's the previous category
        else {
            snprintf(categ, sizeof(categ), "%s", gamesLibrary[start_index - 1].category);
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
    games_range = end_index - start_index + 1;
    games_index = start_index + DISPLAY_CENTER_OFFSET;
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
    //Update all games displayed
    for (int i = 0; i < GAMES_ON_SCREEN + DISPLAY_CENTER_OFFSET; i++) {
        offset = i - DISPLAY_CENTER_OFFSET;
        targetIndex = start_index + (games_index - start_index + offset + games_range) % (games_range);
        gamesDisplayed[i] = &gamesLibrary[targetIndex];
    }
}

//Get the home path
static bool Games_BuildHomePath(char* outPath, size_t outSize, const char* relativePath) {
    const char* home = getenv("HOME");
    //If error getting home path, exit with error
    if (!home) {
        fprintf(stderr, "Games_BuildHomePath: HOME environment variable not set\n");
        return false;
    }
    int written = snprintf(outPath, outSize, "%s/%s", home, relativePath);
    //If path invalid, exit with error
    if (written < 0 || (size_t)written >= outSize) {
        fprintf(stderr, "Games_BuildHomePath: path too long, truncated\n");
        return false;
    }
    return true;
}

//Clear game data
bool Games_ClearData(const game_t* game) {
    //If saves through battery method
    if (game->save == BATTERY) {
        char save_path[SAVE_PATH_LEN];
        snprintf(save_path, sizeof(save_path), "%s.srm", game->romPath);
        //Remove save file
        if (remove(save_path) == 0) {
            printf("Cleared save data: %s\n", save_path);
            return true;
        } 
        else {
            printf("No save data to clear (or delete failed): %s\n", save_path);
            return false;
        }
    }
    //If saves through other method
    else if (game->save == EXTERNAL) {
        //Check which console
        if (strcmp(game->console, "Sony PlayStation") == 0) {
            char save_path[SAVE_PATH_LEN];
            snprintf(save_path, sizeof(save_path), "assets/saves/%s.mcd", game->serial);
            //Remove save file
            if (remove(save_path) == 0) {
                printf("Cleared save data: %s\n", save_path);
                return true;
            } 
            else {
                printf("No save data to clear (or delete failed): %s\n", save_path);
                return false;
            }
        }
        else if (strcmp(game->console, "Sega CD") == 0) {
            char save_path[SAVE_PATH_LEN];
            char path[TEMP_PATH_LEN];
            snprintf(path, sizeof(path), "%s", game->romPath);
            const char* filename = strrchr(path, '/');
            filename = filename ? filename + 1 : path; 
            char* dot = strrchr(filename, '.');
            //Cut it off at the dot
            if (dot) {
                *dot = '\0';
            }
            snprintf(save_path, sizeof(save_path), "assets/saves/%s.brm", filename);
            //Remove save file
            if (remove(save_path) == 0) {
                printf("Cleared save data: %s\n", save_path);
                return true;
            } 
            else {
                printf("No save data to clear (or delete failed): %s\n", save_path);
                return false;
            }
        }
        else if (strcmp(game->console, "Sony PlayStation Portable") == 0) {
            char command[COMMAND_STR_LEN] = "";
            char rel_path[SAVE_PATH_LEN];
            snprintf(rel_path, sizeof(rel_path), ".var/app/org.ppsspp.PPSSPP/config/ppsspp/PSP/SAVEDATA/%s", game->serial);
            char save_path[SAVE_PATH_LEN];
            //Get home path
            if (Games_BuildHomePath(save_path, sizeof(save_path), rel_path)) {
                snprintf(command, sizeof(command), "rm -rf \"%s\"", save_path);
                int result = system(command);
                //Check if command worked
                if (result == 0) {
                    printf("Cleared PSP save data for %s\n", game->serial);
                    return true;
                } 
                else {
                    printf("Failed to clear PSP save (or none existed)\n");
                    return false;
                }
            }
            else {
                printf("Could not resolve save path (HOME not set)\n");
                return false;
            }
        }
        else if (strcmp(game->console, "Sega Saturn") == 0) {
            char command[COMMAND_STR_LEN] = "";
            char rel_path[SAVE_PATH_LEN];
            snprintf(rel_path, sizeof(rel_path), ".var/app/io.github.strikerx3.ymir/data/StrikerX3/Ymir/savestates/%s", game->serial);
            char save_path[SAVE_PATH_LEN];
            //Get home path
            if (Games_BuildHomePath(save_path, sizeof(save_path), rel_path)) {
                snprintf(command, sizeof(command), "rm -rf \"%s\"", save_path);
                int result = system(command);
                //Check if command worked
                if (result == 0) {
                    printf("Cleared Saturn save data for %s\n", game->serial);
                    return true;
                } 
                else {
                    printf("Failed to clear Saturn save (or none existed)\n");
                    return false;
                }
            }
            else {
                printf("Could not resolve save path (HOME not set)\n");
                return false;
            }
        }
        else if (strcmp(game->console, "Nintendo DS") == 0) {
            char save_path[SAVE_PATH_LEN];
            char path[TEMP_PATH_LEN];
            snprintf(path, sizeof(path), "%s", game->romPath);
            const char* filename = strrchr(path, '/');
            filename = filename ? filename + 1 : path; 
            char* dot = strrchr(filename, '.');
            //Cut it off at the dot
            if (dot) {
                *dot = '\0';
            }
            snprintf(save_path, sizeof(save_path), "assets/roms/ds/%s.sav", filename);
            //Remove save file
            if (remove(save_path) == 0) {
                printf("Cleared save data: %s\n", save_path);
                return true;
            } 
            else {
                printf("No save data to clear (or delete failed): %s\n", save_path);
                return false;
            }
        }
        else if (strcmp(game->console, "Nintendo GameCube") == 0) {
            char rel_path[SAVE_PATH_LEN];
            snprintf(rel_path, sizeof(rel_path), ".var/app/org.DolphinEmu.dolphin-emu/data/dolphin-emu/GC/USA/Card A/%s.gci", game->serial);
            char save_path[SAVE_PATH_LEN];
            //Get home path
            if (Games_BuildHomePath(save_path, sizeof(save_path), rel_path)) {
                //Remove save file
                if (remove(save_path) == 0) {
                    printf("Cleared save data: %s\n", save_path);
                    return true;
                } 
                else {
                    printf("No save data to clear (or delete failed): %s\n", save_path);
                    return false;
                }
            }
            else {
                printf("Could not resolve save path (HOME not set)\n");
                return false;
            }
        }
        else if (strcmp(game->console, "Sega Dreamcast") == 0) {
            char rel_path[SAVE_PATH_LEN];
            snprintf(rel_path, sizeof(rel_path), ".var/app/org.flycast.Flycast/data/flycast/%s_vmu_save_A1.bin", game->serial);
            char save_path[SAVE_PATH_LEN];
            //Get home path
            if (Games_BuildHomePath(save_path, sizeof(save_path), rel_path)) {
                //Remove save file
                if (remove(save_path) == 0) {
                    printf("Cleared save data: %s\n", save_path);
                    return true;
                } 
                else {
                    printf("No save data to clear (or delete failed): %s\n", save_path);
                    return false;
                }
            }
            else {
                printf("Could not resolve save path (HOME not set)\n");
                return false;
            }
        }
    }
    return false;
}

//Initialize arcade games
static void Games_Arcade_Init() {
    temp_start_section = games_init_index;
    //Title, cover, and rom for each game
    gamesLibrary[games_init_index].title = "Metal Slug 3";
    gamesLibrary[games_init_index].coverPath = "assets/images/arcade/metal_slug_3.png";
    gamesLibrary[games_init_index].romPath = "assets/roms/arcade/mslug3.zip";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Pac-Man";
    gamesLibrary[games_init_index].coverPath = "assets/images/arcade/pac-man.png";
    gamesLibrary[games_init_index].romPath = "assets/roms/arcade/pacman.zip";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Simpsons Arcade Game";
    gamesLibrary[games_init_index].coverPath = "assets/images/arcade/simpsons_arcade.png";
    gamesLibrary[games_init_index].romPath = "assets/roms/arcade/simpsons2p.zip";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Street Fighter Alpha 3";
    gamesLibrary[games_init_index].coverPath = "assets/images/arcade/sf_alpha_3.png";
    gamesLibrary[games_init_index].romPath = "assets/roms/arcade/sfa3.zip";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Teenage Mutant Ninja Turtles: Turtles in Time";
    gamesLibrary[games_init_index].coverPath = "assets/images/arcade/tmnt_turt_in_time.png";
    gamesLibrary[games_init_index].romPath = "assets/roms/arcade/tmnt22pu.zip";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "The Punisher Arcade";
    gamesLibrary[games_init_index].coverPath = "assets/images/arcade/punisher_arcade.png";
    gamesLibrary[games_init_index].romPath = "assets/roms/arcade/punisher.zip";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "X-Men Arcade";
    gamesLibrary[games_init_index].coverPath = "assets/images/arcade/x-men_arcade.png";
    gamesLibrary[games_init_index].romPath = "assets/roms/arcade/xmen2pa.zip";
    games_init_index += 1;
    //All arcade games have same save type, path, console, and category. All are libretro as well
    for (int i = temp_start_section; i < games_init_index; i++) {
        gamesLibrary[i].save = NONE;
        gamesLibrary[i].libRetro = true;
        gamesLibrary[i].corePath = PATH_ARCADE;
        gamesLibrary[i].console = "Arcade";
        gamesLibrary[i].category = "Arcade";
    }
}

//Initialize handheld classics games
static void Games_Handheld_Init() {
    temp_start_section = games_init_index;
    //Title, cover, console, rom, and (if applys) serial for each game
    gamesLibrary[games_init_index].title = "Grand Theft Auto: Liberty City Stories";
    gamesLibrary[games_init_index].coverPath = "assets/images/handheld/gta_liberty.png";
    gamesLibrary[games_init_index].console = "Sony PlayStation Portable";
    gamesLibrary[games_init_index].romPath = "assets/roms/psp/GTALibertyCity.iso";
    gamesLibrary[games_init_index].serial = "ULUS10041S0";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Mario Kart DS";
    gamesLibrary[games_init_index].coverPath = "assets/images/handheld/mario_kart_ds.png";
    gamesLibrary[games_init_index].console = "Nintendo DS";
    gamesLibrary[games_init_index].romPath = "assets/roms/ds/MarioKartDS.nds";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Pokemon HeartGold";
    gamesLibrary[games_init_index].coverPath = "assets/images/handheld/pokemon_heartgold.png";
    gamesLibrary[games_init_index].console = "Nintendo DS";
    gamesLibrary[games_init_index].romPath = "assets/roms/ds/PokemonHeartGold.nds";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Pokemon SoulSilver";
    gamesLibrary[games_init_index].coverPath = "assets/images/handheld/pokemon_soulsilver.png";
    gamesLibrary[games_init_index].console = "Nintendo DS";
    gamesLibrary[games_init_index].romPath = "assets/roms/ds/PokemonSoulSilver.nds";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "The Legend of Zelda: The Minish Cap";
    gamesLibrary[games_init_index].coverPath = "assets/images/handheld/zelda_minish_cap.png";
    gamesLibrary[games_init_index].console = "Game Boy Advance";
    gamesLibrary[games_init_index].romPath = "assets/roms/gba/MinishChap.gba";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Chrono Trigger";
    gamesLibrary[games_init_index].coverPath = "assets/images/handheld/chrono_trigger.png";
    gamesLibrary[games_init_index].console = "Nintendo DS";
    gamesLibrary[games_init_index].romPath = "assets/roms/ds/ChronoTrigger.nds";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Monster Hunter Freedom Unite";
    gamesLibrary[games_init_index].coverPath = "assets/images/handheld/monst_hunt_free_unite.png";
    gamesLibrary[games_init_index].console = "Sony PlayStation Portable";
    gamesLibrary[games_init_index].romPath = "assets/roms/psp/MonsterHunterFreeUnite.iso";
    gamesLibrary[games_init_index].serial = "ULUS10391";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Super Mario Bros. Deluxe";
    gamesLibrary[games_init_index].coverPath = "assets/images/handheld/mario_bros_deluxe.png";
    gamesLibrary[games_init_index].console = "Game Boy Color";
    gamesLibrary[games_init_index].romPath = "assets/roms/gbc/SupMarBrosDeluxe.gbc";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Metal Slug - 2nd Mission";
    gamesLibrary[games_init_index].coverPath = "assets/images/handheld/met_slug_2nd.png";
    gamesLibrary[games_init_index].console = "Neo Geo Pocket Color";
    gamesLibrary[games_init_index].romPath = "assets/roms/neogeoc/MetalSlug2ndMission.ngc";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Ninja Gaiden";
    gamesLibrary[games_init_index].coverPath = "assets/images/handheld/ninja_gaiden.png";
    gamesLibrary[games_init_index].console = "Atari Lynx";
    gamesLibrary[games_init_index].romPath = "assets/roms/lynx/NinjaGaiden.lyx";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Sonic Blast";
    gamesLibrary[games_init_index].coverPath = "assets/images/handheld/sonic_blast.png";
    gamesLibrary[games_init_index].console = "Sega Game Gear";
    gamesLibrary[games_init_index].romPath = "assets/roms/gamegear/SonicBlast.gg";
    games_init_index += 1;
    //All games have same category. The paths and save type and whether they are libretro depends on the game
    for (int i = temp_start_section; i < games_init_index; i++) {
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
            gamesLibrary[i].processName = "PPSSPPSDL";
        }
        else if (strcmp(gamesLibrary[i].console, "Nintendo DS") == 0) {
            gamesLibrary[i].corePath = PATH_DS;
            gamesLibrary[i].libRetro = false;
            gamesLibrary[i].save = EXTERNAL;
            gamesLibrary[i].processName = "melonDS";
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
    temp_start_section = games_init_index;
    //Titles, images, consoles, and roms for each game
    gamesLibrary[games_init_index].title = "Super Mario 64";
    gamesLibrary[games_init_index].coverPath = "assets/images/nint_3d/mario_64.png";
    gamesLibrary[games_init_index].console = "Nintendo 64";
    gamesLibrary[games_init_index].romPath = "assets/roms/n64/SuperMario64.z64";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Super Mario Sunshine";
    gamesLibrary[games_init_index].coverPath = "assets/images/nint_3d/mario_sunshine.png";
    gamesLibrary[games_init_index].console = "Nintendo GameCube";
    gamesLibrary[games_init_index].romPath = "assets/roms/gamecube/MarioSunshine.ciso";
    gamesLibrary[games_init_index].serial = "01-GMSE-super_mario_sunshine";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Starfox 64";
    gamesLibrary[games_init_index].coverPath = "assets/images/nint_3d/starfox_64.png";
    gamesLibrary[games_init_index].console = "Nintendo 64";
    gamesLibrary[games_init_index].romPath = "assets/roms/n64/StarFox64.z64";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "The Legend of Zelda: Ocarina of Time";
    gamesLibrary[games_init_index].coverPath = "assets/images/nint_3d/zelda_ocarina.png";
    gamesLibrary[games_init_index].console = "Nintendo 64";
    gamesLibrary[games_init_index].romPath = "assets/roms/n64/OcarinaOfTime.z64";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "The Legend of Zelda: The Wind Waker";
    gamesLibrary[games_init_index].coverPath = "assets/images/nint_3d/zelda_windwaker.png";
    gamesLibrary[games_init_index].console = "Nintendo GameCube";
    gamesLibrary[games_init_index].romPath = "assets/roms/gamecube/WindWaker.ciso";
    gamesLibrary[games_init_index].serial = "01-GZLE-gczelda";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Super Smash Bros: Melee";
    gamesLibrary[games_init_index].coverPath = "assets/images/nint_3d/smash_bros_melee.png";
    gamesLibrary[games_init_index].console = "Nintendo GameCube";
    gamesLibrary[games_init_index].romPath = "assets/roms/gamecube/SmashBrosMelee.ciso";
    gamesLibrary[games_init_index].serial = "01-GALE-SuperSmashBros0110290334";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "The Legend of Zelda: Twilight Princess";
    gamesLibrary[games_init_index].coverPath = "assets/images/nint_3d/zelda_twilight.png";
    gamesLibrary[games_init_index].console = "Nintendo GameCube";
    gamesLibrary[games_init_index].romPath = "assets/roms/gamecube/TwilightPrincess.ciso";
    gamesLibrary[games_init_index].serial = "01-GZ2E-gczelda2";
    games_init_index += 1;
    //All have same category. Path and save type and whether it is libretro depends on the console
    for (int i = temp_start_section; i < games_init_index; i++) {
        gamesLibrary[i].category = "Nintendo 3D";
        //Depending on the console, set the path, save type, and whether it is libretro
        if (strcmp(gamesLibrary[i].console, "Nintendo GameCube") == 0) {
            gamesLibrary[i].corePath = PATH_GAMECUBE;
            gamesLibrary[i].libRetro = false;
            gamesLibrary[i].save = EXTERNAL;
            gamesLibrary[i].processName = "dolphin-emu";
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
    temp_start_section = games_init_index;
    //Titles, images, consoles, and roms for each game
    gamesLibrary[games_init_index].title = "Donkey Kong Country 2: Diddy's Kong Quest";
    gamesLibrary[games_init_index].coverPath = "assets/images/nint_ret/dk_country_2.png";
    gamesLibrary[games_init_index].console = "Super Nintendo Entertainment System";
    gamesLibrary[games_init_index].romPath = "assets/roms/snes/DK2.smc";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Mike Tyson's Punch-Out!!";
    gamesLibrary[games_init_index].coverPath = "assets/images/nint_ret/punch-out.png";
    gamesLibrary[games_init_index].console = "Nintendo Entertainment System";
    gamesLibrary[games_init_index].romPath = "assets/roms/snes/Punch-Out.nes";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Super Mario World";
    gamesLibrary[games_init_index].coverPath = "assets/images/nint_ret/super_mario_world.png";
    gamesLibrary[games_init_index].console = "Super Nintendo Entertainment System";
    gamesLibrary[games_init_index].romPath = "assets/roms/snes/SuperMarioWorld.smc";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Super Metroid";
    gamesLibrary[games_init_index].coverPath = "assets/images/nint_ret/super_metroid.png";
    gamesLibrary[games_init_index].console = "Super Nintendo Entertainment System";
    gamesLibrary[games_init_index].romPath = "assets/roms/snes/SuperMetroid.smc";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "The Legend of Zelda";
    gamesLibrary[games_init_index].coverPath = "assets/images/nint_ret/zelda.png";
    gamesLibrary[games_init_index].console = "Nintendo Entertainment System";
    gamesLibrary[games_init_index].romPath = "assets/roms/nes/LegendOfZelda.nes";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "EarthBound";
    gamesLibrary[games_init_index].coverPath = "assets/images/nint_ret/earth_bound.png";
    gamesLibrary[games_init_index].console = "Super Nintendo Entertainment System";
    gamesLibrary[games_init_index].romPath = "assets/roms/snes/EarthBound.sfc";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Megaman X2";
    gamesLibrary[games_init_index].coverPath = "assets/images/nint_ret/megaman_x2.png";
    gamesLibrary[games_init_index].console = "Super Nintendo Entertainment System";
    gamesLibrary[games_init_index].romPath = "assets/roms/snes/MegamanX2.sfc";
    games_init_index += 1;
    //All have same category and save type and all are libretro. Path depends on console
    for (int i = temp_start_section; i < games_init_index; i++) {
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
    temp_start_section = games_init_index;
    //Titles, images, consoles, and roms for each game
    gamesLibrary[games_init_index].title = "Blazing Lazers";
    gamesLibrary[games_init_index].coverPath = "assets/images/pc_ind/blaz_laz.png";
    gamesLibrary[games_init_index].console = "TurboGrafx-16";
    gamesLibrary[games_init_index].romPath = "assets/roms/pc/BlazingLazers.pce";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Doom 2";
    gamesLibrary[games_init_index].coverPath = "assets/images/pc_ind/doom_2.png";
    gamesLibrary[games_init_index].console = "PC";
    gamesLibrary[games_init_index].romPath = "assets/roms/pc/DOOM2.WAD";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Doom";
    gamesLibrary[games_init_index].coverPath = "assets/images/pc_ind/doom.png";
    gamesLibrary[games_init_index].console = "PC";
    gamesLibrary[games_init_index].romPath = "assets/roms/pc/DOOM.WAD";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Ys Book I & II";
    gamesLibrary[games_init_index].coverPath = "assets/images/pc_ind/ys.png";
    gamesLibrary[games_init_index].console = "TurboGrafx-CD";
    gamesLibrary[games_init_index].romPath = "assets/roms/pc/YsBook.chd";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Lords of Thunder";
    gamesLibrary[games_init_index].coverPath = "assets/images/pc_ind/lord_thund.png";
    gamesLibrary[games_init_index].console = "TurboGrafx-CD";
    gamesLibrary[games_init_index].romPath = "assets/roms/pc/LordofThund.chd";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Air Zonk";
    gamesLibrary[games_init_index].coverPath = "assets/images/pc_ind/air_zonk.png";
    gamesLibrary[games_init_index].console = "TurboGrafx-16";
    gamesLibrary[games_init_index].romPath = "assets/roms/pc/AirZonk.pce";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Dragon Slayer - The Legend of Heroes";
    gamesLibrary[games_init_index].coverPath = "assets/images/pc_ind/drag_slay.png";
    gamesLibrary[games_init_index].console = "TurboGrafx-CD";
    gamesLibrary[games_init_index].romPath = "assets/roms/pc/DragSlay.chd";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Splatterhouse";
    gamesLibrary[games_init_index].coverPath = "assets/images/pc_ind/splatterhouse.png";
    gamesLibrary[games_init_index].console = "TurboGrafx-16";
    gamesLibrary[games_init_index].romPath = "assets/roms/trbogrfx/Splatterhouse.pce";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Cosmic Fantasy 2";
    gamesLibrary[games_init_index].coverPath = "assets/images/pc_ind/cos_fant_2.png";
    gamesLibrary[games_init_index].console = "TurboGrafx-CD";
    gamesLibrary[games_init_index].romPath = "assets/roms/trbogrfx/CosmicFantasy2.chd";
    games_init_index += 1;
    //All have same category and save type and all are libretro. Path depends on console
    for (int i = temp_start_section; i < games_init_index; i++) {
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
    temp_start_section = games_init_index;
    //Titles, images, consoles, roms, and (if applys) serials for each game
    gamesLibrary[games_init_index].title = "Crazy Taxi";
    gamesLibrary[games_init_index].coverPath = "assets/images/sega/crazy_taxi.png";
    gamesLibrary[games_init_index].console = "Sega Dreamcast";
    gamesLibrary[games_init_index].romPath = "assets/roms/dreamcast/CrazyTaxi.chd";
    gamesLibrary[games_init_index].serial = "MK-51035";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Fighting Vipers";
    gamesLibrary[games_init_index].coverPath = "assets/images/sega/fighting_vipers.png";
    gamesLibrary[games_init_index].console = "Sega Saturn";
    gamesLibrary[games_init_index].romPath = "assets/roms/saturn/FightingVipers.chd";
    gamesLibrary[games_init_index].serial = "B1334F8949ED55769416D83D7A19F1C5";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Sonic the Hedgehog 3 & Knuckles";
    gamesLibrary[games_init_index].coverPath = "assets/images/sega/sonic_3_and_knuckles.png";
    gamesLibrary[games_init_index].console = "Sega Genesis";
    gamesLibrary[games_init_index].romPath = "assets/roms/genesis/Sonic3&Knuckles.md";
    games_init_index += 1;  
    gamesLibrary[games_init_index].title = "Sonic CD";
    gamesLibrary[games_init_index].coverPath = "assets/images/sega/sonic_cd.png";
    gamesLibrary[games_init_index].console = "Sega CD";
    gamesLibrary[games_init_index].romPath = "assets/roms/cd/SonicCD.chd";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "NBA Jam: Tournament Edition";
    gamesLibrary[games_init_index].coverPath = "assets/images/sega/nba_jam_te.png";
    gamesLibrary[games_init_index].console = "Sega Saturn";
    gamesLibrary[games_init_index].romPath = "assets/roms/saturn/NBAJam.chd";
    gamesLibrary[games_init_index].serial = "DBA9351F1D9322E87F3D7F511992DEB3";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Daytona USA";
    gamesLibrary[games_init_index].coverPath = "assets/images/sega/daytona_usa.png";
    gamesLibrary[games_init_index].console = "Sega Saturn";
    gamesLibrary[games_init_index].romPath = "assets/roms/saturn/DaytonaUSA.chd";
    gamesLibrary[games_init_index].serial = "C8355C918C5A97B9E4AB9322DDAFDB1E";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Gunstar Heroes";
    gamesLibrary[games_init_index].coverPath = "assets/images/sega/gunstar_heroes.png";
    gamesLibrary[games_init_index].console = "Sega Genesis";
    gamesLibrary[games_init_index].romPath = "assets/roms/genesis/GunstarHeroes.md";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Wonder Boy";
    gamesLibrary[games_init_index].coverPath = "assets/images/sega/wonder_boy.png";
    gamesLibrary[games_init_index].console = "Sega Master System";
    gamesLibrary[games_init_index].romPath = "assets/roms/mastsys/WonderBoy.sms";
    games_init_index += 1;
    //All have same category. Path, save type, and whether it is libretro depends on console
    for (int i = temp_start_section; i < games_init_index; i++) {
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
            gamesLibrary[i].processName = "ymir-sdl3";
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
            gamesLibrary[i].processName = "Flycast-rend";
        }
    }
}

//Initialize Playstation games
static void Games_Playstation_Init() {
    temp_start_section = games_init_index;
    //Titles, images, roms, and serials for each game
    gamesLibrary[games_init_index].title = "Mega Man X4";
    gamesLibrary[games_init_index].coverPath = "assets/images/playstation/megaman_x4.png";
    gamesLibrary[games_init_index].romPath = "assets/roms/ps1/MegaManX4.chd";
    gamesLibrary[games_init_index].serial = "SLUS-00561_1";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Pac-Man World";
    gamesLibrary[games_init_index].coverPath = "assets/images/playstation/pac-man_world.png";
    gamesLibrary[games_init_index].romPath = "assets/roms/ps1/Pac-ManWorld.chd";
    gamesLibrary[games_init_index].serial = "SLUS-00439_1";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Soul Blade";
    gamesLibrary[games_init_index].coverPath = "assets/images/playstation/soulblade.png";
    gamesLibrary[games_init_index].romPath = "assets/roms/ps1/SoulBlade.chd";
    gamesLibrary[games_init_index].serial = "SLUS-00240_1";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Spider-Man (2000)";
    gamesLibrary[games_init_index].coverPath = "assets/images/playstation/spider-man.png";
    gamesLibrary[games_init_index].romPath = "assets/roms/ps1/Spider-Man.chd";
    gamesLibrary[games_init_index].serial = "SLUS-00875_1";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Street Fighter: Alpha 3";
    gamesLibrary[games_init_index].coverPath = "assets/images/playstation/street_fighter_alpha_3.png";
    gamesLibrary[games_init_index].romPath = "assets/roms/ps1/StreetFighterAlpha3.chd";
    gamesLibrary[games_init_index].serial = "SLUS-00821_1";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Metal Gear Solid";
    gamesLibrary[games_init_index].coverPath = "assets/images/playstation/met_gear_solid.png";
    gamesLibrary[games_init_index].romPath = "assets/roms/ps1/MetalGearSolid.m3u";
    gamesLibrary[games_init_index].serial = "SLUS-00594_1";
    games_init_index += 1;
    gamesLibrary[games_init_index].title = "Twisted Metal 2";
    gamesLibrary[games_init_index].coverPath = "assets/images/playstation/twist_met_2.png";
    gamesLibrary[games_init_index].romPath = "assets/roms/ps1/TwistedMetal2.chd";
    gamesLibrary[games_init_index].serial = "SCUS-94306_1";
    games_init_index += 1;
    //All have same category, save type, path, and console and all are libretro
    for (int i = temp_start_section; i < games_init_index; i++) {
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
void Games_ScrollRight(void) {
    games_index = start_index + (games_index - start_index + 1) % games_range;
    Games_Refresh();
}

//Shift the order of the games to the left
void Games_ScrollLeft(void) {
    games_index = start_index + (games_index - start_index - 1 + games_range) % games_range;
    Games_Refresh();
}

//Get one of the loaded images
Image* Games_GetLoadedImage(int i) {
    return &LoadedImages[i];
}

//Get whether an image is loaded
bool Games_GetIsLoaded(int i) {
    return atomic_load_explicit(&isLoaded[i], memory_order_acquire);
}

//Load game cover textures
void* Games_LoadImages(void *args) {
    pthread_detach(pthread_self());
    int s = start_index, e = end_index;
    //Load the images of the current category first
    for (int i = s; i <= e; i++) {
        LoadedImages[i] = LoadImage(gamesLibrary[i].coverPath);
        atomic_store_explicit(&isLoaded[i], true, memory_order_release);
    }
    //Load the images at beginning of array
    for (int i = 0; i < s; i++) {
        LoadedImages[i] = LoadImage(gamesLibrary[i].coverPath);
        atomic_store_explicit(&isLoaded[i], true, memory_order_release);
    }
    //Load then images at end of array
    for (int i = e + 1; i < GAMES_LEN; i++) {
        LoadedImages[i] = LoadImage(gamesLibrary[i].coverPath);
        atomic_store_explicit(&isLoaded[i], true, memory_order_release);
    }
    return NULL;
}

//Unload game cover textures
void Games_UnloadTextures(void) {
    //Unload all images
    for (int i = 0; i < GAMES_LEN; i++) {
        UnloadTexture(gamesLibrary[i].cover);
    }
}