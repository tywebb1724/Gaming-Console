#pragma region UI_GENERAL
//General setup definitions
#define FPS 60
#define BACKGROUND_CLR BLACK
//(Color){ 60, 64, 72, 255 }
#define SCREEN_W 1600.0f
#define SCREEN_H 900.0f
#define CENTER_X (SCREEN_W / 2)
#define RIGHT 1
#define LEFT 0
#define MENU_IMG_Y_CENTER (SCREEN_H * (1.0f / 2))

#pragma endregion

#pragma region UI_BOOT
#define LOGO_SIZE (SCREEN_H)
#pragma endregion

#pragma region UI_TOP_TXT
//Definitions for the company name and instructions at the top of the screen
#define COMPANY_NAME "SP1DER GAMES"
#define COMPANY_TXT_SIZE (SCREEN_W / 40.0f)
#define COMPANY_Y (SCREEN_W / 40.0f)
#define PICK_GAME_TXT "PICK A GAME"
#define PICK_GAME_TXT_SIZE (SCREEN_W / 50.0f)
#define PICK_GAME_Y (SCREEN_W / 20.0f)
#pragma endregion

#pragma region UI_GAME
//Game screen coordinates
#define LEFT2_GAME (LEFT1_GAME - (SIDE2_GAME_W * (19 / 20.0f)))
#define LEFT1_GAME (CENTER_X - (SIDE1_GAME_W * (19 / 20.0f)))
#define RIGHT1_GAME (CENTER_X + (SIDE1_GAME_W * (19 / 20.0f)))
#define RIGHT2_GAME (RIGHT1_GAME + (SIDE2_GAME_W * (19 / 20.0f)))
//Game screen constants
#define GAME_ASPECT_RATIO (3.0f / 4)
//Center game coordinates
#define CENTER_GAME_W (SCREEN_W * (2.0f / 8))
#define CENTER_GAME_H ((1 / GAME_ASPECT_RATIO) * CENTER_GAME_W)
#define CENTER_GAME_Y (UI_CenterImg_Y(CENTER_GAME_H, MENU_IMG_Y_CENTER))
#define CENTER_GAME_X (UI_CenterImg_X(CENTER_GAME_W, CENTER_X))
//First side games coordiantes
#define SIDE1_GAME_W (CENTER_GAME_W * (4 / 5.0f))
#define SIDE1_GAME_H ((1 / GAME_ASPECT_RATIO) * SIDE1_GAME_W)
#define SIDE1_GAME_Y (UI_CenterImg_Y(SIDE1_GAME_H, MENU_IMG_Y_CENTER))
#define LEFT1_GAME_X (UI_CenterImg_X(SIDE1_GAME_W, LEFT1_GAME))
#define RIGHT1_GAME_X (UI_CenterImg_X(SIDE1_GAME_W, RIGHT1_GAME))
//Second side games coordinates
#define SIDE2_GAME_W (CENTER_GAME_W * (3 / 5.0f))
#define SIDE2_GAME_H ((1 / GAME_ASPECT_RATIO) * SIDE2_GAME_W)
#define SIDE2_GAME_Y (UI_CenterImg_Y(SIDE2_GAME_H, MENU_IMG_Y_CENTER))
#define LEFT2_GAME_X (UI_CenterImg_X(SIDE2_GAME_W, LEFT2_GAME))
#define RIGHT2_GAME_X (UI_CenterImg_X(SIDE2_GAME_W, RIGHT2_GAME))
//Third side games coordinates
#define SIDE3_GAME_W (CENTER_GAME_W * (2 / 5.0f))
#define SIDE3_GAME_H ((1 / GAME_ASPECT_RATIO) * SIDE3_GAME_W)
#define SIDE3_GAME_Y (UI_CenterImg_Y(SIDE3_GAME_H, MENU_IMG_Y_CENTER))
#define LEFT3_GAME_X (UI_CenterImg_X(SIDE3_GAME_W, (LEFT1_GAME + LEFT2_GAME) / 2))
#define RIGHT3_GAME_X (UI_CenterImg_X(SIDE3_GAME_W, (RIGHT2_GAME + RIGHT1_GAME) / 2))

#define SCROLL_GAMES_SPEED 0.225f
#define SCROLL_GAMES_THRESHOLD 10.0f
#pragma endregion

#pragma region UI_CATEGORIES
#define TOP_Y (SCREEN_H * (1.0f / 10))
#define CATEG_Y (TOP_Y - size.y / 2)
#define LEFT1_CATEG (SCREEN_W * (1.0f / 4))
#define RIGHT1_CATEG (SCREEN_W * (3.0f / 4))

#define CENTER_CATEG_SIZE (SCREEN_W / 38.0f)
#define SIDE1_CATEG_SIZE (CENTER_CATEG_SIZE * (3.0f / 4))

#define CENTER_CATEG_X (CENTER_X - size.x / 2)

#define LEFT1_CATEG_X (UI_CenterText_X(categoriesDisplayed[1].name, SIDE1_CATEG_SIZE, LEFT1_CATEG))
#define RIGHT1_CATEG_X (UI_CenterText_X(categoriesDisplayed[1].name, SIDE1_CATEG_SIZE, RIGHT1_CATEG))
#pragma endregion

#pragma region UI_BOTTOM
#define BOTTOM_Y (SCREEN_H * (7.0f / 8))

//Selected game outline constants
#define THICKNESS_SELECT_GAME (SCREEN_W / 200.0f)
#define THICKNESS_OTHER (SCREEN_W / 400.0f)
#define THICKNESS_SELECT_TXT (SCREEN_W / 400.0f)

#define BOTTOM_TXT_SIZE (SCREEN_W / 60.0f)
#define BOTTOM_TXT_SPACE BUMPER_TXT_SPACE
#define BOTTOM_TXT_SPACING_Y (SCREEN_H / 100.0f)

#define GAME_TXT_Y (SCREEN_H * (7.0f / 8))
#define BOTTOM_TXT "Press [A/X] to play   |   Press [OPTIONS] for options"
#define BOTTOM_TXT_X (SCREEN_W / 30.0f)
#define BOTTOM_TXT_Y (GAME_TXT_Y + gameText_Size.y + BOTTOM_TXT_SPACING_Y)

#define SELECT_TXT_Y (SCREEN_H * (19.0f / 20))

#define BOTTOM_RECT_W (bottomText_Size.x + 4 * BOTTOM_TXT_SPACING_Y)
#define BOTTOM_RECT_H (bottomText_Size.y + gameText_Size.y + 3 * BOTTOM_TXT_SPACING_Y)
#define BOTTOM_RECT_X (BOTTOM_TXT_X - 2 * BOTTOM_TXT_SPACING_Y)
#define BOTTOM_RECT_Y (GAME_TXT_Y - BOTTOM_TXT_SPACING_Y)

#pragma endregion

#pragma region UI_CATEGORIES_SCROLL
#define SCROLL_CATEG_IN_SPEED 0.15f
#define SCROLL_CATEG_OUT_SPEED 0.225f
#define SCROLL_CATEG_THRESHOLD (SCREEN_W / 80.0f)

#define LEFT2_SCROLLR (LEFT1_SCROLLR - (SIDE2_GAME_W * (19 / 20.0f)))
#define LEFT1_SCROLLR (CENTER_SCROLLR - (SIDE1_GAME_W * (19 / 20.0f)))
#define CENTER_SCROLLR (- SCREEN_W * (1.0f / 4))
#define RIGHT1_SCROLLR (CENTER_SCROLLR + (SIDE1_GAME_W * (19 / 20.0f)))
#define RIGHT2_SCROLLR (RIGHT1_SCROLLR + (SIDE2_GAME_W * (19 / 20.0f)))

#define LEFT2_SCROLLR_X (UI_CenterImg_X(SIDE2_GAME_W, LEFT2_SCROLLR))
#define LEFT1_SCROLLR_X (UI_CenterImg_X(SIDE1_GAME_W, LEFT1_SCROLLR))
#define CENTER_SCROLLR_X (UI_CenterImg_X(CENTER_GAME_W, CENTER_SCROLLR))
#define RIGHT1_SCROLLR_X (UI_CenterImg_X(SIDE1_GAME_W, RIGHT1_SCROLLR))
#define RIGHT2_SCROLLR_X (UI_CenterImg_X(SIDE2_GAME_W, RIGHT2_SCROLLR))

#define LEFT2_SCROLLL (LEFT1_SCROLLL - (SIDE2_GAME_W * (19 / 20.0f)))
#define LEFT1_SCROLLL (CENTER_SCROLLL - (SIDE1_GAME_W * (19 / 20.0f)))
#define CENTER_SCROLLL (SCREEN_W * (5.0f / 4))
#define RIGHT1_SCROLLL (CENTER_SCROLLL + (SIDE1_GAME_W * (19 / 20.0f)))
#define RIGHT2_SCROLLL (RIGHT1_SCROLLL + (SIDE2_GAME_W * (19 / 20.0f)))

#define LEFT2_SCROLLL_X (UI_CenterImg_X(SIDE2_GAME_W, LEFT2_SCROLLL))
#define LEFT1_SCROLLL_X (UI_CenterImg_X(SIDE1_GAME_W, LEFT1_SCROLLL))
#define CENTER_SCROLLL_X (UI_CenterImg_X(CENTER_GAME_W, CENTER_SCROLLL))
#define RIGHT1_SCROLLL_X (UI_CenterImg_X(SIDE1_GAME_W, RIGHT1_SCROLLL))
#define RIGHT2_SCROLLL_X (UI_CenterImg_X(SIDE2_GAME_W, RIGHT2_SCROLLL))
#pragma endregion

#pragma region UI_BUMPERS

#define BUMPER_L (SCREEN_W / 20.0f)
#define BUMPER_R (SCREEN_W * 19.0f / 20)
#define BUMPER_Y (TOP_Y - BUMPER_H / 2.0f)
#define BUMPER_W (SCREEN_W / 15.0f)
#define BUMPER_H (SCREEN_H / 15.0f)

#define BUMPER_L_X (BUMPER_L - (BUMPER_W / 2.0f))
#define BUMPER_R_X (BUMPER_R - (BUMPER_W / 2.0f))

#define BUMPER_ROUND 0.5f
#define BUMPER_SEGMENTS 20

#define BUMPER_TXT_SIZE (SCREEN_W / 75.0f)
#define BUMPER_TXT_SPACE (SCREEN_W / 400.0f)
#define BUMPER_R_TXT "[RB/R1]"
#define BUMPER_L_TXT "[LB/L1]"
#define BUMPER_L_TXT_X (BUMPER_L - bumperL_Size.x / 2)
#define BUMPER_R_TXT_X (BUMPER_R - bumperR_Size.x / 2)
#define BUMPER_TXT_Y (TOP_Y - bumperR_Size.y / 2)
#pragma endregion