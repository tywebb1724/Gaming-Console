#ifndef UI_CONFIG_H
#define UI_CONFIG_H


//Macros for drawing boot screen
#pragma region UI_BOOT
//Width and length of logo
#define LOGO_SIZE (Var_GetMonitorHeight() / 8 * 9)
#pragma endregion

//Macros for drawing base of the main screen
#pragma region UI_BASE
//Macros for drawing background
#define BACKGROUND_W Var_GetMonitorWidth()
#define BACKGROUND_H (Var_GetMonitorWidth() / 3.0f * 2.0f)
#define BACKGROUND_ALPHA 0.4f
#define TOP_ALPHA 0.7f
//Start and end of the section
#define START_SECTION_Y (TOP_Y + MeasureTextEx(Var_GetFontBold(), "L", CENTER_CATEG_SIZE, BOTTOM_TXT_SPACE).y / 2 + 2 * BOTTOM_TXT_SPACING_Y) 
#define END_SECTION_Y (TOP_Y - MeasureTextEx(Var_GetFontBold(), "L", CENTER_CATEG_SIZE, BOTTOM_TXT_SPACE).y / 2 - 2 * BOTTOM_TXT_SPACING_Y)
//Thickness of the lines for the base
#define THICKNESS_BASE (Var_GetMonitorHeight() / 500.0f)
#pragma endregion

//Macros for drawing the top section
#pragma region UI_TOP
//Radius of the options button
#define OPTIONS_RADIUS (Var_GetMonitorWidth() / 90.0f)
//Macros for the button
#define OPTIONS_BTN_X (Var_GetMonitorWidth() * 19.0f / 20)
#define OPTIONS_BTN_Y (Var_GetMonitorHeight() / 18.0f)
#define OPTIONS_LINE_START_X (OPTIONS_BTN_X - OPTIONS_RADIUS / 2 )
#define OPTIONS_LINE_END_X (OPTIONS_BTN_X + OPTIONS_RADIUS / 2 )
#define OPTIONS_LINE1_Y (OPTIONS_BTN_Y - OPTIONS_RADIUS * (2.0f / 5))
#define OPTIONS_LINE2_Y (OPTIONS_BTN_Y)
#define OPTIONS_LINE3_Y (OPTIONS_BTN_Y + OPTIONS_RADIUS * (2.0f / 5))
#define OPTIONS_THICKNESS_LINE1 (Var_GetMonitorHeight() / 650.0f)
#define OPTIONS_THICKNESS_LINE2 (Var_GetMonitorHeight() / 550.0f)
#define OPTIONS_THICKNESS_LINE3 (Var_GetMonitorHeight() / 550.0f)
//Macros for the text
#define OPTIONS_TXT_SIZE (Var_GetMonitorWidth() / 90.0f)
#define OPTIONS_TXT_SPACE (Var_GetMonitorWidth() / 700.0f)
#define OPTIONS_TXT "[OPTIONS]"
#define OPTIONS_TXT_X (OPTIONS_BTN_X - optionsSize.x / 2.0f)
#define OPTIONS_TXT_Y (OPTIONS_BTN_Y + OPTIONS_RADIUS + Var_GetMonitorHeight() / 100.0f)
//Bumper coordinates and values
#define BUMPER_L (Var_GetMonitorWidth() / 15.0f)
#define BUMPER_R (Var_GetMonitorWidth() * 14.0f / 15)
#define BUMPER_Y (TOP_Y - BUMPER_H / 2.0f)
#define BUMPER_W (Var_GetMonitorWidth() / 22.0f)
#define BUMPER_H (Var_GetMonitorHeight() / 18.0f)
#define BUMPER_L_X (BUMPER_L - (BUMPER_W / 2.0f))
#define BUMPER_R_X (BUMPER_R - (BUMPER_W / 2.0f))
//Macros for drawing edges of bumpers
#define BUMPER_ROUND 0.5f
#define BUMPER_SEGMENTS 20
//Bumper text macros
#define BUMPER_TXT_SIZE (Var_GetMonitorWidth() / 88.0f)
#define BUMPER_TXT_SPACE (Var_GetMonitorWidth() / 700.0f)
#define BUMPER_R_TXT "[RB/R1]"
#define BUMPER_L_TXT "[LB/L1]"
#define BUMPER_L_TXT_X (BUMPER_L - bumperLSize.x / 2)
#define BUMPER_R_TXT_X (BUMPER_R - bumperRSize.x / 2)
#define BUMPER_TXT_Y (TOP_Y - bumperRSize.y / 2)
//Small logo macros
#define SMALL_LOGO_SIZE (Var_GetMonitorWidth() / 15.0f)
#define SMALL_LOGO_Y (Var_GetMonitorHeight() / 18.0f + Var_GetMonitorHeight() / 125.0f)
//Thickness for top section
#define THICKNESS_TOP_SECTION (Var_GetMonitorHeight() / 500.0f)
#pragma endregion

//Macros for drawing bottom section
#pragma region UI_BOTTOM
//Macros for left and right arrows (LS and RS)
#define LS_LEFT_X (CENTER_X - Var_GetMonitorWidth() / 55.0f)
#define LS_RIGHT_X (CENTER_X + Var_GetMonitorWidth() / 55.0f)
#define LS_Y (CENTER_GAME_Y + CENTER_GAME_H + Var_GetMonitorHeight() / 23.0f)
#define LS_TRIANGLE_SIZE (Var_GetMonitorWidth() / 80.0f)
#define LS_RADIUS (Var_GetMonitorWidth() / 90.0f)
#define LS_OFFSET (Var_GetMonitorWidth() / 700.0f)
//Drawing bottom rectangle and text
#define BOTTOM_TXT_SIZE (Var_GetMonitorWidth() / 72.0f)
#define BOTTOM_TXT_SPACE (Var_GetMonitorWidth() / 750.0f)
#define BOTTOM_TXT_SPACING_Y (Var_GetMonitorHeight() / 120.0f)
#define BOTTOM_ROUND 0.1f
#define BOTTOM_SEGMENTS 25
#define GAME_TXT_Y (Var_GetMonitorHeight() * (7.0f / 8))
#define BOTTOM_TXT "Press [A/X] to play"
#define CONSOLE_TXT_Y (GAME_TXT_Y + gameSize.y + BOTTOM_TXT_SPACING_Y)
#define BOTTOM_TXT_X (Var_GetMonitorWidth() / 40.0f)
#define BOTTOM_TXT_Y (CONSOLE_TXT_Y + gameSize.y + BOTTOM_TXT_SPACING_Y)
#define BOTTOM_RECT_W (maxLen + 4 * BOTTOM_TXT_SPACING_Y)
#define BOTTOM_RECT_H (bottomSize.y * 3 + 4 * BOTTOM_TXT_SPACING_Y)
#define BOTTOM_RECT_X (BOTTOM_TXT_X - 2 * BOTTOM_TXT_SPACING_Y)
#define BOTTOM_RECT_Y (GAME_TXT_Y - BOTTOM_TXT_SPACING_Y)
#define THICKNESS_BOTTOM (Var_GetMonitorHeight() / 500.0f)
#pragma endregion

//Macros for displaying diagnostics
#pragma region UI_DISP_DIAG
#define DISP_DIAG_SIZE (Var_GetMonitorWidth() / 100.0f)
#define DISP_DIAG_SPACE (Var_GetMonitorWidth() / 800.0f)
#define DISP_DIAG_RES_X (DISP_DIAG_FPS_X)
#define DISP_DIAG_RES_Y (Var_GetMonitorHeight() / 70.0f)
#define DISP_DIAG_FPS_X (Var_GetMonitorWidth() / 70.0f)
#define DISP_DIAG_FPS_Y (DISP_DIAG_RES_Y + Var_GetMonitorHeight() / 40.0f)
#pragma endregion

//Macros for game drawing and scrolling
#pragma region UI_GAME
//Thickness for game outlines
#define THICKNESS_GAME_SELECT (Var_GetMonitorWidth() / 200.0f)
#define THICKNESS_GAME_OTHER (Var_GetMonitorWidth() / 400.0f)
//Y coordinate for game cover
#define MENU_IMG_Y_CENTER (Var_GetMonitorHeight() * (11.0f / 20))
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
#define CENTER_GAME_W (Var_GetMonitorWidth() * (2.0f / 8))
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

//Macros for category drawing and scrolling
#pragma region UI_CATEG
//Macros for drawing static categories
#define TOP_Y (Var_GetMonitorHeight() * (1.0f / 7))
#define CATEG_SPACE (Var_GetMonitorWidth() / 750.0f)
#define CENTER_CATEG_SIZE (Var_GetMonitorWidth() / 38.0f)
#define SIDE1_CATEG_SIZE (CENTER_CATEG_SIZE * (3.0f / 4))
//#define CATEG_Y (TOP_Y - size.y / 2)
#define CENTER_CATEG_Y (TOP_Y - MeasureTextEx(Var_GetFontBold(), "L", CENTER_CATEG_SIZE, CATEG_SPACE).y / 2)
#define CENTER_CATEG_X (UI_CenterText_X(Categories_GetDisplayed(2), CENTER_CATEG_SIZE, CENTER_X))
#define LEFT1_CATEG (Var_GetMonitorWidth() * (1.0f / 4))
#define RIGHT1_CATEG (Var_GetMonitorWidth() * (3.0f / 4))
#define LEFT1_CATEG_X (UI_CenterText_X(Categories_GetDisplayed(1), SIDE1_CATEG_SIZE, LEFT1_CATEG))
#define RIGHT1_CATEG_X (UI_CenterText_X(Categories_GetDisplayed(3), SIDE1_CATEG_SIZE, RIGHT1_CATEG))
#define SIDE_CATEG_Y (TOP_Y - MeasureTextEx(Var_GetFontBold(), "L", SIDE1_CATEG_SIZE, CATEG_SPACE).y / 2)
//Macros for scrolling categories
#define SCROLL_CATEG_IN_SPEED 0.15f
#define SCROLL_CATEG_OUT_SPEED 0.225f
#define SCROLL_CATEG_THRESHOLD (Var_GetMonitorWidth() / 80.0f)
#define LEFT2_SCROLLR (LEFT1_SCROLLR - (SIDE2_GAME_W * (19 / 20.0f)))
#define LEFT1_SCROLLR (CENTER_SCROLLR - (SIDE1_GAME_W * (19 / 20.0f)))
#define CENTER_SCROLLR (- Var_GetMonitorWidth() * (1.0f / 4))
#define RIGHT1_SCROLLR (CENTER_SCROLLR + (SIDE1_GAME_W * (19 / 20.0f)))
#define RIGHT2_SCROLLR (RIGHT1_SCROLLR + (SIDE2_GAME_W * (19 / 20.0f)))
#define LEFT2_SCROLLR_X (UI_CenterImg_X(SIDE2_GAME_W, LEFT2_SCROLLR))
#define LEFT1_SCROLLR_X (UI_CenterImg_X(SIDE1_GAME_W, LEFT1_SCROLLR))
#define CENTER_SCROLLR_X (UI_CenterImg_X(CENTER_GAME_W, CENTER_SCROLLR))
#define RIGHT1_SCROLLR_X (UI_CenterImg_X(SIDE1_GAME_W, RIGHT1_SCROLLR))
#define RIGHT2_SCROLLR_X (UI_CenterImg_X(SIDE2_GAME_W, RIGHT2_SCROLLR))
#define LEFT2_SCROLLL (LEFT1_SCROLLL - (SIDE2_GAME_W * (19 / 20.0f)))
#define LEFT1_SCROLLL (CENTER_SCROLLL - (SIDE1_GAME_W * (19 / 20.0f)))
#define CENTER_SCROLLL (Var_GetMonitorWidth() * (5.0f / 4))
#define RIGHT1_SCROLLL (CENTER_SCROLLL + (SIDE1_GAME_W * (19 / 20.0f)))
#define RIGHT2_SCROLLL (RIGHT1_SCROLLL + (SIDE2_GAME_W * (19 / 20.0f)))
#define LEFT2_SCROLLL_X (UI_CenterImg_X(SIDE2_GAME_W, LEFT2_SCROLLL))
#define LEFT1_SCROLLL_X (UI_CenterImg_X(SIDE1_GAME_W, LEFT1_SCROLLL))
#define CENTER_SCROLLL_X (UI_CenterImg_X(CENTER_GAME_W, CENTER_SCROLLL))
#define RIGHT1_SCROLLL_X (UI_CenterImg_X(SIDE1_GAME_W, RIGHT1_SCROLLL))
#define RIGHT2_SCROLLL_X (UI_CenterImg_X(SIDE2_GAME_W, RIGHT2_SCROLLL))
#pragma endregion

//Macros for drawing launching screen
#pragma region UI_LAUNCH
#define LAUNCH_TXT_SIZE (Var_GetMonitorWidth() / 30.0f)
#define LAUNCH_TXT "Launching:"
#define LAUNCH_SPACE (Var_GetMonitorWidth() / 400.0f)
#define LAUNCH_TITLE_X (CENTER_X - launchTxt_Size.x / 2)
#define LAUNCH_TITLE_Y (Var_GetMonitorHeight() / 8.0f)
#define LAUNCH_GAME_X (CENTER_X - gameTxt_Size.x / 2)
#define LAUNCH_GAME_Y (LAUNCH_TITLE_Y + launchTxt_Size.y + LAUNCH_SPACE * 10)
#define LAUNCH_HOME "Press [HOME] to pause emulator"
#define LAUNCH_HOME_X (CENTER_X - homeTxt_Size.x / 2)
#define LAUNCH_HOME_Y (Var_GetMonitorHeight() / 2.0f)
#pragma endregion



//Y coordinate for bottom section
//#define BOTTOM_Y (Var_GetMonitorHeight() * (7.0f / 8))

#endif