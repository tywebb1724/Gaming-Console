#include "games.h"
#include "raylib.h"
#include "ui.h"
#include <stdio.h>
#include <unistd.h>
#include "categories.h"

Game gameLibrary[MAX_GAMES];

Game gamesDisplayed[GAMES_ON_SCREEN + 2];

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

    gameLibrary[5].title = "The Punisher Arcade";
    gameLibrary[5].coverPath = "./assets/covers/arcade/punisher_arcade.png";
    gameLibrary[5].cover.id = 0;
    gameLibrary[5].index = 5;
    gameLibrary[5].category = ARCADE;

    gameLibrary[6].title = "X-Men Arcade";
    gameLibrary[6].coverPath = "./assets/covers/arcade/x-men_arcade.png";
    gameLibrary[6].cover.id = 0;
    gameLibrary[6].index = 6;
    gameLibrary[6].category = ARCADE;

    //Handheld classics games
    gameLibrary[7].title = "GTA: Liberty City Stories";
    gameLibrary[7].coverPath = "./assets/covers/handheld/gta_liberty.png";
    gameLibrary[7].cover.id = 0;
    gameLibrary[7].index = 7;
    gameLibrary[7].category = HANDHELD;

    gameLibrary[8].title = "Mario Kart DS";
    gameLibrary[8].coverPath = "./assets/covers/handheld/mario_kart_ds.png";
    gameLibrary[8].cover.id = 0;
    gameLibrary[8].index = 8;
    gameLibrary[8].category = HANDHELD;

    gameLibrary[9].title = "Pokemon HeartGold";
    gameLibrary[9].coverPath = "./assets/covers/handheld/pokemon_heartgold.png";
    gameLibrary[9].cover.id = 0;
    gameLibrary[9].index = 9;
    gameLibrary[9].category = HANDHELD;

    gameLibrary[10].title = "Pokemon SoulSilver";
    gameLibrary[10].coverPath = "./assets/covers/handheld/pokemon_soulsilver.png";
    gameLibrary[10].cover.id = 0;
    gameLibrary[10].index = 10;
    gameLibrary[10].category = HANDHELD;

    gameLibrary[11].title = "The Legend of Zelda: The Minish Cap";
    gameLibrary[11].coverPath = "./assets/covers/handheld/zelda_minish_cap.png";
    gameLibrary[11].cover.id = 0;
    gameLibrary[11].index = 11;
    gameLibrary[11].category = HANDHELD;

    gameLibrary[12].title = "Chrono Trigger";
    gameLibrary[12].coverPath = "./assets/covers/handheld/chrono_trigger.png";
    gameLibrary[12].cover.id = 0;
    gameLibrary[12].index = 12;
    gameLibrary[12].category = HANDHELD;

    gameLibrary[13].title = "Monster Hunter Freedom Unite";
    gameLibrary[13].coverPath = "./assets/covers/handheld/monst_hunt_free_unite.png";
    gameLibrary[13].cover.id = 0;
    gameLibrary[13].index = 13;
    gameLibrary[13].category = HANDHELD;

    //Nintendo 3D games
    gameLibrary[14].title = "Super Mario 64";
    gameLibrary[14].coverPath = "./assets/covers/nint_3d/mario_64.png";
    gameLibrary[14].cover.id = 0;
    gameLibrary[14].index = 14;
    gameLibrary[14].category = NINTENDO_3D;

    gameLibrary[15].title = "Super Mario Sunshine";
    gameLibrary[15].coverPath = "./assets/covers/nint_3d/mario_sunshine.png";
    gameLibrary[15].cover.id = 0;
    gameLibrary[15].index = 15;
    gameLibrary[15].category = NINTENDO_3D;

    gameLibrary[16].title = "Starfox 64";
    gameLibrary[16].coverPath = "./assets/covers/nint_3d/starfox_64.png";
    gameLibrary[16].cover.id = 0;
    gameLibrary[16].index = 16;
    gameLibrary[16].category = NINTENDO_3D;

    gameLibrary[17].title = "The Legend of Zelda: Ocarina of Time";
    gameLibrary[17].coverPath = "./assets/covers/nint_3d/zelda_ocarina.png";
    gameLibrary[17].cover.id = 0;
    gameLibrary[17].index = 17;
    gameLibrary[17].category = NINTENDO_3D;

    gameLibrary[18].title = "The Legend of Zelda: The Wind Waker";
    gameLibrary[18].coverPath = "./assets/covers/nint_3d/zelda_windwaker.png";
    gameLibrary[18].cover.id = 0;
    gameLibrary[18].index = 18;
    gameLibrary[18].category = NINTENDO_3D;

    gameLibrary[19].title = "Super Smash Bros: Melee";
    gameLibrary[19].coverPath = "./assets/covers/nint_3d/smash_bros_melee.png";
    gameLibrary[19].cover.id = 0;
    gameLibrary[19].index = 19;
    gameLibrary[19].category = NINTENDO_3D;

    gameLibrary[20].title = "The Legend of Zelda: Twilight Princess";
    gameLibrary[20].coverPath = "./assets/covers/nint_3d/zelda_twilight.png";
    gameLibrary[20].cover.id = 0;
    gameLibrary[20].index = 20;
    gameLibrary[20].category = NINTENDO_3D;

    //Retro Nintendo games
    gameLibrary[21].title = "Donkey Kong Country 2: Diddy's Kong Quest";
    gameLibrary[21].coverPath = "./assets/covers/nint_ret/dk_country_2.png";
    gameLibrary[21].cover.id = 0;
    gameLibrary[21].index = 21;
    gameLibrary[21].category = NINTENDO_RETRO;

    gameLibrary[22].title = "Punch-Out";
    gameLibrary[22].coverPath = "./assets/covers/nint_ret/punch-out.png";
    gameLibrary[22].cover.id = 0;
    gameLibrary[22].index = 22;
    gameLibrary[22].category = NINTENDO_RETRO;

    gameLibrary[23].title = "Super Mario World";
    gameLibrary[23].coverPath = "./assets/covers/nint_ret/super_mario_world.png";
    gameLibrary[23].cover.id = 0;
    gameLibrary[23].index = 23;
    gameLibrary[23].category = NINTENDO_RETRO;

    gameLibrary[24].title = "Super Metroid";
    gameLibrary[24].coverPath = "./assets/covers/nint_ret/super_metroid.png";
    gameLibrary[24].cover.id = 0;
    gameLibrary[24].index = 24;
    gameLibrary[24].category = NINTENDO_RETRO;

    gameLibrary[25].title = "The Legend of Zelda";
    gameLibrary[25].coverPath = "./assets/covers/nint_ret/zelda.png";
    gameLibrary[25].cover.id = 0;
    gameLibrary[25].index = 25;
    gameLibrary[25].category = NINTENDO_RETRO;

    gameLibrary[26].title = "Earth Bound";
    gameLibrary[26].coverPath = "./assets/covers/nint_ret/earth_bound.png";
    gameLibrary[26].cover.id = 0;
    gameLibrary[26].index = 26;
    gameLibrary[26].category = NINTENDO_RETRO;

    gameLibrary[27].title = "Megaman X2";
    gameLibrary[27].coverPath = "./assets/covers/nint_ret/megaman_x2.png";
    gameLibrary[27].cover.id = 0;
    gameLibrary[27].index = 27;
    gameLibrary[27].category = NINTENDO_RETRO;

    //Retro PC & indie games
    gameLibrary[28].title = "Counter Strike 1.6";
    gameLibrary[28].coverPath = "./assets/covers/pc_ind/counter_strike_1.6.png";
    gameLibrary[28].cover.id = 0;
    gameLibrary[28].index = 28;
    gameLibrary[28].category = PC_INDIE;

    gameLibrary[29].title = "Doom";
    gameLibrary[29].coverPath = "./assets/covers/pc_ind/doom_2.png";
    gameLibrary[29].cover.id = 0;
    gameLibrary[29].index = 29;
    gameLibrary[29].category = PC_INDIE;

    gameLibrary[30].title = "Doom 2";
    gameLibrary[30].coverPath = "./assets/covers/pc_ind/doom.png";
    gameLibrary[30].cover.id = 0;
    gameLibrary[30].index = 30;
    gameLibrary[30].category = PC_INDIE;

    gameLibrary[31].title = "Half-Life";
    gameLibrary[31].coverPath = "./assets/covers/pc_ind/half-life.png";
    gameLibrary[31].cover.id = 0;
    gameLibrary[31].index = 31;
    gameLibrary[31].category = PC_INDIE;

    gameLibrary[32].title = "Stardew Valley";
    gameLibrary[32].coverPath = "./assets/covers/pc_ind/stardew_valley.png";
    gameLibrary[32].cover.id = 0;
    gameLibrary[32].index = 32;
    gameLibrary[32].category = PC_INDIE;

    gameLibrary[33].title = "Diablo";
    gameLibrary[33].coverPath = "./assets/covers/pc_ind/diablo.png";
    gameLibrary[33].cover.id = 0;
    gameLibrary[33].index = 33;
    gameLibrary[33].category = PC_INDIE;

    gameLibrary[34].title = "Hollow Knight";
    gameLibrary[34].coverPath = "./assets/covers/pc_ind/hollow_knight.png";
    gameLibrary[34].cover.id = 0;
    gameLibrary[34].index = 34;
    gameLibrary[34].category = PC_INDIE;

    //Sega central games
    gameLibrary[35].title = "Crazy Taxi";
    gameLibrary[35].coverPath = "./assets/covers/sega/crazy_taxi.png";
    gameLibrary[35].cover.id = 0;
    gameLibrary[35].index = 35;
    gameLibrary[35].category = SEGA;

    gameLibrary[36].title = "Fighting Vipers";
    gameLibrary[36].coverPath = "./assets/covers/sega/fighting_vipers.png";
    gameLibrary[36].cover.id = 0;
    gameLibrary[36].index = 36;
    gameLibrary[36].category = SEGA;

    gameLibrary[37].title = "Sonic the Hedgehog 3 & Knuckles";
    gameLibrary[37].coverPath = "./assets/covers/sega/sonic_3_and_knuckles.png";
    gameLibrary[37].cover.id = 0;
    gameLibrary[37].index = 37;
    gameLibrary[37].category = SEGA;

    gameLibrary[38].title = "Sonic CD";
    gameLibrary[38].coverPath = "./assets/covers/sega/sonic_cd.png";
    gameLibrary[38].cover.id = 0;
    gameLibrary[38].index = 38;
    gameLibrary[38].category = SEGA;

    gameLibrary[39].title = "X-Men vs Street Fighter";
    gameLibrary[39].coverPath = "./assets/covers/sega/x-men_vs_street_fighter.png";
    gameLibrary[39].cover.id = 0;
    gameLibrary[39].index = 39;
    gameLibrary[39].category = SEGA;

    gameLibrary[40].title = "Daytona USA";
    gameLibrary[40].coverPath = "./assets/covers/sega/daytona_usa.png";
    gameLibrary[40].cover.id = 0;
    gameLibrary[40].index = 40;
    gameLibrary[40].category = SEGA;

    gameLibrary[41].title = "Gunstar Heroes";
    gameLibrary[41].coverPath = "./assets/covers/sega/gunstar_heroes.png";
    gameLibrary[41].cover.id = 0;
    gameLibrary[41].index = 41;
    gameLibrary[41].category = SEGA;

    //Sony Playstation games
    gameLibrary[42].title = "Megaman X4";
    gameLibrary[42].coverPath = "./assets/covers/playstation/megaman_x4.png";
    gameLibrary[42].cover.id = 0;
    gameLibrary[42].index = 42;
    gameLibrary[42].category = PLAYSTATION;

    gameLibrary[43].title = "Pac-Man World";
    gameLibrary[43].coverPath = "./assets/covers/playstation/pac-man_world.png";
    gameLibrary[43].cover.id = 0;
    gameLibrary[43].index = 43;
    gameLibrary[43].category = PLAYSTATION;

    gameLibrary[44].title = "Soulblade";
    gameLibrary[44].coverPath = "./assets/covers/playstation/soulblade.png";
    gameLibrary[44].cover.id = 0;
    gameLibrary[44].index = 44;
    gameLibrary[44].category = PLAYSTATION;

    gameLibrary[45].title = "Spider-Man (2000)";
    gameLibrary[45].coverPath = "./assets/covers/playstation/spider-man.png";
    gameLibrary[45].cover.id = 0;
    gameLibrary[45].index = 45;
    gameLibrary[45].category = PLAYSTATION;

    gameLibrary[46].title = "Street Fighter: Alpha 3";
    gameLibrary[46].coverPath = "./assets/covers/playstation/street_fighter_alpha_3.png";
    gameLibrary[46].cover.id = 0;
    gameLibrary[46].index = 46;
    gameLibrary[46].category = PLAYSTATION;

    gameLibrary[47].title = "Metal Gear Solid";
    gameLibrary[47].coverPath = "./assets/covers/playstation/met_gear_solid.png";
    gameLibrary[47].cover.id = 0;
    gameLibrary[47].index = 47;
    gameLibrary[47].category = PLAYSTATION;

    gameLibrary[48].title = "Twisted Metal 2";
    gameLibrary[48].coverPath = "./assets/covers/playstation/twist_met_2.png";
    gameLibrary[48].cover.id = 0;
    gameLibrary[48].index = 48;
    gameLibrary[48].category = PLAYSTATION;


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
    printf("start:%d\n", start_index);
    for (int i = start_index + 1; i < gamesLen; i++) {
        if (gameLibrary[i].category != categ || i == (gamesLen - 1)) {
            end_index = i - 1;
            break;
        }
    }
    for (int i = 0; i < GAMES_ON_SCREEN + 2; i++) {
        gamesDisplayed[i] = gameLibrary[i + start_index];
    }
}

//Load game cover textures
void Games_LoadTextures() {
    if (texturesLoaded) {
        return;
    }

    for (int i = start_index; i <= end_index; i++) {
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