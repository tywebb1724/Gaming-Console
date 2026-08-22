#include "games.h"
#include <pthread.h>
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <stdint.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>


//Arrays to hold all games, the games displayed, and the new games displayed
static game_t gamesLibrary[MAX_GAMES];
static game_t* gamesDisplayed[GAMES_ON_SCREEN + DISPLAY_CENTER_OFFSET];
static game_t* newGamesDisplayed[GAMES_ON_SCREEN];
//Array to hold loaded images during boot up
static Image LoadedImages[MAX_GAMES];
//Indexes and range to keep track of current and new games
static int games_index;
static int start_index;
static int end_index;
static int games_range;
static int start_index_new;

//Variables to keep track of which images and textures are loaded
static atomic_bool isLoaded[MAX_GAMES] = { false };
//Indexes for initializing the games
static int gamesLen = 0;
static int temp_start_section = 0;
static char m3uSkipList[MAX_M3U_SKIP_ENTRIES][256];
static int m3uSkipCount = 0;


int Games_GetLength() {
    return gamesLen;
}

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
    printf("%s\n", game->serial);
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
    char parent_rel_path[SAVE_PATH_LEN];
    snprintf(parent_rel_path, sizeof(parent_rel_path), ".var/app/org.ppsspp.PPSSPP/config/ppsspp/PSP/SAVEDATA");

    char parent_abs_path[SAVE_PATH_LEN];
    if (!Games_BuildHomePath(parent_abs_path, sizeof(parent_abs_path), parent_rel_path)) {
        printf("Could not resolve PSP save path (HOME not set)\n");
        return false;
    }

    DIR* dir = opendir(parent_abs_path);
    if (!dir) {
        printf("Failed to open PSP save directory (or none exists): %s\n", parent_abs_path);
        return false;
    }

    bool cleared_any = false;
    size_t serial_len = strlen(game->serial);
    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        // Skip '.' and '..'
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        // Check if directory name starts with the serial prefix
        if (strncmp(entry->d_name, game->serial, serial_len) == 0) {
            char target_path[SAVE_PATH_LEN];
            snprintf(target_path, sizeof(target_path), "%s/%s", parent_abs_path, entry->d_name);

            char command[COMMAND_STR_LEN];
            snprintf(command, sizeof(command), "rm -rf \"%s\"", target_path);

            if (system(command) == 0) {
                printf("Cleared PSP save data directory: %s\n", entry->d_name);
                cleared_any = true;
            } else {
                printf("Failed to delete PSP save folder: %s\n", entry->d_name);
            }
        }
    }
    closedir(dir);

    if (!cleared_any) {
        printf("No PSP save folders found matching prefix: %s\n", game->serial);
    }
    return cleared_any;
}
        else if (strcmp(game->console, "Sega Saturn") == 0) {
    if (game->serial == NULL || game->serial[0] == '\0') {
        printf("No known save folder for this game yet — nothing to clear\n");
        return false;
    }

    char rel_path[SAVE_PATH_LEN];
    snprintf(rel_path, sizeof(rel_path),
             ".var/app/io.github.strikerx3.ymir/data/StrikerX3/Ymir/savestates/%s", game->serial);

    char save_path[SAVE_PATH_LEN];
    if (Games_BuildHomePath(save_path, sizeof(save_path), rel_path)) {
        //Check whether there's actually anything to delete first
        struct stat st;
        if (stat(save_path, &st) != 0) {
            printf("Save data for %s was already cleared\n", game->serial);
            return false;
        }

        char command[COMMAND_STR_LEN];
        snprintf(command, sizeof(command), "rm -rf \"%s\"", save_path);
        int result = system(command);
        if (result == 0) {
            printf("Cleared Saturn save data for %s\n", game->serial);
            return true;
        } else {
            printf("Failed to clear Saturn save\n");
            return false;
        }
    } else {
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
    char parent_rel[SAVE_PATH_LEN];
    snprintf(parent_rel, sizeof(parent_rel), 
             ".var/app/org.DolphinEmu.dolphin-emu/data/dolphin-emu/GC/USA/Card A");

    char parent_abs[SAVE_PATH_LEN];
    if (!Games_BuildHomePath(parent_abs, sizeof(parent_abs), parent_rel)) {
        printf("Could not resolve GameCube save path (HOME not set)\n");
        return false;
    }

    DIR* dir = opendir(parent_abs);
    if (!dir) {
        printf("Failed to open GameCube Card A directory: %s\n", parent_abs);
        return false;
    }

    bool cleared_any = false;
    size_t serial_len = strlen(game->serial); // Matches "01-GZLE"
    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        // Matches files starting with game->serial (e.g. "01-GZLE")
        if (strncmp(entry->d_name, game->serial, serial_len) == 0) {
            char target_file[SAVE_PATH_LEN];
            snprintf(target_file, sizeof(target_file), "%s/%s", parent_abs, entry->d_name);

            if (remove(target_file) == 0) {
                printf("Cleared GameCube save file: %s\n", entry->d_name);
                cleared_any = true;
            } else {
                printf("Failed to remove GameCube save file: %s\n", entry->d_name);
            }
        }
    }
    closedir(dir);

    if (!cleared_any) {
        printf("No GameCube save files found matching prefix: %s\n", game->serial);
    }
    return cleared_any;
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


/*
//Initialize arcade games
static void Games_Arcade_Init() {
    temp_start_section = gamesLen;
    //Title, cover, and rom for each game
    gamesLibrary[gamesLen].title = "Metal Slug 3";
    gamesLibrary[gamesLen].coverPath = "assets/images/arcade/metal_slug_3.png";
    gamesLibrary[gamesLen].romPath = "assets/roms/arcade/mslug3.zip";
    gamesLen += 1;
    gamesLibrary[gamesLen].title = "Pac-Man";
    gamesLibrary[gamesLen].coverPath = "assets/images/arcade/pac-man.png";
    gamesLibrary[gamesLen].romPath = "assets/roms/arcade/pacman.zip";
    gamesLen += 1;
    gamesLibrary[gamesLen].title = "Simpsons Arcade Game";
    gamesLibrary[gamesLen].coverPath = "assets/images/arcade/simpsons_arcade.png";
    gamesLibrary[gamesLen].romPath = "assets/roms/arcade/simpsons2p.zip";
    gamesLen += 1;
    gamesLibrary[gamesLen].title = "Street Fighter Alpha 3";
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
    gamesLibrary[games_init_index].serial = "ULUS10041";
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
*/


#include <dirent.h>

#define SAVE_MAP_PATH_LEN 300
#define MAX_SNAPSHOT_ENTRIES 256
#define SNAPSHOT_NAME_LEN 256

static char g_saveSnapshot[MAX_SNAPSHOT_ENTRIES][SNAPSHOT_NAME_LEN];
static int g_saveSnapshotCount = 0;

//Takes a snapshot of every entry currently in the given directory.
//Call this immediately BEFORE launching an emulator.
void Games_SnapshotSaveFolder(const char* dirPath) {
    g_saveSnapshotCount = 0;

    DIR* dir = opendir(dirPath);
    if (!dir) {
        printf("DEBUG: Snapshot failed to open dir: %s\n", dirPath);
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL && g_saveSnapshotCount < MAX_SNAPSHOT_ENTRIES) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        snprintf(g_saveSnapshot[g_saveSnapshotCount], SNAPSHOT_NAME_LEN, "%s", entry->d_name);
        g_saveSnapshotCount++;
    }
     printf("DEBUG: Snapshot captured %d entries in %s\n", g_saveSnapshotCount, dirPath);
    closedir(dir);
}


//Detects a newly-created Flycast VMU save file after a session ends, and
//records the game's serial (e.g. "MK-51035") extracted from the filename.
void Games_DetectNewFlycastSaveFile(game_t* game, const char* saveDir) {
    if (game->serial[0] != '\0') return;

    DIR* dir = opendir(saveDir);
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        //Only interested in files matching the "_vmu_save_A1.bin" pattern
        char* suffix = strstr(entry->d_name, "_vmu_save_A1.bin");
        if (!suffix) continue;

        bool wasPresentBefore = false;
        for (int i = 0; i < g_saveSnapshotCount; i++) {
            if (strcmp(g_saveSnapshot[i], entry->d_name) == 0) {
                wasPresentBefore = true;
                break;
            }
        }

        if (!wasPresentBefore) {
            //Extract just the serial portion (everything before the suffix)
            size_t serialLen = (size_t)(suffix - entry->d_name);
            if (serialLen >= sizeof(game->serial)) serialLen = sizeof(game->serial) - 1;
            memcpy(game->serial, entry->d_name, serialLen);
            game->serial[serialLen] = '\0';

            printf("Recorded Flycast save ID '%s' for %s\n", game->serial, game->romPath);

            FILE* f = fopen("assets/txt/flycast_save_map.txt", "a");
            if (f) {
                fprintf(f, "%s|%s\n", game->romPath, game->serial);
                fclose(f);
            }
            break;
        }
    }
    closedir(dir);
}

//Reads the persisted Flycast save-ID map and restores game->serial for any
//matching entries in the already-populated games library.
static void Games_LoadFlycastSaveMap(void) {
    FILE* f = fopen("assets/txt/flycast_save_map.txt", "r");
    if (!f) return;

    char line[600];
    while (fgets(line, sizeof(line), f)) {
        char* sep = strchr(line, '|');
        if (!sep) continue;
        *sep = '\0';
        char* romPath = line;
        char* serial = sep + 1;

        size_t len = strlen(serial);
        if (len > 0 && serial[len - 1] == '\n') serial[len - 1] = '\0';

        for (int i = 0; i < gamesLen; i++) {
            if (strcmp(gamesLibrary[i].romPath, romPath) == 0) {
                snprintf(gamesLibrary[i].serial, sizeof(gamesLibrary[i].serial), "%s", serial);
                break;
            }
        }
    }
    fclose(f);
}


//Detects a newly-created Ymir savestate folder after a session ends, and
//records its name as the game's serial.
void Games_DetectNewYmirSaveFolder(game_t* game, const char* savestatesDir) {
    if (game->serial[0] != '\0') return;

    DIR* dir = opendir(savestatesDir);
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        bool wasPresentBefore = false;
        for (int i = 0; i < g_saveSnapshotCount; i++) {
            if (strcmp(g_saveSnapshot[i], entry->d_name) == 0) {
                wasPresentBefore = true;
                break;
            }
        }

        if (!wasPresentBefore) {
            snprintf(game->serial, sizeof(game->serial), "%s", entry->d_name);
            printf("Recorded Ymir save folder '%s' for %s\n", game->serial, game->romPath);

            FILE* f = fopen("assets/txt/ymir_save_map.txt", "a");
            if (f) {
                fprintf(f, "%s|%s\n", game->romPath, game->serial);
                fclose(f);
            }
            break;
        }
    }
    closedir(dir);
}

//Reads the persisted Ymir save-folder map and restores game->serial for any
//matching entries in the already-populated games library.
static void Games_LoadYmirSaveMap(void) {
    FILE* f = fopen("assets/txt/ymir_save_map.txt", "r");
    if (!f) return;

    char line[600];
    while (fgets(line, sizeof(line), f)) {
        char* sep = strchr(line, '|');
        if (!sep) continue;
        *sep = '\0';
        char* romPath = line;
        char* folderName = sep + 1;

        size_t len = strlen(folderName);
        if (len > 0 && folderName[len - 1] == '\n') folderName[len - 1] = '\0';

        for (int i = 0; i < gamesLen; i++) {
            if (strcmp(gamesLibrary[i].romPath, romPath) == 0) {
                snprintf(gamesLibrary[i].serial, sizeof(gamesLibrary[i].serial), "%s", folderName);
                break;
            }
        }
    }
    fclose(f);
}

//Detects a newly-created PS1 .mcd save file after a session ends, and
//records its base name (e.g. "SCUS-94306_1") as the game's serial.
void Games_DetectNewPS1SaveFile(game_t* game, const char* savesDir) {
     printf("DEBUG: Games_DetectNewPS1SaveFile called, serial='%s', dir='%s'\n", game->serial, savesDir);
    if (game->serial[0] != '\0') return;

    DIR* dir = opendir(savesDir);
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        if (!IsFileExtension(entry->d_name, ".mcd")) continue;

        bool wasPresentBefore = false;
        for (int i = 0; i < g_saveSnapshotCount; i++) {
            if (strcmp(g_saveSnapshot[i], entry->d_name) == 0) {
                wasPresentBefore = true;
                break;
            }
        }

        if (!wasPresentBefore) {
            snprintf(game->serial, sizeof(game->serial), "%s", GetFileNameWithoutExt(entry->d_name));
            printf("Recorded PS1 save ID '%s' for %s\n", game->serial, game->romPath);

            FILE* f = fopen("assets/txt/ps1_save_map.txt", "a");
            if (f) {
                fprintf(f, "%s|%s\n", game->romPath, game->serial);
                fclose(f);
            }
            break;
        }
    }
    closedir(dir);
}

//Reads the persisted PS1 save-ID map and restores game->serial for any
//matching entries in the already-populated games library.
static void Games_LoadPS1SaveMap(void) {
    FILE* f = fopen("assets/txt/ps1_save_map.txt", "r");
    if (!f) return;

    char line[600];
    while (fgets(line, sizeof(line), f)) {
        char* sep = strchr(line, '|');
        if (!sep) continue;
        *sep = '\0';
        char* romPath = line;
        char* serial = sep + 1;

        size_t len = strlen(serial);
        if (len > 0 && serial[len - 1] == '\n') serial[len - 1] = '\0';

        for (int i = 0; i < gamesLen; i++) {
            if (strcmp(gamesLibrary[i].romPath, romPath) == 0) {
                snprintf(gamesLibrary[i].serial, sizeof(gamesLibrary[i].serial), "%s", serial);
                break;
            }
        }
    }
    fclose(f);
}



// GameCube Save ID Extraction (Handles both Raw ISOs and CISOs)
// Output Format: 01-GALE
bool GetGameCubeSaveID(const char* filePath, char* outSaveID, size_t maxLen) {
    FILE* f = fopen(filePath, "rb");
    if (!f) return false;

    // 1. Check for CISO Header Signature
    char magic[4] = {0};
    long headerOffset = 0x00;

    if (fread(magic, 1, 4, f) == 4) {
        if (memcmp(magic, "CISO", 4) == 0) {
            // CISO format shifts the GameCube ISO header to 0x8000
            headerOffset = 0x8000;
        }
    }

    // 2. Seek to the actual GameCube header location
    if (fseek(f, headerOffset, SEEK_SET) != 0) {
        fclose(f);
        return false;
    }

    char gameCode[5] = {0};
    char makerCode[3] = {0};

    // Read 4-byte Game Code (e.g. GALE)
    if (fread(gameCode, 1, 4, f) == 4) {
        // Read 2-byte Maker Code at +0x04 (e.g. 01)
        if (fread(makerCode, 1, 2, f) == 2) {
            snprintf(outSaveID, maxLen, "%s-%s", makerCode, gameCode); // Outputs "01-GALE"
            fclose(f);
            return true;
        }
    }

    fclose(f);
    return false;
}




// 5. PlayStation Portable (Disc ID @ Offset 0x8373)
// Example Output: ULUS10041
// ============================================================================
bool GetPSPSaveID(const char* filePath, char* outSaveID, size_t maxLen) {
    FILE* f = fopen(filePath, "rb");
    if (!f) return false;

    if (fseek(f, 0x8373, SEEK_SET) == 0) {
        char raw[11] = {0};
        if (fread(raw, 1, 10, f) == 10) {
            // Strip hyphens if present to form clean standard directory ID
            char clean[11] = {0};
            int cIdx = 0;
            for (int i = 0; i < 10; i++) {
                if (isalnum((unsigned char)raw[i])) {
                    clean[cIdx++] = raw[i];
                }
            }
            if (cIdx >= 9) {
                snprintf(outSaveID, maxLen, "%s", clean);
                fclose(f);
                return true;
            }
        }
    }
    fclose(f);
    return false;
}


static void Games_SetTypes(save_t saveType, bool libretro, char* category, char* path, char* process) {
    for (int i = temp_start_section; i < gamesLen; i++) {
        gamesLibrary[i].save = saveType;
        gamesLibrary[i].libRetro = libretro;
        gamesLibrary[i].category = category;
        gamesLibrary[i].corePath = path;
        if (!libretro) {
            gamesLibrary[i].processName = process;
        }
    }
}

//Reads a title string at a given offset, trims trailing whitespace.
//Returns true if a plausible title was read, false otherwise.
static bool Games_TryReadTitleAt(FILE* f, long offset, size_t rawLen, char* outTitle, size_t outSize) {
    if (fseek(f, offset, SEEK_SET) != 0) return false;

    char* rawTitle = malloc(rawLen);
    if (!rawTitle) return false;

    bool success = false;
    if (fread(rawTitle, 1, rawLen - 1, f) == rawLen - 1) {
        

        rawTitle[rawLen - 1] = '\0';

        //Trim trailing whitespace
        size_t len = strlen(rawTitle);
        while (len > 0 && isspace((unsigned char)rawTitle[len - 1])) {
            rawTitle[--len] = '\0';
        }

        //Only accept it if something non-empty remains
        if (len > 0) {
            snprintf(outTitle, outSize, "%s", rawTitle);
            success = true;
        }
    }
    

    free(rawTitle);
    return success;
}

//For consoles with one known, reliable header offset
static void Games_ExtractTitleAtOffset(const char* romPath, game_t* game, long offset, size_t rawLen) {
    FILE* f = fopen(romPath, "rb");
    if (!f) return;
    bool success = Games_TryReadTitleAt(f, offset, rawLen, game->title, TITLE_MAX_LEN);
    fclose(f);

    if (!success) {
        snprintf(game->title, TITLE_MAX_LEN, "%s", GetFileNameWithoutExt(romPath));
    }
}

#define CISO_HEADER_SIZE 0x8000  // magic(4) + block_size(4) + bitmap(0x7FF8, i.e. up to 32752 blocks)

void Games_GetGCTitle(const char* romPath, game_t* game) {
    FILE* f = fopen(romPath, "rb");
    if (!f) return;

    unsigned char magic[4];
    bool success = false;

    if (fread(magic, 1, 4, f) == 4 && memcmp(magic, "CISO", 4) == 0) {
        //It's a CISO file — block 0 (containing disc offset 0x20) sits right after
        //the fixed-size header+bitmap block, as long as block 0 is marked present.
        unsigned char bitmapByte;
        fseek(f, 8, SEEK_SET);   //bitmap starts at byte 8
        if (fread(&bitmapByte, 1, 1, f) == 1 && (bitmapByte & 0x01)) {
            success = Games_TryReadTitleAt(f, CISO_HEADER_SIZE + GC_OFFSET, GC_RAW_LEN, game->title, TITLE_MAX_LEN);
        }
    }
    else {
        //Plain, uncompressed ISO — read directly at the normal offset
        success = Games_TryReadTitleAt(f, GC_OFFSET, GC_RAW_LEN, game->title, TITLE_MAX_LEN);
    }

    fclose(f);

    if (!success) {
        snprintf(game->title, TITLE_MAX_LEN, "%s", GetFileNameWithoutExt(romPath));
    }
}

void Games_GetSNESTitle(const char* romPath, game_t* game) {
    FILE* f = fopen(romPath, "rb");
    if (!f) return;

    //Determine file size to detect a 512-byte copier header
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        snprintf(game->title, TITLE_MAX_LEN, "%s", GetFileNameWithoutExt(romPath));
        return;
    }
    long fileSize = ftell(f);
    long headerOffset = ((fileSize % 1024) == SNES_COPIER_HEADER_SIZE) ? SNES_COPIER_HEADER_SIZE : 0;

    //Try LoROM location first
    bool success = Games_TryReadTitleAt(f, headerOffset + SNES_LOROM_OFFSET, SNES_RAW_LEN, game->title, TITLE_MAX_LEN);

    //If that didn't work, try HiROM location
    if (!success) {
        success = Games_TryReadTitleAt(f, headerOffset + SNES_HIROM_OFFSET, SNES_RAW_LEN, game->title, TITLE_MAX_LEN);
    }

    fclose(f);

    if (!success) {
        snprintf(game->title, TITLE_MAX_LEN, "%s", GetFileNameWithoutExt(romPath));
    }
}

//Checks whether a given base filename is already covered by a processed M3U playlist
static bool Games_IsCoveredByM3U(const char* filename) {
    for (int i = 0; i < m3uSkipCount; i++) {
        if (strcmp(m3uSkipList[i], filename) == 0) return true;
    }
    return false;
}

static void Games_ScanM3UPlaylists(const char* dirPath, const char* console) {
    temp_start_section = gamesLen;
    FilePathList files = LoadDirectoryFiles(dirPath);
    for (int i = 0; i < files.count; i++) {
        if (!IsFileExtension(files.paths[i], ".m3u")) continue;

        FILE* f = fopen(files.paths[i], "r");
        if (!f) continue;

        char line[M3U_LINE_LEN];
        bool addedEntry = false;
        
        while (fgets(line, sizeof(line), f)) {
            size_t len = strlen(line);
            while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
                line[--len] = '\0';
            }
            if (len == 0 || line[0] == '#') continue;   //skip blank lines/comments

            //Record just the base filename so it matches later, regardless of path style
            const char* base = GetFileName(line);
            if (m3uSkipCount < MAX_M3U_SKIP_ENTRIES) {
                snprintf(m3uSkipList[m3uSkipCount], sizeof(m3uSkipList[m3uSkipCount]), "%s", base);
                m3uSkipCount++;
            }

            //Launch the M3U itself — the core handles which disc loads first/swapping
            if (!addedEntry) {
                snprintf(gamesLibrary[gamesLen].romPath, sizeof(gamesLibrary[gamesLen].romPath),
                         "%s", files.paths[i]);
                snprintf(gamesLibrary[gamesLen].console, sizeof(gamesLibrary[gamesLen].console),
                         "%s", console);
                snprintf(gamesLibrary[gamesLen].title, TITLE_MAX_LEN, "%s",
                         GetFileNameWithoutExt(files.paths[i]));

                char coverPath[256];
                snprintf(coverPath, sizeof(coverPath), "assets/images/%s/%s.png", console, GetFileNameWithoutExt(files.paths[i]));
                if (FileExists(coverPath)) {
                    snprintf(gamesLibrary[gamesLen].coverPath, sizeof(gamesLibrary[gamesLen].coverPath), "%s", coverPath);
                }
                gamesLen += 1;
                addedEntry = true;
            }
        }
        fclose(f);
    }
    UnloadDirectoryFiles(files);
    Games_SetTypes(EXTERNAL, true, "PlayStation", PATH_PS1, NULL);
}

//Reads a 32-bit little-endian value from a buffer at a given offset
static uint32_t ReadLE32(const unsigned char* buf, size_t offset) {
    return (uint32_t)buf[offset] |
           ((uint32_t)buf[offset + 1] << 8) |
           ((uint32_t)buf[offset + 2] << 16) |
           ((uint32_t)buf[offset + 3] << 24);
}

//Reads a 16-bit little-endian value from a buffer at a given offset
static uint16_t ReadLE16(const unsigned char* buf, size_t offset) {
    return (uint16_t)buf[offset] | ((uint16_t)buf[offset + 1] << 8);
}

//Parses a PARAM.SFO buffer already loaded into memory, extracting the TITLE value
static bool ParseSFOTitle(const unsigned char* sfoData, size_t sfoSize, char* outTitle, size_t outSize) {
    if (sfoSize < 20) return false;

    //SFO header: magic(4) + version(4) + key_table_offset(4) + data_table_offset(4) + entry_count(4)
    uint32_t magic = ReadLE32(sfoData, 0);
    if (magic != 0x00465350) return false;  // "\0PSF" as stored little-endian

    uint32_t keyTableOffset = ReadLE32(sfoData, 8);
    uint32_t dataTableOffset = ReadLE32(sfoData, 12);
    uint32_t entryCount = ReadLE32(sfoData, 16);

    //Each index table entry is 16 bytes: key_offset(2) + data_fmt(2) + data_len(4) + data_max_len(4) + data_offset(4)
    for (uint32_t i = 0; i < entryCount; i++) {
        size_t entryOffset = 20 + (i * 16);
        if (entryOffset + 16 > sfoSize) break;

        uint16_t keyOffset = ReadLE16(sfoData, entryOffset);
        uint32_t dataLen = ReadLE32(sfoData, entryOffset + 4);
        uint32_t dataOffset = ReadLE32(sfoData, entryOffset + 12);

        size_t keyPos = keyTableOffset + keyOffset;
        if (keyPos >= sfoSize) continue;

        //Check if this key is "TITLE" (not "GAME_TITLE" or others we don't want)
        if (strcmp((const char*)(sfoData + keyPos), "TITLE") == 0) {
            size_t dataPos = dataTableOffset + dataOffset;
            if (dataPos + dataLen > sfoSize) return false;

            size_t copyLen = (dataLen < outSize) ? dataLen : outSize - 1;
            memcpy(outTitle, sfoData + dataPos, copyLen);
            outTitle[copyLen] = '\0';

            //Trim trailing whitespace/nulls
            size_t len = strlen(outTitle);
            while (len > 0 && isspace((unsigned char)outTitle[len - 1])) {
                outTitle[--len] = '\0';
            }
            return (len > 0);
        }
    }
    return false;
}

//Extracts the title from a PSP ISO by locating and parsing PSP_GAME/PARAM.SFO
static void Games_GetPSPTitle(const char* romPath, game_t* game) {
    FILE* f = fopen(romPath, "rb");
    if (!f) return;

    unsigned char pvd[ISO_SECTOR_SIZE];
    bool foundSFO = false;

    //Read the Primary Volume Descriptor to find the root directory
    if (fseek(f, ISO_PVD_OFFSET, SEEK_SET) == 0 &&
        fread(pvd, 1, ISO_SECTOR_SIZE, f) == ISO_SECTOR_SIZE) {

        //Root directory record starts at byte 156 within the PVD, is 34 bytes
        uint32_t rootDirLBA = ReadLE32(pvd, 156 + 2);
        uint32_t rootDirSize = ReadLE32(pvd, 156 + 10);

        //Read the root directory's sector(s) to find "PSP_GAME"
        unsigned char* rootDir = malloc(rootDirSize);
        if (rootDir && fseek(f, (long)rootDirLBA * ISO_SECTOR_SIZE, SEEK_SET) == 0 &&
            fread(rootDir, 1, rootDirSize, f) == rootDirSize) {

            uint32_t pspGameLBA = 0;
            size_t pos = 0;
            while (pos < rootDirSize) {
                unsigned char recordLen = rootDir[pos];
                if (recordLen == 0) { pos++; continue; }
                unsigned char nameLen = rootDir[pos + 32];
                char name[64] = {0};
                size_t copyLen = (nameLen < 63) ? nameLen : 63;
                if (rootDir + pos <= rootDirSize) memcpy(name, rootDir + pos + 33, copyLen);

                if (strcmp(name, "PSP_GAME") == 0) {
                    pspGameLBA = ReadLE32(rootDir, pos + 2);
                    break;
                }
                pos += recordLen;
            }

            //If found, read PSP_GAME's directory to find PARAM.SFO
            if (pspGameLBA != 0) {
                unsigned char dirBuf[ISO_SECTOR_SIZE];
                if (fseek(f, (long)pspGameLBA * ISO_SECTOR_SIZE, SEEK_SET) == 0 &&
                    fread(dirBuf, 1, ISO_SECTOR_SIZE, f) == ISO_SECTOR_SIZE) {

                    uint32_t sfoLBA = 0, sfoSize = 0;
                    size_t dpos = 0;
                    while (dpos < ISO_SECTOR_SIZE) {
                        unsigned char recordLen = dirBuf[dpos];
                        if (recordLen == 0) { dpos++; continue; }
                        unsigned char nameLen = dirBuf[dpos + 32];
                        char name[64] = {0};
                        size_t copyLen = (nameLen < 63) ? nameLen : 63;
                        memcpy(name, dirBuf + dpos + 33, copyLen);

                        //ISO9660 often appends ";1" (version number) to filenames
                        char* semicolon = strchr(name, ';');
                        if (semicolon) *semicolon = '\0';

                        if (strcmp(name, "PARAM.SFO") == 0) {
                            sfoLBA = ReadLE32(dirBuf, dpos + 2);
                            sfoSize = ReadLE32(dirBuf, dpos + 10);
                            break;
                        }
                        dpos += recordLen;
                    }

                    //Read and parse PARAM.SFO itself
                    if (sfoLBA != 0 && sfoSize > 0 && sfoSize < 65536) {
                        unsigned char* sfoData = malloc(sfoSize);
                        if (sfoData && fseek(f, (long)sfoLBA * ISO_SECTOR_SIZE, SEEK_SET) == 0 &&
                            fread(sfoData, 1, sfoSize, f) == sfoSize) {
                            foundSFO = ParseSFOTitle(sfoData, sfoSize, game->title, TITLE_MAX_LEN);
                        }
                        free(sfoData);
                    }
                }
            }
        }
        free(rootDir);
    }

    fclose(f);

    if (!foundSFO) {
        snprintf(game->title, TITLE_MAX_LEN, "%s", GetFileNameWithoutExt(romPath));
    }
}

static void Games_GetRoms(char* romExtension, char* dirPath, char* console, save_t saveType, bool libretro, char* category, char* path, char* process) {
    temp_start_section = gamesLen;
    FilePathList files = LoadDirectoryFiles(dirPath);
    for (int i = 0; i < files.count; i++) {
        if (IsFileExtension(files.paths[i], romExtension)) {
            if (Games_IsCoveredByM3U(GetFileName(files.paths[i]))) {
                continue;
            }
            snprintf(gamesLibrary[gamesLen].romPath, sizeof(gamesLibrary[gamesLen].romPath), "%s", files.paths[i]);
            snprintf(gamesLibrary[gamesLen].console, sizeof(gamesLibrary[gamesLen].console), "%s", console);
            if (strcmp(console, "Nintendo Entertainment System") == 0 || strcmp(console, "Sony PlayStation") == 0 || 
                strcmp(console, "PC") == 0 || strcmp(console, "Arcade") == 0 || strcmp(console, "Sega Master System") == 0 ||
                strcmp(console, "Sega Game Gear") == 0 || strcmp(console, "Sega Saturn") == 0 || strcmp(console, "Sega CD") == 0 ||
                strcmp(console, "Sega Dreamcast") == 0 || strcmp(console, "TurboGrafx-16") == 0 || strcmp(console, "TurboGrafx-CD") == 0 ||
                strcmp(console, "Game Boy") == 0 || strcmp(console, "Game Boy Color") == 0 || strcmp(console, "Game Boy Advance") == 0 ||
                strcmp(console, "Atari Lynx") == 0) {
                snprintf(gamesLibrary[gamesLen].title, TITLE_MAX_LEN, "%s", GetFileNameWithoutExt(files.paths[i]));
            }
            else if (strcmp(console, "Super Nintendo Entertainment System") == 0) {
                Games_GetSNESTitle(files.paths[i], &gamesLibrary[gamesLen]);
            }
            else if (strcmp(console, "Sony PlayStation Portable") == 0) {
                Games_GetPSPTitle(files.paths[i], &gamesLibrary[gamesLen]);
            }
            else if (strcmp(console, "Neo Geo Pocket Color") == 0) {
                Games_ExtractTitleAtOffset(files.paths[i], &gamesLibrary[gamesLen], NGPC_OFFSET, NGPC_RAW_LEN);
            }
            else if (strcmp(console, "Nintendo 64") == 0) {
                Games_ExtractTitleAtOffset(files.paths[i], &gamesLibrary[gamesLen], N64_OFFSET, N64_RAW_LEN);
            }
            else if (strcmp(console, "Nintendo DS") == 0) {
                Games_ExtractTitleAtOffset(files.paths[i], &gamesLibrary[gamesLen], DS_OFFSET, DS_RAW_LEN);
            }
            else if (strcmp(console, "Nintendo GameCube") == 0) {
                Games_GetGCTitle(files.paths[i], &gamesLibrary[gamesLen]);
            }
            else if (strcmp(console, "Sega Genesis") == 0) {
                Games_ExtractTitleAtOffset(files.paths[i], &gamesLibrary[gamesLen], GENESIS_OFFSET, GENESIS_RAW_LEN);
            }
            //Check if local cover image exists
            char coverPath[256];
            snprintf(coverPath, sizeof(coverPath), "assets/images/%s/%s.png", console, GetFileNameWithoutExt(files.paths[i]));
            if (FileExists(coverPath)) {
                snprintf(gamesLibrary[gamesLen].coverPath, sizeof(gamesLibrary[gamesLen].coverPath), "%s", coverPath);
            }
            gamesLen += 1;
        }
    }
    UnloadDirectoryFiles(files);
    Games_SetTypes(saveType, libretro, category, path, process);
}


//Initialize game library
void Games_Init() {
    gamesLen = 0;
    //Arcade games
    Games_GetRoms("zip", "assets/roms/Arcade", "Arcade", NONE, true, "Arcade", PATH_ARCADE, NULL);
    //Handheld games
    Games_GetRoms("nds", "assets/roms/Nintendo DS", "Nintendo DS", EXTERNAL, false, "Handheld Classics", PATH_DS, PROCESS_MELON);
    Games_GetRoms("gg", "assets/roms/Sega Game Gear", "Sega Game Gear", EXTERNAL, true, "Handheld Classics", PATH_GENESIS, NULL);
    Games_GetRoms("gba", "assets/roms/Game Boy Advance", "Game Boy Advance", BATTERY, true, "Handheld Classics", PATH_GBA, NULL);
    Games_GetRoms("gbc", "assets/roms/Game Boy Color", "Game Boy Color", BATTERY, true, "Handheld Classics", PATH_GAMEBOY, NULL);
    Games_GetRoms("gb", "assets/roms/Game Boy", "Game Boy", BATTERY, true, "Handheld Classics", PATH_GAMEBOY, NULL);
    Games_GetRoms("ngc", "assets/roms/Neo Geo Pocket Color", "Neo Geo Pocket Color", NONE, true, "Handheld Classics", PATH_NGPC, NULL);
    Games_GetRoms("iso", "assets/roms/Sony PlayStation Portable", "Sony PlayStation Portable", EXTERNAL, false, "Handheld Classics", PATH_PSP, PROCESS_PPSSPP);
    for (int i = temp_start_section; i < gamesLen; i++) {
        GetPSPSaveID(gamesLibrary[i].romPath, gamesLibrary[i].serial, sizeof(gamesLibrary[i].serial));
    }
    Games_GetRoms("lyx", "assets/roms/Atari Lynx", "Atari Lynx", NONE, true, "Handheld Classics", PATH_LYNX, NULL);
    //Nintendo 3D games
    Games_GetRoms("ciso", "assets/roms/Nintendo GameCube", "Nintendo GameCube", EXTERNAL, false, "Nintendo 3D", PATH_GAMECUBE, PROCESS_DOLPHIN);
    for (int i = temp_start_section; i < gamesLen; i++) {
        GetGameCubeSaveID(gamesLibrary[i].romPath, gamesLibrary[i].serial, sizeof(gamesLibrary[i].serial));
    }
    Games_GetRoms("z64", "assets/roms/Nintendo 64", "Nintendo 64", BATTERY, true, "Nintendo 3D", PATH_N64, NULL);
    //Nintendo retro games
    Games_GetRoms("nes", "assets/roms/Nintendo Entertainment System", "Nintendo Entertainment System", BATTERY, true, "Retro Nintendo", PATH_NES, NULL);
    Games_GetRoms("sfc", "assets/roms/Super Nintendo Entertainment System", "Super Nintendo Entertainment System", BATTERY, true, "Retro Nintendo", PATH_SNES, NULL);
    Games_GetRoms("smc", "assets/roms/Super Nintendo Entertainment System", "Super Nintendo Entertainment System", BATTERY, true, "Retro Nintendo", PATH_SNES, NULL);
    //PC/Indie/Other games
    Games_GetRoms("pce", "assets/roms/TurboGrafx-16", "TurboGrafx-16", BATTERY, true, "TurboGrafx/PC/Other", PATH_TG16, NULL);
    Games_GetRoms("chd", "assets/roms/TurboGrafx-CD", "TurboGrafx-CD", BATTERY, true, "TurboGrafx/PC/Other", PATH_TG16, NULL);
    Games_GetRoms("WAD", "assets/roms/PC", "PC", BATTERY, true, "TurboGrafx/PC/Other", PATH_PRBOOM, NULL);
    //Sega games
    Games_GetRoms("chd", "assets/roms/Sega CD", "Sega CD", EXTERNAL, true, "Sega", PATH_GENESIS, NULL);
    Games_GetRoms("chd", "assets/roms/Sega Dreamcast", "Sega Dreamcast", EXTERNAL, false, "Sega", PATH_DREAMCAST, PROCESS_FLYCAST);
    Games_LoadFlycastSaveMap();
    Games_GetRoms("md", "assets/roms/Sega Genesis", "Sega Genesis", BATTERY, true, "Sega", PATH_GENESIS, NULL);
    Games_GetRoms("sms", "assets/roms/Sega Master System", "Sega Master System", NONE, true, "Sega", PATH_GENESIS, NULL);
    Games_GetRoms("chd", "assets/roms/Sega Saturn", "Sega Saturn", EXTERNAL, false, "Sega", PATH_SATURN, PROCESS_YMIR);
    Games_LoadYmirSaveMap();
    //PS1 games
    Games_ScanM3UPlaylists("assets/roms/Sony PlayStation", "Sony PlayStation");
    for (int i = temp_start_section; i < gamesLen; i++) {
        GetPSPSaveID(gamesLibrary[i].romPath, gamesLibrary[i].serial, sizeof(gamesLibrary[i].serial));
    }
    Games_GetRoms("chd", "assets/roms/Sony PlayStation", "Sony PlayStation", EXTERNAL, true, "PlayStation", PATH_PS1, NULL);
    Games_LoadPS1SaveMap();
    printf("GAMESSSSSSSSSSSSSSSSSSSSS\n");
    

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