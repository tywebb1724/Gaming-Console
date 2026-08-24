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
//Skip list for chd that correspond to m3u files
static char m3uSkipList[MAX_M3U_SKIP_ENTRIES][M3U_MAX_FILE_LEN];

//Variables to keep track of which images and textures are loaded
static atomic_bool isLoaded[MAX_GAMES] = { false };
//Indexes for initializing the games
static int gamesLen = 0;
static int temp_start_section = 0;
static int temp_start_categ = 0;
//Skip count for chd files that correspond to m3u files
static int m3uSkipCount = 0;
//Variables for checking save files in order to identify save IDs
static char g_saveSnapshot[MAX_SNAPSHOT_ENTRIES][SNAPSHOT_NAME_LEN];
static int g_saveSnapshotCount = 0;


//Get length of games array
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
            //If you find the start of the new category, update the start index
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

//Remove a save file and log result
static bool Games_RemoveFileLogged(const char* path) {
    //Remove file and check if was successful
    if (remove(path) == 0) {
        printf("Cleared save data: %s\n", path);
        return true;
    }
    printf("No save data to clear (or delete failed): %s\n", path);
    return false;
}

//Scans path and removes all entries with the specific prefix
static bool Games_RemoveEntriesWithPrefix(const char* parentPath, const char* prefix, bool asDirectory) {
    DIR* dir = opendir(parentPath);
    //Check for error opening directory
    if (!dir) {
        printf("Failed to open save directory (or none exists): %s\n", parentPath);
        return false;
    }
    bool clearedAny = false;
    size_t prefixLen = strlen(prefix);
    struct dirent* entry;
    //Read the entries
    while ((entry = readdir(dir)) != NULL) {
        //Go to next entry
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        //If entry doesn't have correct prefix, go to next entry
        if (strncmp(entry->d_name, prefix, prefixLen) != 0) continue;
        char targetPath[SAVE_PATH_LEN];
        snprintf(targetPath, sizeof(targetPath), "%s/%s", parentPath, entry->d_name);
        //If it is a directory
        if (asDirectory) {
            char command[COMMAND_STR_LEN];
            snprintf(command, sizeof(command), "rm -rf \"%s\"", targetPath);
            //Run command and check if it worked
            if (system(command) == 0) {
                printf("Cleared save data directory: %s\n", entry->d_name);
                clearedAny = true;
            } 
            else {
                printf("Failed to delete save folder: %s\n", entry->d_name);
            }
        } 
        else {
            //Remove file and check if it worked
            if (remove(targetPath) == 0) {
                printf("Cleared save file: %s\n", entry->d_name);
                clearedAny = true;
            } else {
                printf("Failed to remove save file: %s\n", entry->d_name);
            }
        }
    }
    closedir(dir);
    //Check if anything was cleared
    if (!clearedAny) {
        printf("No save entries found matching prefix: %s\n", prefix);
    }
    return clearedAny;
}

//Clear game data
bool Games_ClearData(const game_t* game) {
    printf("%s\n", game->serial);
    //If game doesn't save
    if (game->save == NONE) {
        return false;
    }
    //If saves through battery method
    if (game->save == BATTERY) {
        char save_path[SAVE_PATH_LEN];
        snprintf(save_path, sizeof(save_path), "%s.srm", game->romPath);
        return Games_RemoveFileLogged(save_path);
    }
    //Check which console and remove save accordingly
    if (strcmp(game->console, "Sony PlayStation") == 0) {
        char save_path[SAVE_PATH_LEN];
        snprintf(save_path, sizeof(save_path), "assets/saves/%s.mcd", game->serial);
        return Games_RemoveFileLogged(save_path);
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
        return Games_RemoveFileLogged(save_path);
    }
    else if (strcmp(game->console, "Sony PlayStation Portable") == 0) {
        char parent_rel_path[SAVE_PATH_LEN];
        snprintf(parent_rel_path, sizeof(parent_rel_path),
                 ".var/app/org.ppsspp.PPSSPP/config/ppsspp/PSP/SAVEDATA");
        char parent_abs_path[SAVE_PATH_LEN];
        //Build home path and check for error
        if (!Games_BuildHomePath(parent_abs_path, sizeof(parent_abs_path), parent_rel_path)) {
            printf("Could not resolve PSP save path (HOME not set)\n");
            return false;
        }
        return Games_RemoveEntriesWithPrefix(parent_abs_path, game->serial, /*asDirectory=*/true);
    }
    else if (strcmp(game->console, "Sega Saturn") == 0) {
        //Check to see if valid serial is available
        if (game->serial[0] == '\0') {
            printf("No known save folder for this game yet — nothing to clear\n");
            return false;
        }
        char rel_path[SAVE_PATH_LEN];
        snprintf(rel_path, sizeof(rel_path),
                 ".var/app/io.github.strikerx3.ymir/data/StrikerX3/Ymir/savestates/%s", game->serial);
        char save_path[SAVE_PATH_LEN];
        //Build home path and check for error
        if (!Games_BuildHomePath(save_path, sizeof(save_path), rel_path)) {
            printf("Could not resolve save path (HOME not set)\n");
            return false;
        }
        struct stat st;
        //Check whether there's actually anything to delete first
        if (stat(save_path, &st) != 0) {
            printf("Save data for %s was already cleared\n", game->serial);
            return false;
        }
        char command[COMMAND_STR_LEN];
        snprintf(command, sizeof(command), "rm -rf \"%s\"", save_path);
        //Run command to delete save and check for error
        if (system(command) == 0) {
            printf("Cleared Saturn save data for %s\n", game->serial);
            return true;
        }
        printf("Failed to clear Saturn save\n");
        return false;
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
        return Games_RemoveFileLogged(save_path);
    }
    else if (strcmp(game->console, "Nintendo GameCube") == 0) {
        char parent_rel[SAVE_PATH_LEN];
        snprintf(parent_rel, sizeof(parent_rel),
                 ".var/app/org.DolphinEmu.dolphin-emu/data/dolphin-emu/GC/USA/Card A");
        char parent_abs[SAVE_PATH_LEN];
        //Build home path and check for error
        if (!Games_BuildHomePath(parent_abs, sizeof(parent_abs), parent_rel)) {
            printf("Could not resolve GameCube save path (HOME not set)\n");
            return false;
        }
        return Games_RemoveEntriesWithPrefix(parent_abs, game->serial, false);
    }
    else if (strcmp(game->console, "Sega Dreamcast") == 0) {
        char rel_path[SAVE_PATH_LEN];
        snprintf(rel_path, sizeof(rel_path), ".var/app/org.flycast.Flycast/data/flycast/%s_vmu_save_A1.bin", game->serial);
        char save_path[SAVE_PATH_LEN];
        //Build home path and check for error
        if (!Games_BuildHomePath(save_path, sizeof(save_path), rel_path)) {
            printf("Could not resolve save path (HOME not set)\n");
            return false;
        }
        return Games_RemoveFileLogged(save_path);
    }
    return false;
}

//Takes a snapshot of every save entry currently in the given directory
void Games_SnapshotSaveFolder(const char* dirPath) {
    g_saveSnapshotCount = 0;
    DIR* dir = opendir(dirPath);
    //Check for error opening directory
    if (!dir) {
        return;
    }
    struct dirent* entry;
    //Read all the entries in the directory
    while ((entry = readdir(dir)) != NULL && g_saveSnapshotCount < MAX_SNAPSHOT_ENTRIES) {
        //Go to next entry
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        snprintf(g_saveSnapshot[g_saveSnapshotCount], SNAPSHOT_NAME_LEN, "%s", entry->d_name);
        g_saveSnapshotCount++;
    }
    closedir(dir);
}

//Returns true if name appeared in the snapshot taken by Games_SnapshotSaveFolde
static bool Games_WasInSnapshot(const char* name) {
    //Go through all saved snapshots
    for (int i = 0; i < g_saveSnapshotCount; i++) {
        //If it has a save, return true
        if (strcmp(g_saveSnapshot[i], name) == 0) {
            return true;
        }
    }
    return false;
}

//Detects a newly-created Flycast save
void Games_DetectNewFlycastSaveFile(game_t* game, const char* saveDir) {
    //If already has the serial saved, return
    if (game->serial[0] != '\0') return;
    DIR* dir = opendir(saveDir);
    //Check for error opening directory
    if (!dir) return;
    struct dirent* entry;
    //Read all entries in the directory
    while ((entry = readdir(dir)) != NULL) {
        //Go to next entry
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        char* suffix = strstr(entry->d_name, "_vmu_save_A1.bin");
        //If doesn't follow correct pattern, go to next entry
        if (!suffix) continue;
        //If save was already in previous snapshot, go to next entry
        if (Games_WasInSnapshot(entry->d_name)) continue;
        //Extract just the serial portion (everything before the suffix)
        size_t serialLen = (size_t)(suffix - entry->d_name);
        //If length too big, decrease accordingly
        if (serialLen >= sizeof(game->serial)) serialLen = sizeof(game->serial) - 1;
        //Copy over serial
        memcpy(game->serial, entry->d_name, serialLen);
        game->serial[serialLen] = '\0';
        printf("Recorded Flycast save ID '%s' for %s\n", game->serial, game->romPath);
        FILE* f = fopen("assets/txt/flycast_save_map.txt", "a");
        //Check for error opening file
        if (f) {
            fprintf(f, "%s|%s\n", game->romPath, game->serial);
            fclose(f);
        }
        break;
    }
    closedir(dir);
}

//Reads Flycast maps file and sets serial variables for the correct games
static void Games_LoadFlycastSaveMap(void) {
    FILE* f = fopen("assets/txt/flycast_save_map.txt", "r");
    //Check for error opening file
    if (!f) return;
    char line[MAP_FILE_LINE_LEN];
    //Read the lines of the file
    while (fgets(line, sizeof(line), f)) {
        char* sep = strchr(line, '|');
        //Skip bad lines
        if (!sep) continue;
        *sep = '\0';
        char* romPath = line;
        char* serial = sep + 1;
        size_t len = strlen(serial);
        //Trim newline
        if (len > 0 && serial[len - 1] == '\n') serial[len - 1] = '\0';
        //Go through the games
        for (int i = 0; i < gamesLen; i++) {
            //Match game to ROM path and set serial
            if (strcmp(gamesLibrary[i].romPath, romPath) == 0) {
                snprintf(gamesLibrary[i].serial, sizeof(gamesLibrary[i].serial), "%s", serial);
                break;
            }
        }
    }
    fclose(f);
}

//Detects new Ymir saves and sets serials and updates file
void Games_DetectNewYmirSaveFolder(game_t* game, const char* savestatesDir) {
    //If already has a serial, exit
    if (game->serial[0] != '\0') return;
    DIR* dir = opendir(savestatesDir);
    //Check for error opening directory
    if (!dir) return;
    struct dirent* entry;
    //Read all entries in the directory
    while ((entry = readdir(dir)) != NULL) {
        //Go to next entry
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        //If save was already in the snapshot
        if (Games_WasInSnapshot(entry->d_name)) continue;
        snprintf(game->serial, sizeof(game->serial), "%s", entry->d_name);
        printf("Recorded Ymir save folder '%s' for %s\n", game->serial, game->romPath);
        FILE* f = fopen("assets/txt/ymir_save_map.txt", "a");
        //If file opens, write the serial with the game name
        if (f) {
            fprintf(f, "%s|%s\n", game->romPath, game->serial);
            fclose(f);
        }
        break;
    }
    closedir(dir);
}

//Reads Ymir maps file and sets serial variables for the correct games
static void Games_LoadYmirSaveMap(void) {
    FILE* f = fopen("assets/txt/ymir_save_map.txt", "r");
    //Check for error opening file
    if (!f) return;
    char line[MAP_FILE_LINE_LEN];
    //Read the lines of the file
    while (fgets(line, sizeof(line), f)) {
        char* sep = strchr(line, '|');
        //Skip bad lines
        if (!sep) continue;
        *sep = '\0';
        char* romPath = line;
        char* folderName = sep + 1;
        size_t len = strlen(folderName);
        //Trim newline
        if (len > 0 && folderName[len - 1] == '\n') folderName[len - 1] = '\0';
        //Go through the games
        for (int i = 0; i < gamesLen; i++) {
            //Match game to ROM path and set serial
            if (strcmp(gamesLibrary[i].romPath, romPath) == 0) {
                snprintf(gamesLibrary[i].serial, sizeof(gamesLibrary[i].serial), "%s", folderName);
                break;
            }
        }
    }
    fclose(f);
}

//Detects new PS1 saves and sets serials and updates file
void Games_DetectNewPS1SaveFile(game_t* game, const char* savesDir) {
    if (game->serial[0] != '\0') return;
    DIR* dir = opendir(savesDir);
    //Check for error opening directory
    if (!dir) return;
    struct dirent* entry;
    //Read all entries in the directory
    while ((entry = readdir(dir)) != NULL) {
        //Go to next entry
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        //Skip non mcd files
        if (!IsFileExtension(entry->d_name, ".mcd")) continue;
        //If save was already in the snapshot
        if (Games_WasInSnapshot(entry->d_name)) continue;
        snprintf(game->serial, sizeof(game->serial), "%s", GetFileNameWithoutExt(entry->d_name));
        printf("Recorded PS1 save ID '%s' for %s\n", game->serial, game->romPath);
        FILE* f = fopen("assets/txt/ps1_save_map.txt", "a");
        //If file opens, write the serial with the game name
        if (f) {
            fprintf(f, "%s|%s\n", game->romPath, game->serial);
            fclose(f);
        }
        break;
    }
    closedir(dir);
}

//Reads PS1 maps file and sets serial variables for the correct games
static void Games_LoadPS1SaveMap(void) {
    FILE* f = fopen("assets/txt/ps1_save_map.txt", "r");
    //Check for error opening file
    if (!f) return;
    char line[MAP_FILE_LINE_LEN];
    //Read the lines of the file
    while (fgets(line, sizeof(line), f)) {
        char* sep = strchr(line, '|');
        //Skip bad lines
        if (!sep) continue;
        *sep = '\0';
        char* romPath = line;
        char* serial = sep + 1;
        size_t len = strlen(serial);
        //Trim newline
        if (len > 0 && serial[len - 1] == '\n') serial[len - 1] = '\0';
        //Go through the games
        for (int i = 0; i < gamesLen; i++) {
            //Match game to ROM path and set serial
            if (strcmp(gamesLibrary[i].romPath, romPath) == 0) {
                snprintf(gamesLibrary[i].serial, sizeof(gamesLibrary[i].serial), "%s", serial);
                break;
            }
        }
    }
    fclose(f);
}

//Extract GameCube save ID
bool GetGameCubeSaveID(const char* filePath, char* outSaveID, size_t maxLen) {
    FILE* f = fopen(filePath, "rb");
    //Check for error opening file
    if (!f) return false;
    char magic[CISO_MAGIC_LEN] = {0};
    long headerOffset = 0x00;
    //Read the line
    if (fread(magic, 1, CISO_MAGIC_LEN, f) == 4) {
        //Check for CISO Header Signature
        if (memcmp(magic, "CISO", CISO_MAGIC_LEN) == 0) {
            headerOffset = CISO_HEADER_SIZE;
        }
    }
    //Seek to the actual GameCube header location
    if (fseek(f, headerOffset, SEEK_SET) != 0) {
        fclose(f);
        return false;
    }
    char gameCode[GC_GAME_CODE_LEN + 1] = {0};
    char makerCode[GC_MAKER_CODE_LEN + 1] = {0};
    //Read 4-byte Game Code
    if (fread(gameCode, 1, GC_GAME_CODE_LEN, f) == 4) {
        //Read 2-byte Maker Code
        if (fread(makerCode, 1, GC_MAKER_CODE_LEN, f) == GC_MAKER_CODE_LEN) {
            snprintf(outSaveID, maxLen, "%s-%s", makerCode, gameCode);
            fclose(f);
            return true;
        }
    }
    fclose(f);
    return false;
}

//Extract PSP save ID
bool GetPSPSaveID(const char* filePath, char* outSaveID, size_t maxLen) {
    FILE* f = fopen(filePath, "rb");
    //Check for error opening file
    if (!f) return false;
    //Seek to the correct location
    if (fseek(f, PSP_ISO_DISC_ID_OFFSET, SEEK_SET) == 0) {
        char raw[PSP_RAW_SERIAL_LEN + 1] = {0};
        //Read the line
        if (fread(raw, 1, PSP_RAW_SERIAL_LEN, f) == PSP_RAW_SERIAL_LEN) {
            char clean[PSP_CLEAN_SERIAL_MIN_LEN + 2] = {0};
            int cIdx = 0;
            //Strip hyphens if present to form clean standard directory ID
            for (int i = 0; i < PSP_CLEAN_SERIAL_MIN_LEN + 1; i++) {
                if (isalnum((unsigned char)raw[i])) {
                    clean[cIdx++] = raw[i];
                }
            }
            if (cIdx >= PSP_CLEAN_SERIAL_MIN_LEN) {
                snprintf(outSaveID, maxLen, "%s", clean);
                fclose(f);
                return true;
            }
        }
    }
    fclose(f);
    return false;
}

//Set attributes of games
static void Games_SetAttributes(save_t saveType, bool libretro, char* category, char* path, char* process) {
    //Go through all games in the section
    for (int i = temp_start_section; i < gamesLen; i++) {
        gamesLibrary[i].save = saveType;
        gamesLibrary[i].libRetro = libretro;
        gamesLibrary[i].category = category;
        gamesLibrary[i].corePath = path;
        //If not libretro, set the process name
        if (!libretro) {
            gamesLibrary[i].processName = process;
        }
    }
}

//Reads a title string at a given offset
static bool Games_TryReadTitleAt(FILE* f, long offset, size_t rawLen, char* outTitle, size_t outSize) {
    //Go to the offset
    if (fseek(f, offset, SEEK_SET) != 0) return false;
    char* rawTitle = malloc(rawLen);
    //If no title, exit
    if (!rawTitle) return false;
    bool success = false;
    //Read the raw title
    if (fread(rawTitle, 1, rawLen - 1, f) == rawLen - 1) {
        rawTitle[rawLen - 1] = '\0';
        size_t len = strlen(rawTitle);
        //Trim trailing whitespace
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
    //Check for error opening file
    if (!f) return;
    bool success = Games_TryReadTitleAt(f, offset, rawLen, game->title, TITLE_MAX_LEN);
    fclose(f);
    //Fallback case
    if (!success) {
        snprintf(game->title, TITLE_MAX_LEN, "%s", GetFileNameWithoutExt(romPath));
    }
}

//Extract GameCube title
void Games_GetGCTitle(const char* romPath, game_t* game) {
    FILE* f = fopen(romPath, "rb");
    //Check for error opening file
    if (!f) return;
    unsigned char magic[CISO_MAGIC_LEN];
    bool success = false;
    //Check for CISO (Compact ISO) magic header bytes
    if (fread(magic, 1, CISO_MAGIC_LEN, f) == 4 && memcmp(magic, "CISO", CISO_MAGIC_LEN) == 0) {
        //Read the CISO block map flag at byte offset 8
        unsigned char bitmapByte;
        fseek(f, CISO_MAP_OFFSET, SEEK_SET);
        //Verify block 0 is present in the block map before reading title data
        if (fread(&bitmapByte, 1, 1, f) == 1 && (bitmapByte & CISO_BLOCK0_MASK)) {
            success = Games_TryReadTitleAt(f, CISO_HEADER_SIZE + GC_OFFSET, GC_RAW_LEN, game->title, TITLE_MAX_LEN);
        }
    }
    //Plain, uncompressed ISO — read directly at the normal offset
    else {
        success = Games_TryReadTitleAt(f, GC_OFFSET, GC_RAW_LEN, game->title, TITLE_MAX_LEN);
    }
    fclose(f);
    //Fallback case
    if (!success) {
        snprintf(game->title, TITLE_MAX_LEN, "%s", GetFileNameWithoutExt(romPath));
    }
}

//Extract SNES title
void Games_GetSNESTitle(const char* romPath, game_t* game) {
    FILE* f = fopen(romPath, "rb");
    //Check for error opening file
    if (!f) return;
    //Determine file size to detect a 512-byte copier header
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        snprintf(game->title, TITLE_MAX_LEN, "%s", GetFileNameWithoutExt(romPath));
        return;
    }
    long fileSize = ftell(f);
    long headerOffset = ((fileSize % SNES_BLOCK_ALIGNMENT) == SNES_COPIER_HEADER_SIZE) ? SNES_COPIER_HEADER_SIZE : 0;
    //Try LoROM location first
    bool success = Games_TryReadTitleAt(f, headerOffset + SNES_LOROM_OFFSET, SNES_RAW_LEN, game->title, TITLE_MAX_LEN);
    //If that didn't work, try HiROM location
    if (!success) {
        success = Games_TryReadTitleAt(f, headerOffset + SNES_HIROM_OFFSET, SNES_RAW_LEN, game->title, TITLE_MAX_LEN);
    }
    fclose(f);
    //Fallback case
    if (!success) {
        snprintf(game->title, TITLE_MAX_LEN, "%s", GetFileNameWithoutExt(romPath));
    }
}

//Checks whether a given base filename is already covered by a processed M3U playlist
static bool Games_IsCoveredByM3U(const char* filename) {
    //Go through all the files in the list
    for (int i = 0; i < m3uSkipCount; i++) {
        //If the file is on the list, skip it
        if (strcmp(m3uSkipList[i], filename) == 0) return true;
    }
    return false;
}

//Scan directory for M3U multi-disc playlists and register them as playable entries
static void Games_ScanM3UPlaylists(const char* dirPath, const char* console) {
    temp_start_section = gamesLen;
    FilePathList files = LoadDirectoryFiles(dirPath);
    //Iterate through all files in the target folder
    for (unsigned int i = 0; i < files.count; i++) {
        //Skip files that do not have an .m3u extension
        if (!IsFileExtension(files.paths[i], ".m3u")) continue;
        FILE* f = fopen(files.paths[i], "r");
        //Skip playlist if file cannot be opened
        if (!f) continue;
        char line[M3U_LINE_LEN];
        bool addedEntry = false;
        //Read each disc entry referenced inside the M3U playlist file
        while (fgets(line, sizeof(line), f)) {
            size_t len = strlen(line);
            //Trim trailing carriage returns and newlines from line string
            while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
                line[--len] = '\0';
            }
            //Skip blank lines and comments
            if (len == 0 || line[0] == '#') continue;
            const char* base = GetFileName(line);
            //Record just the base filename so it matches later, regardless of path style
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

                char coverPath[sizeof(gamesLibrary[gamesLen].coverPath)];
                snprintf(coverPath, sizeof(coverPath), "assets/images/%s/%s.png", console, GetFileNameWithoutExt(files.paths[i]));
                //If file exists there, add cover path
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
    Games_SetAttributes(EXTERNAL, true, "PlayStation", PATH_PS1, NULL);
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
    // Validate minimum SFO header size (20 bytes)
    if (sfoSize < SFO_MIN_HEADER_SIZE) return false;
    uint32_t magic = ReadLE32(sfoData, 0);
    // Verify "\0PSF" magic bytes (0x00465350 in little-endian)
    if (magic != SFO_MAGIC) return false;
    uint32_t keyTableOffset = ReadLE32(sfoData, SFO_HEADER_KEY_OFFSET);
    uint32_t dataTableOffset = ReadLE32(sfoData, SFO_HEADER_DATA_OFFSET);
    uint32_t entryCount = ReadLE32(sfoData, SFO_HEADER_COUNT_OFFSET);
    // Iterate through index entries in the SFO header
    for (uint32_t i = 0; i < entryCount; i++) {
        size_t entryOffset = SFO_MIN_HEADER_SIZE + (i * SFO_INDEX_ENTRY_SIZE);
        // Ensure the current index entry lies within buffer bounds
        if (entryOffset + SFO_INDEX_ENTRY_SIZE > sfoSize) break;
        uint16_t keyOffset = ReadLE16(sfoData, entryOffset + SFO_ENTRY_KEY_OFFSET);
        uint32_t dataLen = ReadLE32(sfoData, entryOffset + SFO_ENTRY_DATALEN_OFFSET);
        uint32_t dataOffset = ReadLE32(sfoData, entryOffset + SFO_ENTRY_DATA_OFFSET);
        size_t keyPos = keyTableOffset + keyOffset;
        // Bounds-check the key string offset
        if (keyPos >= sfoSize) continue;
        //Check if this key is "TITLE"
        if (strcmp((const char*)(sfoData + keyPos), "TITLE") == 0) {
            size_t dataPos = dataTableOffset + dataOffset;
            // Ensure the value string length fits within the loaded buffer
            if (dataPos + dataLen > sfoSize) return false;
            size_t copyLen = (dataLen < outSize) ? dataLen : outSize - 1;
            memcpy(outTitle, sfoData + dataPos, copyLen);
            outTitle[copyLen] = '\0';
            size_t len = strlen(outTitle);
            //Trim trailing whitespace/nulls
            while (len > 0 && isspace((unsigned char)outTitle[len - 1])) {
                outTitle[--len] = '\0';
            }
            return (len > 0);
        }
    }
    return false;
}

//Extracts the title from a PSP ISO
static void Games_GetPSPTitle(const char* romPath, game_t* game) {
    FILE* f = fopen(romPath, "rb");
    //Check for error opening file
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
                //Bounds check before reading the name bytes out of rootDir
                //(previously compared a pointer to rootDirSize, which never
                //caught an out-of-range read)
                if (pos + 33 + copyLen <= rootDirSize) {
                    memcpy(name, rootDir + pos + 33, copyLen);
                }

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
    //Fallback case
    if (!foundSFO) {
        snprintf(game->title, TITLE_MAX_LEN, "%s", GetFileNameWithoutExt(romPath));
    }
}

//Get all the roms for the specific section
static void Games_GetRoms(char* romExtension, char* dirPath, char* console, save_t saveType, bool libretro, char* category, char* path, char* process) {
    temp_start_section = gamesLen;
    FilePathList files = LoadDirectoryFiles(dirPath);
    //Go through all the files in the directory
    for (unsigned int i = 0; i < files.count; i++) {
        //If the file has the correct extension
        if (IsFileExtension(files.paths[i], romExtension)) {
            //Check if file needs to be skipped
            if (Games_IsCoveredByM3U(GetFileName(files.paths[i]))) {
                continue;
            }
            snprintf(gamesLibrary[gamesLen].romPath, sizeof(gamesLibrary[gamesLen].romPath), "%s", files.paths[i]);
            snprintf(gamesLibrary[gamesLen].console, sizeof(gamesLibrary[gamesLen].console), "%s", console);
            //Get the title of the game depending on the console
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
            char coverPath[sizeof(gamesLibrary[gamesLen].coverPath)];
                snprintf(coverPath, sizeof(coverPath), "assets/images/%s/%s.png", console, GetFileNameWithoutExt(files.paths[i]));
            //Check if local cover image exists
            if (FileExists(coverPath)) {
                snprintf(gamesLibrary[gamesLen].coverPath, sizeof(gamesLibrary[gamesLen].coverPath), "%s", coverPath);
            }
            gamesLen += 1;
        }
    }
    UnloadDirectoryFiles(files);
    Games_SetAttributes(saveType, libretro, category, path, process);
}

//Fill the rest of the category with blank games
static void Games_FillCategory(char* category) {
    //If not a sufficient number of games in category, fill with blank games
    while (gamesLen < temp_start_categ + MIN_GAMES_IN_CATEG) {
        snprintf(gamesLibrary[gamesLen].title, sizeof(gamesLibrary[gamesLen].title), "NO GAME AVAILABLE");
        snprintf(gamesLibrary[gamesLen].console, sizeof(gamesLibrary[gamesLen].console), "N/A");
        gamesLibrary[gamesLen].corePath = "";
        snprintf(gamesLibrary[gamesLen].romPath, sizeof(gamesLibrary[gamesLen].romPath), "/0");
        snprintf(gamesLibrary[gamesLen].coverPath, sizeof(gamesLibrary[gamesLen].coverPath), "/0");
        gamesLibrary[gamesLen].category = category;
        gamesLibrary[gamesLen].libRetro = true;
        gamesLen += 1;
    }
}

//Initialize game library
void Games_Init() {
    gamesLen = 0;
    //Arcade games
    temp_start_categ = 0;
    Games_GetRoms("zip", "assets/roms/Arcade", "Arcade", NONE, true, "Arcade", PATH_ARCADE, NULL);
    Games_FillCategory("Arcade");
    //Handheld games
    temp_start_categ = gamesLen;
    Games_GetRoms("nds", "assets/roms/Nintendo DS", "Nintendo DS", EXTERNAL, false, "Handheld Classics", PATH_DS, PROCESS_MELON);
    Games_GetRoms("gg", "assets/roms/Sega Game Gear", "Sega Game Gear", EXTERNAL, true, "Handheld Classics", PATH_GENESIS, NULL);
    Games_GetRoms("gba", "assets/roms/Game Boy Advance", "Game Boy Advance", BATTERY, true, "Handheld Classics", PATH_GBA, NULL);
    Games_GetRoms("gbc", "assets/roms/Game Boy Color", "Game Boy Color", BATTERY, true, "Handheld Classics", PATH_GAMEBOY, NULL);
    Games_GetRoms("gb", "assets/roms/Game Boy", "Game Boy", BATTERY, true, "Handheld Classics", PATH_GAMEBOY, NULL);
    Games_GetRoms("ngc", "assets/roms/Neo Geo Pocket Color", "Neo Geo Pocket Color", NONE, true, "Handheld Classics", PATH_NGPC, NULL);
    Games_GetRoms("iso", "assets/roms/Sony PlayStation Portable", "Sony PlayStation Portable", EXTERNAL, false, "Handheld Classics", PATH_PSP, PROCESS_PPSSPP);
    //Get PSP save IDs
    for (int i = temp_start_section; i < gamesLen; i++) {
        GetPSPSaveID(gamesLibrary[i].romPath, gamesLibrary[i].serial, sizeof(gamesLibrary[i].serial));
    }
    Games_GetRoms("lyx", "assets/roms/Atari Lynx", "Atari Lynx", NONE, true, "Handheld Classics", PATH_LYNX, NULL);
    Games_FillCategory("Handheld Classics");
    //Nintendo 3D games
    temp_start_categ = gamesLen;
    Games_GetRoms("ciso", "assets/roms/Nintendo GameCube", "Nintendo GameCube", EXTERNAL, false, "Nintendo 3D", PATH_GAMECUBE, PROCESS_DOLPHIN);
    //Get GameCube save IDs
    for (int i = temp_start_section; i < gamesLen; i++) {
        GetGameCubeSaveID(gamesLibrary[i].romPath, gamesLibrary[i].serial, sizeof(gamesLibrary[i].serial));
    }
    Games_GetRoms("z64", "assets/roms/Nintendo 64", "Nintendo 64", BATTERY, true, "Nintendo 3D", PATH_N64, NULL);
    Games_FillCategory("Nintendo 3D");
    //Nintendo retro games
    temp_start_categ = gamesLen;
    Games_GetRoms("nes", "assets/roms/Nintendo Entertainment System", "Nintendo Entertainment System", BATTERY, true, "Retro Nintendo", PATH_NES, NULL);
    Games_GetRoms("sfc", "assets/roms/Super Nintendo Entertainment System", "Super Nintendo Entertainment System", BATTERY, true, "Retro Nintendo", PATH_SNES, NULL);
    Games_GetRoms("smc", "assets/roms/Super Nintendo Entertainment System", "Super Nintendo Entertainment System", BATTERY, true, "Retro Nintendo", PATH_SNES, NULL);
    Games_FillCategory("Retro Nintendo");
    //PC/Indie/Other games
    temp_start_categ = gamesLen;
    Games_GetRoms("pce", "assets/roms/TurboGrafx-16", "TurboGrafx-16", BATTERY, true, "TurboGrafx/PC/Other", PATH_TG16, NULL);
    Games_GetRoms("chd", "assets/roms/TurboGrafx-CD", "TurboGrafx-CD", BATTERY, true, "TurboGrafx/PC/Other", PATH_TG16, NULL);
    Games_GetRoms("WAD", "assets/roms/PC", "PC", BATTERY, true, "TurboGrafx/PC/Other", PATH_PRBOOM, NULL);
    Games_FillCategory("TurboGrafx/PC/Other");
    //Sega games
    temp_start_categ = gamesLen;
    Games_GetRoms("chd", "assets/roms/Sega CD", "Sega CD", EXTERNAL, true, "Sega", PATH_GENESIS, NULL);
    Games_GetRoms("chd", "assets/roms/Sega Dreamcast", "Sega Dreamcast", EXTERNAL, false, "Sega", PATH_DREAMCAST, PROCESS_FLYCAST);
    Games_LoadFlycastSaveMap();
    Games_GetRoms("md", "assets/roms/Sega Genesis", "Sega Genesis", BATTERY, true, "Sega", PATH_GENESIS, NULL);
    Games_GetRoms("sms", "assets/roms/Sega Master System", "Sega Master System", NONE, true, "Sega", PATH_GENESIS, NULL);
    Games_GetRoms("chd", "assets/roms/Sega Saturn", "Sega Saturn", EXTERNAL, false, "Sega", PATH_SATURN, PROCESS_YMIR);
    Games_LoadYmirSaveMap();
    Games_FillCategory("Sega");
    //PS1 games
    temp_start_categ = gamesLen;
    Games_ScanM3UPlaylists("assets/roms/Sony PlayStation", "Sony PlayStation");
    Games_GetRoms("chd", "assets/roms/Sony PlayStation", "Sony PlayStation", EXTERNAL, true, "PlayStation", PATH_PS1, NULL);
    Games_LoadPS1SaveMap();
    Games_FillCategory("Sony PlayStation");
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