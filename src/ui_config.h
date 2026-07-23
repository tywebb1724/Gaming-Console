
//Macros for drawing boot screen
#pragma region UI_BOOT
//Width and length of logo
#define LOGO_SIZE (SCREEN_H / 8 * 9)
#pragma endregion

//Macros for game drawing and scrolling
#pragma region UI_GAME
//Y coordinate for game cover
#define MENU_IMG_Y_CENTER (SCREEN_H * (11.0f / 20))
//Game screen constants
#define GAME_ASPECT_RATIO (3.0f / 4)
//Macros for scrolling animation
#define SCROLL_GAMES_SPEED 0.225f
#define SCROLL_GAMES_THRESHOLD 10.0f
//Game screen coordinates
#define LEFT2_GAME (LEFT1_GAME - (SIDE2_GAME_W * (19 / 20.0f)))
#define LEFT1_GAME (CENTER_X - (SIDE1_GAME_W * (19 / 20.0f)))
#define RIGHT1_GAME (CENTER_X + (SIDE1_GAME_W * (19 / 20.0f)))
#define RIGHT2_GAME (RIGHT1_GAME + (SIDE2_GAME_W * (19 / 20.0f)))
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
#pragma endregion

//Macros for drawing the top section
#pragma region UI_TOP
//Radius of the options button
#define OPTIONS_RADIUS (SCREEN_W / 90.0f)
//Macros for the button
#define OPTIONS_BTN_X (SCREEN_W * 19.0f / 20)
#define OPTIONS_BTN_Y (SCREEN_H / 18.0f)
#define OPTIONS_LINE_START_X (OPTIONS_BTN_X - OPTIONS_RADIUS / 2 )
#define OPTIONS_LINE_END_X (OPTIONS_BTN_X + OPTIONS_RADIUS / 2 )
#define OPTIONS_LINE1_Y (OPTIONS_BTN_Y - OPTIONS_RADIUS * (2.0f / 5))
#define OPTIONS_LINE2_Y (OPTIONS_BTN_Y)
#define OPTIONS_LINE3_Y (OPTIONS_BTN_Y + OPTIONS_RADIUS * (2.0f / 5))
#define OPTIONS_THICKNESS_LINE1 (SCREEN_H / 650.0f)
#define OPTIONS_THICKNESS_LINE2 (SCREEN_H / 550.0f)
#define OPTIONS_THICKNESS_LINE3 (SCREEN_H / 550.0f)
//Macros for the text
#define OPTIONS_TXT_SIZE (SCREEN_W / 90.0f)
#define OPTIONS_TXT_SPACE (SCREEN_W / 700.0f)
#define OPTIONS_TXT "[OPTIONS]"
#define OPTIONS_TXT_X (OPTIONS_BTN_X - optionsSize.x / 2.0f)
#define OPTIONS_TXT_Y (OPTIONS_BTN_Y + OPTIONS_RADIUS + SCREEN_H / 100.0f)
//Bumper coordinates and values
#define BUMPER_L (SCREEN_W / 15.0f)
#define BUMPER_R (SCREEN_W * 14.0f / 15)
#define BUMPER_Y (TOP_Y - BUMPER_H / 2.0f)
#define BUMPER_W (SCREEN_W / 22.0f)
#define BUMPER_H (SCREEN_H / 18.0f)
#define BUMPER_L_X (BUMPER_L - (BUMPER_W / 2.0f))
#define BUMPER_R_X (BUMPER_R - (BUMPER_W / 2.0f))
//Macros for drawing edges of bumpers
#define BUMPER_ROUND 0.5f
#define BUMPER_SEGMENTS 20
//Bumper text macros
#define BUMPER_TXT_SIZE (SCREEN_W / 88.0f)
#define BUMPER_TXT_SPACE (SCREEN_W / 700.0f)
#define BUMPER_R_TXT "[RB/R1]"
#define BUMPER_L_TXT "[LB/L1]"
#define BUMPER_L_TXT_X (BUMPER_L - bumperL_Size.x / 2)
#define BUMPER_R_TXT_X (BUMPER_R - bumperR_Size.x / 2)
#define BUMPER_TXT_Y (TOP_Y - bumperR_Size.y / 2)
//Small logo macros
#define SMALL_LOGO_SIZE (SCREEN_W / 15.0f)
#define SMALL_LOGO_Y (SCREEN_H / 18.0f + SCREEN_H / 125.0f)
//Draw lines for the start and end of the top section
#define START_SECTION_Y (TOP_Y + MeasureTextEx(fontBold, "L", CENTER_CATEG_SIZE, BOTTOM_TXT_SPACE).y / 2 + 2 * BOTTOM_TXT_SPACING_Y) 
#define END_SECTION_Y (CENTER_CATEG_Y - 2 * BOTTOM_TXT_SPACING_Y)
#define THICKNESS_TOP_SECTION (SCREEN_H / 500.0f)
#pragma endregion

//Macros for drawing bottom section
#pragma region UI_BOTTOM
//Macros for left and right arrows (LS and RS)
#define LS_LEFT_X (CENTER_X - SCREEN_W / 55.0f)
#define LS_RIGHT_X (CENTER_X + SCREEN_W / 55.0f)
#define LS_Y (CENTER_GAME_Y + CENTER_GAME_H + SCREEN_H / 23.0f)
#define LS_TRIANGLE_SIZE (SCREEN_W / 80.0f)
#define LS_RADIUS (SCREEN_W / 90.0f)
#define LS_OFFSET (SCREEN_W / 700.0f)
//Drawing bottom rectangle and text
#define BOTTOM_TXT_SIZE (SCREEN_W / 72.0f)
#define BOTTOM_TXT_SPACE (SCREEN_W / 750.0f)
#define BOTTOM_TXT_SPACING_Y (SCREEN_H / 120.0f)
#define BOTTOM_ROUND 0.1f
#define BOTTOM_SEGMENTS 25
#define GAME_TXT_Y (SCREEN_H * (7.0f / 8))
#define BOTTOM_TXT "Press [A/X] to play"
#define CONSOLE_TXT_Y (GAME_TXT_Y + gameSize.y + BOTTOM_TXT_SPACING_Y)
#define BOTTOM_TXT_X (SCREEN_W / 40.0f)
#define BOTTOM_TXT_Y (CONSOLE_TXT_Y + gameSize.y + BOTTOM_TXT_SPACING_Y)
#define BOTTOM_RECT_W (maxLen + 4 * BOTTOM_TXT_SPACING_Y)
#define BOTTOM_RECT_H (bottomSize.y * 3 + 4 * BOTTOM_TXT_SPACING_Y)
#define BOTTOM_RECT_X (BOTTOM_TXT_X - 2 * BOTTOM_TXT_SPACING_Y)
#define BOTTOM_RECT_Y (GAME_TXT_Y - BOTTOM_TXT_SPACING_Y)
#define THICKNESS_BOTTOM (SCREEN_H / 500.0f)
#pragma endregion

//Macros for drawing launching screen
#pragma region LAUNCH
#define LAUNCH_TXT_SIZE (SCREEN_W / 30.0f)
#define LAUNCH_TXT "Launching:"
#define LAUNCH_SPACE (SCREEN_W / 400.0f)
#define LAUNCH_TITLE_X (CENTER_X - launchTxt_Size.x / 2)
#define LAUNCH_TITLE_Y (SCREEN_H / 8.0f)
#define LAUNCH_GAME_X (CENTER_X - gameTxt_Size.x / 2)
#define LAUNCH_GAME_Y (LAUNCH_TITLE_Y + launchTxt_Size.y + LAUNCH_SPACE * 10)
#define LAUNCH_HOME "Press [HOME] to pause emulator"
#define LAUNCH_HOME_X (CENTER_X - homeTxt_Size.x / 2)
#define LAUNCH_HOME_Y (SCREEN_H / 2.0f)
#pragma endregion


/*
//Y coordinate for bottom section
#define BOTTOM_Y (SCREEN_H * (7.0f / 8))
//Selected game outline constants
#define THICKNESS_SELECT_GAME (SCREEN_W / 200.0f)
#define THICKNESS_OTHER (SCREEN_W / 400.0f)
#define THICKNESS_SELECT_TXT (SCREEN_W / 400.0f)
*/