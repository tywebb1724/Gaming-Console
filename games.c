#include "games.h"
#include "raylib.h"
#include "ui.h"

Game gameLibrary[MAX_GAMES];

Game gamesDisplayed[];

int gamesLen;
int start_index;
int end_index;

static bool texturesLoaded = false;

//Initialize game library
void Games_Init() {
    //Arcade games
    gameLibrary[0].title = "Metal Slug 3";
    gameLibrary[0].coverPath = "./assets/covers/arcade/metal_slug_3.png";
    gameLibrary[0].cover.id = 0;
    gameLibrary[0].index = 0;
    gameLibrary[0].category = ARCADE;
    
    gameLibrary[1].title = "Pac-Man";
    gameLibrary[1].coverPath = "./assets/covers/arcade/pac-man.png";
    gameLibrary[1].cover.id = 0;
    gameLibrary[1].index = 1;
    gameLibrary[1].category = ARCADE;

    gameLibrary[2].title = "Simpsons Arcade Game";
    gameLibrary[2].coverPath = "./assets/covers/arcade/simpsons_arcade.png";
    gameLibrary[2].cover.id = 0;
    gameLibrary[2].index = 2;
    gameLibrary[2].category = ARCADE;

    gameLibrary[3].title = "Street Fighter: 3rd Strike";
    gameLibrary[3].coverPath = "./assets/covers/arcade/street_fighter_3rd_strike.png";
    gameLibrary[3].cover.id = 0;
    gameLibrary[3].index = 3;
    gameLibrary[3].category = ARCADE;

    gameLibrary[4].title = "Teenage Mutant Ninja Turtles: Turtles in Time";
    gameLibrary[4].coverPath = "./assets/covers/arcade/tmnt_turtles_in_time.png";
    gameLibrary[4].cover.id = 0;
    gameLibrary[4].index = 4;
    gameLibrary[4].category = ARCADE;

    //Handheld classics games
    gameLibrary[5].title = "GTA: Liberty City Stories";
    gameLibrary[5].coverPath = "./assets/covers/handheld/gta_liberty.png";
    gameLibrary[5].cover.id = 0;
    gameLibrary[5].index = 5;
    gameLibrary[5].category = HANDHELD;

    gameLibrary[6].title = "Mario Kart DS";
    gameLibrary[6].coverPath = "./assets/covers/handheld/mario_kart_ds.png";
    gameLibrary[6].cover.id = 0;
    gameLibrary[6].index = 6;
    gameLibrary[6].category = HANDHELD;

    gameLibrary[7].title = "Pokemon HeartGold";
    gameLibrary[7].coverPath = "./assets/covers/handheld/pokemon_heartgold.png";
    gameLibrary[7].cover.id = 0;
    gameLibrary[7].index = 7;
    gameLibrary[7].category = HANDHELD;

    gameLibrary[8].title = "Pokemon SoulSilver";
    gameLibrary[8].coverPath = "./assets/covers/handheld/pokemon_soulsilver.png";
    gameLibrary[8].cover.id = 0;
    gameLibrary[8].index = 8;
    gameLibrary[8].category = HANDHELD;

    gameLibrary[9].title = "The Legend of Zelda: The Minish Cap";
    gameLibrary[9].coverPath = "./assets/covers/handheld/zelda_minish_cap.png";
    gameLibrary[9].cover.id = 0;
    gameLibrary[9].index = 9;
    gameLibrary[9].category = HANDHELD;

    //Nintendo 3D games
    gameLibrary[10].title = "Super Mario 64";
    gameLibrary[10].coverPath = "./assets/covers/nint_3d/mario_64.png";
    gameLibrary[10].cover.id = 0;
    gameLibrary[10].index = 10;
    gameLibrary[10].category = NINTENDO_3D;

    gameLibrary[11].title = "Super Mario Sunshine";
    gameLibrary[11].coverPath = "./assets/covers/nint_3d/mario_sunshine.png";
    gameLibrary[11].cover.id = 0;
    gameLibrary[11].index = 11;
    gameLibrary[11].category = NINTENDO_3D;

    gameLibrary[12].title = "Starfox 64";
    gameLibrary[12].coverPath = "./assets/covers/nint_3d/starfox_64.png";
    gameLibrary[12].cover.id = 0;
    gameLibrary[12].index = 12;
    gameLibrary[12].category = NINTENDO_3D;

    gameLibrary[13].title = "The Legend of Zelda: Ocarina of Time";
    gameLibrary[13].coverPath = "./assets/covers/nint_3d/zelda_ocarina.png";
    gameLibrary[13].cover.id = 0;
    gameLibrary[13].index = 13;
    gameLibrary[13].category = NINTENDO_3D;

    gameLibrary[14].title = "The Legend of Zelda: The Wind Waker";
    gameLibrary[14].coverPath = "./assets/covers/nint_3d/zelda_windwaker.png";
    gameLibrary[14].cover.id = 0;
    gameLibrary[14].index = 14;
    gameLibrary[14].category = NINTENDO_3D;

    //Retro Nintendo games
    gameLibrary[15].title = "Donkey Kong Country 2: Diddy's Kong Quest";
    gameLibrary[15].coverPath = "./assets/covers/nint_ret/dk_country_2.png";
    gameLibrary[15].cover.id = 0;
    gameLibrary[15].index = 15;
    gameLibrary[15].category = NINTENDO_RETRO;

    gameLibrary[16].title = "Punch-Out";
    gameLibrary[16].coverPath = "./assets/covers/nint_ret/punch_out.png";
    gameLibrary[16].cover.id = 0;
    gameLibrary[16].index = 16;
    gameLibrary[16].category = NINTENDO_RETRO;

    gameLibrary[17].title = "Super Mario World";
    gameLibrary[17].coverPath = "./assets/covers/nint_ret/super_mario_world.png";
    gameLibrary[17].cover.id = 0;
    gameLibrary[17].index = 17;
    gameLibrary[17].category = NINTENDO_RETRO;

    gameLibrary[18].title = "Super Metroid";
    gameLibrary[18].coverPath = "./assets/covers/nint_ret/super_metroid.png";
    gameLibrary[18].cover.id = 0;
    gameLibrary[18].index = 18;
    gameLibrary[18].category = NINTENDO_RETRO;

    gameLibrary[19].title = "The Legend of Zelda";
    gameLibrary[19].coverPath = "./assets/covers/nint_ret/zelda.png";
    gameLibrary[19].cover.id = 0;
    gameLibrary[19].index = 19;
    gameLibrary[19].category = NINTENDO_RETRO;

    //Retro PC & indie games
    gameLibrary[20].title = "Counter Strike 1.6";
    gameLibrary[20].coverPath = "./assets/covers/pc_ind/counter_strike_1.6.png";
    gameLibrary[20].cover.id = 0;
    gameLibrary[20].index = 20;
    gameLibrary[20].category = PC_INDIE;

    gameLibrary[21].title = "Doom";
    gameLibrary[21].coverPath = "./assets/covers/pc_ind/doom_2.png";
    gameLibrary[21].cover.id = 0;
    gameLibrary[21].index = 21;
    gameLibrary[21].category = PC_INDIE;

    gameLibrary[22].title = "Doom 2";
    gameLibrary[22].coverPath = "./assets/covers/pc_ind/doom.png";
    gameLibrary[22].cover.id = 0;
    gameLibrary[22].index = 22;
    gameLibrary[22].category = PC_INDIE;

    gameLibrary[23].title = "Half-Life";
    gameLibrary[23].coverPath = "./assets/covers/pc_ind/half-life.png";
    gameLibrary[23].cover.id = 0;
    gameLibrary[23].index = 23;
    gameLibrary[23].category = PC_INDIE;

    gameLibrary[24].title = "Stardew Valley";
    gameLibrary[24].coverPath = "./assets/covers/pc_ind/stardew_valley.png";
    gameLibrary[24].cover.id = 0;
    gameLibrary[24].index = 24;
    gameLibrary[24].category = PC_INDIE;

    //Sega central games
    gameLibrary[25].title = "Crazy Taxi";
    gameLibrary[25].coverPath = "./assets/covers/sega/crazy_taxi.png";
    gameLibrary[25].cover.id = 0;
    gameLibrary[25].index = 25;
    gameLibrary[25].category = SEGA;

    gameLibrary[26].title = "Fighting Vipers";
    gameLibrary[26].coverPath = "./assets/covers/sega/fighting_vipers.png";
    gameLibrary[26].cover.id = 0;
    gameLibrary[26].index = 26;
    gameLibrary[26].category = SEGA;

    gameLibrary[27].title = "Sonic the Hedgehog 3 & Knuckles";
    gameLibrary[27].coverPath = "./assets/covers/sega/sonic_3_and_knuckles.png";
    gameLibrary[27].cover.id = 0;
    gameLibrary[27].index = 27;
    gameLibrary[27].category = SEGA;

    gameLibrary[28].title = "Sonic CD";
    gameLibrary[28].coverPath = "./assets/covers/sega/sonic_cd.png";
    gameLibrary[28].cover.id = 0;
    gameLibrary[28].index = 28;
    gameLibrary[28].category = SEGA;

    gameLibrary[29].title = "X-Men vs Street Fighter";
    gameLibrary[29].coverPath = "./assets/covers/sega/x-men_vs_street_fighter.png";
    gameLibrary[29].cover.id = 0;
    gameLibrary[29].index = 29;
    gameLibrary[29].category = SEGA;

    //Sony Playstation games
    gameLibrary[30].title = "Megaman X4";
    gameLibrary[30].coverPath = "./assets/covers/playstation/megaman_x4.png";
    gameLibrary[30].cover.id = 0;
    gameLibrary[30].index = 30;
    gameLibrary[30].category = PLAYSTATION;

    gameLibrary[31].title = "Pac-Man World";
    gameLibrary[31].coverPath = "./assets/covers/playstation/pac-man_world.png";
    gameLibrary[31].cover.id = 0;
    gameLibrary[31].index = 31;
    gameLibrary[31].category = PLAYSTATION;

    gameLibrary[32].title = "Soulblade";
    gameLibrary[32].coverPath = "./assets/covers/playstation/soulblade.png";
    gameLibrary[32].cover.id = 0;
    gameLibrary[32].index = 32;
    gameLibrary[32].category = PLAYSTATION;

    gameLibrary[33].title = "Spider-Man (2000)";
    gameLibrary[33].coverPath = "./assets/covers/playstation/spider-man.png";
    gameLibrary[33].cover.id = 0;
    gameLibrary[33].index = 33;
    gameLibrary[33].category = PLAYSTATION;

    gameLibrary[34].title = "Street Fighter: Alpha 3";
    gameLibrary[34].coverPath = "./assets/covers/playstation/street_fighter_alpha_3.png";
    gameLibrary[34].cover.id = 0;
    gameLibrary[34].index = 34;
    gameLibrary[34].category = PLAYSTATION;

    gamesLen = sizeof(gameLibrary) / sizeof(gameLibrary[0]);

    Games_UpdateDisplayed(NINTENDO_3D);

    texturesLoaded = false;
}

//Update the games displayed for the new category
void Games_UpdateDisplayed(Categories categ) {
    for (int i = 0; i < gamesLen; i++) {
        if (gameLibrary[i].category == categ) {
            start_index = i;
            break;
        }
    }
    for (int i = start_index + 1; i < gamesLen; i++) {
        if (gameLibrary[i].category != categ || i == (gamesLen - 1)) {
            end_index = i;
            break;
        }
    }
    for (int i = 0; i < GAMES_ON_SCREEN + 2; i++) {
        gamesDisplayed[i].index = i + start_index;
    }
    for (int i = 0; i < GAMES_ON_SCREEN + 2; i++) {
        gamesDisplayed[i] = gameLibrary[gamesDisplayed[i].index];
    }
}

//Load game cover textures
void Games_LoadTextures() {
    if (texturesLoaded) {
        return;
    }

    for (int i = start_index; i < end_index - 1; i++) {
        gameLibrary[i].cover = LoadTexture(gameLibrary[i].coverPath);
    }

    texturesLoaded = true;
}

//Shift the order of the games to the right
void Games_ScrollRight() {
    for (int i = 0; i < GAMES_ON_SCREEN + 2; i++) {
        gamesDisplayed[i].index -= 1;
        if (gamesDisplayed[i].index < start_index) {
            gamesDisplayed[i].index = end_index;
        }
    }
    for (int i = 0; i < GAMES_ON_SCREEN + 2; i++) {
        gamesDisplayed[i] = gameLibrary[gamesDisplayed[i].index];
    }
}

//Shift the order of the gamees to the left
void Games_ScrollLeft() {
    for (int i = 0; i < GAMES_ON_SCREEN + 2; i++) {
        gamesDisplayed[i].index += 1;
        if (gamesDisplayed[i].index > end_index) {
            gamesDisplayed[i].index = start_index;
        }
    }
    for (int i = 0; i < GAMES_ON_SCREEN + 2; i++) {
        gamesDisplayed[i] = gameLibrary[gamesDisplayed[i].index];
    }
}
    
//Unload game cover textures
void Games_UnloadTextures() {
    for (int i = start_index; i < end_index - 1; i++) {
        UnloadTexture(gameLibrary[i].cover);
    }

    texturesLoaded = false;
}