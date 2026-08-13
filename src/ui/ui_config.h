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
#define BUMPER_R_TXT "[RB/D]"
#define BUMPER_L_TXT "[LB/A]"
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
#define BOTTOM_TXT "[A/ENTER] Play | [MENU/P] Erase game data"
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
#define DISP_DIAG_FRAME_X (Var_GetMonitorWidth() / 15.0f)
#define DISP_DIAG_FRAME_Y (DISP_DIAG_FPS_Y)
#define DISP_DIAG_TEMP_X (Var_GetMonitorWidth() / 70.0f)
#define DISP_DIAG_TEMP_Y (DISP_DIAG_FPS_Y + Var_GetMonitorHeight() / 40.0f)
#define DISP_DIAG_CLOCK_X (DISP_DIAG_FPS_X)
#define DISP_DIAG_CLOCK_Y (DISP_DIAG_TEMP_Y + Var_GetMonitorHeight() / 40.0f)
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
#define FRAME_LERP(rate) (1.0f - powf(1.0f - (rate), GetFrameTime() * 60.0f))
#define SCROLL_GAMES_SPEED FRAME_LERP(0.225f)
#define SCROLL_GAMES_THRESHOLD (Var_GetMonitorWidth() / 192.0f)
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
#define TOP_Y (Var_GetMonitorHeight() / 6.0f)
#define CATEG_SPACE (Var_GetMonitorWidth() / 750.0f)
#define CENTER_CATEG_SIZE (Var_GetMonitorWidth() / 38.0f)
#define SIDE1_CATEG_SIZE (CENTER_CATEG_SIZE * (3.0f / 4))
//#define CATEG_Y (TOP_Y - size.y / 2)
#define CENTER_CATEG_Y (TOP_Y - MeasureTextEx(Var_GetFontBold(), "L", CENTER_CATEG_SIZE, CATEG_SPACE).y / 2)
#define CENTER_CATEG_X (CENTER_X - MeasureTextEx(Var_GetFontBold(), Categories_GetDisplayed(2), CENTER_CATEG_SIZE, CATEG_SPACE).x / 2)
#define LEFT1_CATEG (Var_GetMonitorWidth() * (1.0f / 4))
#define RIGHT1_CATEG (Var_GetMonitorWidth() * (3.0f / 4))
#define LEFT1_CATEG_X (LEFT1_CATEG - MeasureTextEx(Var_GetFontBold(), Categories_GetDisplayed(1), SIDE1_CATEG_SIZE, CATEG_SPACE).x / 2)
#define RIGHT1_CATEG_X (RIGHT1_CATEG - MeasureTextEx(Var_GetFontBold(), Categories_GetDisplayed(3), SIDE1_CATEG_SIZE, CATEG_SPACE).x / 2)
#define SIDE_CATEG_Y (TOP_Y - MeasureTextEx(Var_GetFontBold(), "L", SIDE1_CATEG_SIZE, CATEG_SPACE).y / 2)
//Macros for scrolling categories
#define SCROLL_CATEG_IN_SPEED FRAME_LERP(0.15f)
#define SCROLL_CATEG_OUT_SPEED FRAME_LERP(0.225f)
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
//Launching text
#define LAUNCH_TXT_SIZE (Var_GetMonitorWidth() / 25.0f)
#define LAUNCH_TXT "Launching:"
#define LAUNCH_SPACE (Var_GetMonitorWidth() / 400.0f)
#define LAUNCH_TITLE_X (CENTER_X - launchTxt_Size.x / 2)
#define LAUNCH_TITLE_Y (Var_GetMonitorHeight() / 20.0f)
#define LAUNCH_GAME_X (CENTER_X - gameTxt_Size.x / 2)
#define LAUNCH_GAME_Y (LAUNCH_TITLE_Y + launchTxt_Size.y + LAUNCH_SPACE * 5)
#define LAUNCH_HOME_LIB "Press [HOME/ESC] to pause emulator"
#define LAUNCH_HOME_EXT "Press [HOME] to close game (may need multiple presses)"
#define LAUNCH_SMALL_SIZE (Var_GetMonitorWidth() / 35.0f)
#define LAUNCH_HOME_X (CENTER_X - homeTxt_Size.x / 2)
#define LAUNCH_HOME_Y (LAUNCH_GAME_Y + gameTxt_Size.y + LAUNCH_SPACE * 5)
//Rectangles for controller and keyboard controls
#define LAUNCH_CONTROLLER_X (Var_GetMonitorWidth() / 20)
#define LAUNCH_CONTROLLER_Y (Var_GetMonitorHeight() / 3)
#define LAUNCH_CONTROLLER_W (Var_GetMonitorWidth() / 20 * 9)
#define LAUNCH_CONTROLLER_H (Var_GetMonitorHeight() / 5 * 3)
#define LAUNCH_KEYBOARD_X (Var_GetMonitorWidth() / 2)
#define LAUNCH_KEYBOARD_Y (LAUNCH_CONTROLLER_Y)
#define LAUNCH_KEYBOARD_W (LAUNCH_CONTROLLER_W)
#define LAUNCH_KEYBOARD_H (LAUNCH_CONTROLLER_H)
//Drawing controller
//Shared macros for the controls
#define LAUNCH_CONTROL_SIZE (Var_GetMonitorWidth() / 85.0f)
#define LAUNCH_CONTROL_SIZE_DOOM1 (Var_GetMonitorWidth() / 120.0f)
#define LAUNCH_CONTROL_SIZE_DOOM2 (Var_GetMonitorWidth() / 190.0f)
#define LAUNCH_CONTROL_SPACE (Var_GetMonitorWidth() / 2000.0f)
#define LAUNCH_CONTROL_RADIUS (Var_GetMonitorWidth() / 83.0f)
//Right face buttons
#define LAUNCH_RIGHT_FACE_DOWN_X (Var_GetMonitorWidth() / 1600 * 731)
#define LAUNCH_RIGHT_FACE_DOWN_Y (Var_GetMonitorHeight() / 800 * 492)
#define LAUNCH_RIGHT_FACE_DOWN_TXT_X (LAUNCH_RIGHT_FACE_DOWN_X - rDown_Size.x / 2)
#define LAUNCH_RIGHT_FACE_DOWN_TXT_Y (LAUNCH_RIGHT_FACE_DOWN_Y - rDown_Size.y / 2)
#define LAUNCH_RIGHT_FACE_RIGHT_X (Var_GetMonitorWidth() / 1600 * 769)
#define LAUNCH_RIGHT_FACE_RIGHT_Y (Var_GetMonitorHeight() / 800 * 456)
#define LAUNCH_RIGHT_FACE_RIGHT_TXT_X (LAUNCH_RIGHT_FACE_RIGHT_X - rRight_Size.x / 2)
#define LAUNCH_RIGHT_FACE_RIGHT_TXT_Y (LAUNCH_RIGHT_FACE_RIGHT_Y - rRight_Size.y / 2)
#define LAUNCH_RIGHT_FACE_UP_X (Var_GetMonitorWidth() / 1600 * 731)
#define LAUNCH_RIGHT_FACE_UP_Y (Var_GetMonitorHeight() / 800 * 421)
#define LAUNCH_RIGHT_FACE_UP_TXT_X (LAUNCH_RIGHT_FACE_UP_X - rUp_Size.x / 2)
#define LAUNCH_RIGHT_FACE_UP_TXT_Y (LAUNCH_RIGHT_FACE_UP_Y - rUp_Size.y / 2)
#define LAUNCH_RIGHT_FACE_LEFT_X (Var_GetMonitorWidth() / 1600 * 691)
#define LAUNCH_RIGHT_FACE_LEFT_Y (Var_GetMonitorHeight() / 800 * 456)
#define LAUNCH_RIGHT_FACE_LEFT_TXT_X (LAUNCH_RIGHT_FACE_LEFT_X - rLeft_Size.x / 2)
#define LAUNCH_RIGHT_FACE_LEFT_TXT_Y (LAUNCH_RIGHT_FACE_LEFT_Y - rLeft_Size.y / 2)
//Middle buttons
#define LAUNCH_HOME_RECT_W (Var_GetMonitorWidth() / 21.0f)
#define LAUNCH_HOME_RECT_H (Var_GetMonitorHeight() / 30.0f)
#define LAUNCH_HOME_RECT_X (Var_GetMonitorWidth() / 800.0f * 195)
#define LAUNCH_HOME_RECT_Y (Var_GetMonitorHeight() / 400.0f * 327)
#define LAUNCH_HOME_TXT_X (LAUNCH_HOME_RECT_X + LAUNCH_HOME_RECT_W / 2 - home_Size.x / 2)
#define LAUNCH_HOME_TXT_Y (LAUNCH_HOME_RECT_Y + LAUNCH_HOME_RECT_H / 2 - home_Size.y / 2)
#define LAUNCH_BACK_RECT_W (Var_GetMonitorWidth() / 22.0f)
#define LAUNCH_BACK_RECT_H (Var_GetMonitorHeight() / 31.0f)
#define LAUNCH_BACK_RECT_X (Var_GetMonitorWidth() / 800.0f * 167)
#define LAUNCH_BACK_RECT_Y (Var_GetMonitorHeight() / 800.0f * 309)
#define LAUNCH_BACK_TXT_X (LAUNCH_BACK_RECT_X + LAUNCH_BACK_RECT_W / 2 - back_Size.x / 2)
#define LAUNCH_BACK_TXT_Y (LAUNCH_BACK_RECT_Y + LAUNCH_BACK_RECT_H / 2 - back_Size.y / 2)
#define LAUNCH_START_RECT_W (Var_GetMonitorWidth() / 22.0f)
#define LAUNCH_START_RECT_H (Var_GetMonitorHeight() / 31.0f)
#define LAUNCH_START_RECT_X (Var_GetMonitorWidth() / 800.0f * 225)
#define LAUNCH_START_RECT_Y (Var_GetMonitorHeight() / 800.0f * 309)
#define LAUNCH_START_TXT_X (LAUNCH_START_RECT_X + LAUNCH_START_RECT_W / 2 - start_Size.x / 2)
#define LAUNCH_START_TXT_Y (LAUNCH_START_RECT_Y + LAUNCH_START_RECT_H / 2 - start_Size.y / 2)
//D-Pad
#define LAUNCH_DPAD_RECT_W (Var_GetMonitorWidth() / 21.0f)
#define LAUNCH_DPAD_RECT_H (Var_GetMonitorHeight() / 30.0f)
#define LAUNCH_DPAD_RECT_X (Var_GetMonitorWidth() / 800.0f * 154)
#define LAUNCH_DPAD_RECT_Y (Var_GetMonitorHeight() / 400.0f * 328)
#define LAUNCH_DPAD_TXT_X (LAUNCH_DPAD_RECT_X + LAUNCH_DPAD_RECT_W / 2 - dPad_Size.x / 2)
#define LAUNCH_DPAD_TXT_Y (LAUNCH_DPAD_RECT_Y + LAUNCH_DPAD_RECT_H / 2 - dPad_Size.y / 2)
//Joysticks
#define LAUNCH_RS_RECT_W (Var_GetMonitorWidth() / 21.0f)
#define LAUNCH_RS_RECT_H (Var_GetMonitorHeight() / 30.0f)
#define LAUNCH_RS_RECT_X (Var_GetMonitorWidth() / 800.0f * 236)
#define LAUNCH_RS_RECT_Y (Var_GetMonitorHeight() / 400.0f * 328)
#define LAUNCH_RS_TXT_X (LAUNCH_RS_RECT_X + LAUNCH_RS_RECT_W / 2 - rs_Size.x / 2)
#define LAUNCH_RS_TXT_Y (LAUNCH_RS_RECT_Y + LAUNCH_RS_RECT_H / 2 - rs_Size.y / 2)
#define LAUNCH_LS_RECT_W (Var_GetMonitorWidth() / 21.0f)
#define LAUNCH_LS_RECT_H (Var_GetMonitorHeight() / 31.0f)
#define LAUNCH_LS_RECT_X (Var_GetMonitorWidth() / 800.0f * 48)
#define LAUNCH_LS_RECT_Y (Var_GetMonitorHeight() / 400.0f * 222)
#define LAUNCH_LS_TXT_X (LAUNCH_LS_RECT_X + LAUNCH_LS_RECT_W / 2 - ls_Size.x / 2)
#define LAUNCH_LS_TXT_Y (LAUNCH_LS_RECT_Y + LAUNCH_LS_RECT_H / 2 - ls_Size.y / 2)
//Triggers
#define LAUNCH_LT_RECT_W (Var_GetMonitorWidth() / 22.0f)
#define LAUNCH_LT_RECT_H (Var_GetMonitorHeight() / 31.0f)
#define LAUNCH_LT_RECT_X (Var_GetMonitorWidth() / 800.0f * 117)
#define LAUNCH_LT_RECT_Y (Var_GetMonitorHeight() / 800.0f * 309)
#define LAUNCH_LT_TXT_X (LAUNCH_LT_RECT_X + LAUNCH_LT_RECT_W / 2 - lt_Size.x / 2)
#define LAUNCH_LT_TXT_Y (LAUNCH_LT_RECT_Y + LAUNCH_LT_RECT_H / 2 - lt_Size.y / 2)
#define LAUNCH_RT_RECT_W (Var_GetMonitorWidth() / 22.0f)
#define LAUNCH_RT_RECT_H (Var_GetMonitorHeight() / 31.0f)
#define LAUNCH_RT_RECT_X (Var_GetMonitorWidth() / 800.0f * 276)
#define LAUNCH_RT_RECT_Y (Var_GetMonitorHeight() / 800.0f * 309)
#define LAUNCH_RT_TXT_X (LAUNCH_RT_RECT_X + LAUNCH_RT_RECT_W / 2 - rt_Size.x / 2)
#define LAUNCH_RT_TXT_Y (LAUNCH_RT_RECT_Y + LAUNCH_RT_RECT_H / 2 - rt_Size.y / 2)
//Bumpers
#define LAUNCH_LB_RECT_W (Var_GetMonitorWidth() / 26.0f)
#define LAUNCH_LB_RECT_H (Var_GetMonitorHeight() / 31.0f)
#define LAUNCH_LB_RECT_X (Var_GetMonitorWidth() / 800.0f * 69)
#define LAUNCH_LB_RECT_Y (Var_GetMonitorHeight() / 400.0f * 188)
#define LAUNCH_LB_TXT_X (LAUNCH_LB_RECT_X + LAUNCH_LB_RECT_W / 2 - lb_Size.x / 2)
#define LAUNCH_LB_TXT_Y (LAUNCH_LB_RECT_Y + LAUNCH_LB_RECT_H / 2 - lb_Size.y / 2)
#define LAUNCH_RB_RECT_W (Var_GetMonitorWidth() / 26.0f)
#define LAUNCH_RB_RECT_H (Var_GetMonitorHeight() / 31.0f)
#define LAUNCH_RB_RECT_X (Var_GetMonitorWidth() / 800.0f * 329)
#define LAUNCH_RB_RECT_Y (Var_GetMonitorHeight() / 400.0f * 188)
#define LAUNCH_RB_TXT_X (LAUNCH_RB_RECT_X + LAUNCH_RB_RECT_W / 2 - rb_Size.x / 2)
#define LAUNCH_RB_TXT_Y (LAUNCH_RB_RECT_Y + LAUNCH_RB_RECT_H / 2 - rb_Size.y / 2)
//Keyboard and mouse
#define LAUNCH_KEY_THICKNESS (Var_GetMonitorHeight() / 500.0f)
#define LAUNCH_ARROW_THICKNESS (Var_GetMonitorHeight() / 540.0f)
#define LAUNCH_KEYBOARD_SPACE_X (Var_GetMonitorWidth() / 80.0f)
#define LAUNCH_KEY_X (LAUNCH_KEYBOARD_X + LAUNCH_KEYBOARD_SPACE_X)
#define LAUNCH_KEY_Y (LAUNCH_KEYBOARD_Y + LAUNCH_KEYBOARD_H / 5 * 2 - LAUNCH_KEY_H / 2)
#define LAUNCH_KEY_W (Var_GetMonitorWidth() / 3.0f)
#define LAUNCH_KEY_H (Var_GetMonitorHeight() / 6.0f)
#define LAUNCH_MOUSE_X (LAUNCH_KEYBOARD_X + LAUNCH_KEYBOARD_W - LAUNCH_KEYBOARD_SPACE_X - LAUNCH_MOUSE_W)
#define LAUNCH_MOUSE_Y (LAUNCH_KEY_Y + LAUNCH_KEY_H / 2 - LAUNCH_MOUSE_H / 2)
#define LAUNCH_MOUSE_W (Var_GetMonitorHeight() / 10.0f)
#define LAUNCH_MOUSE_H (Var_GetMonitorHeight() / 8.0f)
#define LAUNCH_ARROWS_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 20.0f)
#define LAUNCH_ARROWS_Y (LAUNCH_KEYBOARD_Y + LAUNCH_KEYBOARD_H / 9 * 8 - LAUNCH_KEY_H / 2)
#define LAUNCH_ARROWS_W (Var_GetMonitorHeight() / 9.0f)
#define LAUNCH_ARROWS_H (Var_GetMonitorHeight() / 8.0f)

#define LAUNCH_KEY_LINE_END_TOP1 (LAUNCH_KEY_Y - Var_GetMonitorHeight() / 50.0f)
#define LAUNCH_KEY_LINE_END_BOTTOM1 (LAUNCH_KEY_Y + LAUNCH_KEY_H + Var_GetMonitorHeight() / 50.0f)
#define LAUNCH_KEY_LINE_END_TOP2 (LAUNCH_KEY_LINE_END_TOP1 - Var_GetMonitorHeight() / 22.0f)
#define LAUNCH_KEY_LINE_END_BOTTOM2 (LAUNCH_KEY_LINE_END_BOTTOM1 + Var_GetMonitorHeight() / 22.0f)
#define LAUNCH_KEY_REC_W (Var_GetMonitorWidth() / 25.0f)
#define LAUNCH_KEY_REC_H (Var_GetMonitorHeight() / 31.0f)
//1st keyboard line
#define LAUNCH_KEY_TAB_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 25)
#define LAUNCH_KEY_TAB_LINE_Y_START (LAUNCH_KEY_Q_LINE_Y_START)
#define LAUNCH_KEY_TAB_X (LAUNCH_KEY_TAB_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_TAB_Y (LAUNCH_KEY_LINE_END_TOP1 - LAUNCH_KEY_REC_H)
#define LAUNCH_KEY_TAB_TXT_X (LAUNCH_KEY_TAB_LINE_X - tab_Size.x / 2.0f)
#define LAUNCH_KEY_TAB_TXT_Y (LAUNCH_KEY_TAB_Y + LAUNCH_KEY_REC_H / 2.0f - tab_Size.y / 2.0f)

#define LAUNCH_KEY_Q_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 72)
#define LAUNCH_KEY_Q_LINE_Y_START (LAUNCH_KEY_Y + Var_GetMonitorHeight() / 800.0f * 33)
#define LAUNCH_KEY_Q_X (LAUNCH_KEY_Q_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_Q_Y (LAUNCH_KEY_LINE_END_TOP2 - LAUNCH_KEY_REC_H)
#define LAUNCH_KEY_Q_TXT_X (LAUNCH_KEY_Q_LINE_X - q_Size.x / 2.0f)
#define LAUNCH_KEY_Q_TXT_Y (LAUNCH_KEY_Q_Y + LAUNCH_KEY_REC_H / 2.0f - q_Size.y / 2.0f)

#define LAUNCH_KEY_W_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 107)
#define LAUNCH_KEY_W_LINE_Y_START (LAUNCH_KEY_Q_LINE_Y_START)
#define LAUNCH_KEY_W_X (LAUNCH_KEY_W_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_W_Y (LAUNCH_KEY_LINE_END_TOP1 - LAUNCH_KEY_REC_H)
#define LAUNCH_KEY_W_TXT_X (LAUNCH_KEY_W_LINE_X - w_Size.x / 2.0f)
#define LAUNCH_KEY_W_TXT_Y (LAUNCH_KEY_W_Y + LAUNCH_KEY_REC_H / 2.0f - w_Size.y / 2.0f)

#define LAUNCH_KEY_E_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 143)
#define LAUNCH_KEY_E_LINE_Y_START (LAUNCH_KEY_Q_LINE_Y_START)
#define LAUNCH_KEY_E_X (LAUNCH_KEY_E_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_E_Y (LAUNCH_KEY_LINE_END_TOP2 - LAUNCH_KEY_REC_H)
#define LAUNCH_KEY_E_TXT_X (LAUNCH_KEY_E_LINE_X - e_Size.x / 2.0f)
#define LAUNCH_KEY_E_TXT_Y (LAUNCH_KEY_E_Y + LAUNCH_KEY_REC_H / 2.0f - e_Size.y / 2.0f)

#define LAUNCH_KEY_U_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 285)
#define LAUNCH_KEY_U_LINE_Y_START (LAUNCH_KEY_Q_LINE_Y_START)
#define LAUNCH_KEY_U_X (LAUNCH_KEY_U_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_U_Y (LAUNCH_KEY_LINE_END_TOP1 - LAUNCH_KEY_REC_H)
#define LAUNCH_KEY_U_TXT_X (LAUNCH_KEY_U_LINE_X - u_Size.x / 2.0f)
#define LAUNCH_KEY_U_TXT_Y (LAUNCH_KEY_U_Y + LAUNCH_KEY_REC_H / 2.0f - u_Size.y / 2.0f)

#define LAUNCH_KEY_I_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 321)
#define LAUNCH_KEY_I_LINE_Y_START (LAUNCH_KEY_Q_LINE_Y_START)
#define LAUNCH_KEY_I_X (LAUNCH_KEY_I_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_I_Y (LAUNCH_KEY_LINE_END_TOP2 - LAUNCH_KEY_REC_H)
#define LAUNCH_KEY_I_TXT_X (LAUNCH_KEY_I_LINE_X - i_Size.x / 2.0f)
#define LAUNCH_KEY_I_TXT_Y (LAUNCH_KEY_I_Y + LAUNCH_KEY_REC_H / 2.0f - i_Size.y / 2.0f)

#define LAUNCH_KEY_O_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 356)
#define LAUNCH_KEY_O_LINE_Y_START (LAUNCH_KEY_Q_LINE_Y_START)
#define LAUNCH_KEY_O_X (LAUNCH_KEY_O_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_O_Y (LAUNCH_KEY_LINE_END_TOP1 - LAUNCH_KEY_REC_H)
#define LAUNCH_KEY_O_TXT_X (LAUNCH_KEY_O_LINE_X - o_Size.x / 2.0f)
#define LAUNCH_KEY_O_TXT_Y (LAUNCH_KEY_O_Y + LAUNCH_KEY_REC_H / 2.0f - o_Size.y / 2.0f)

#define LAUNCH_KEY_P_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 391)
#define LAUNCH_KEY_P_LINE_Y_START (LAUNCH_KEY_Q_LINE_Y_START)
#define LAUNCH_KEY_P_X (LAUNCH_KEY_P_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_P_Y (LAUNCH_KEY_LINE_END_TOP2 - LAUNCH_KEY_REC_H)
#define LAUNCH_KEY_P_TXT_X (LAUNCH_KEY_P_LINE_X - p_Size.x / 2.0f)
#define LAUNCH_KEY_P_TXT_Y (LAUNCH_KEY_P_Y + LAUNCH_KEY_REC_H / 2.0f - p_Size.y / 2.0f)
//2nd keyboard line
#define LAUNCH_KEY_A_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 81)
#define LAUNCH_KEY_A_LINE_Y_START (LAUNCH_KEY_Y + Var_GetMonitorHeight() / 800.0f * 72)
#define LAUNCH_KEY_A_X (LAUNCH_KEY_A_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_A_Y (LAUNCH_KEY_LINE_END_BOTTOM2)
#define LAUNCH_KEY_A_TXT_X (LAUNCH_KEY_A_LINE_X - a_Size.x / 2.0f)
#define LAUNCH_KEY_A_TXT_Y (LAUNCH_KEY_A_Y + LAUNCH_KEY_REC_H / 2.0f - a_Size.y / 2.0f)

#define LAUNCH_KEY_S_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 117)
#define LAUNCH_KEY_S_LINE_Y_START (LAUNCH_KEY_A_LINE_Y_START)
#define LAUNCH_KEY_S_X (LAUNCH_KEY_S_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_S_Y (LAUNCH_KEY_LINE_END_BOTTOM1)
#define LAUNCH_KEY_S_TXT_X (LAUNCH_KEY_S_LINE_X - s_Size.x / 2.0f)
#define LAUNCH_KEY_S_TXT_Y (LAUNCH_KEY_S_Y + LAUNCH_KEY_REC_H / 2.0f - s_Size.y / 2.0f)

#define LAUNCH_KEY_D_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 152)
#define LAUNCH_KEY_D_LINE_Y_START (LAUNCH_KEY_A_LINE_Y_START)
#define LAUNCH_KEY_D_X (LAUNCH_KEY_D_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_D_Y (LAUNCH_KEY_LINE_END_BOTTOM2)
#define LAUNCH_KEY_D_TXT_X (LAUNCH_KEY_D_LINE_X - d_Size.x / 2.0f)
#define LAUNCH_KEY_D_TXT_Y (LAUNCH_KEY_D_Y + LAUNCH_KEY_REC_H / 2.0f - d_Size.y / 2.0f)

#define LAUNCH_KEY_F_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 188)
#define LAUNCH_KEY_F_LINE_Y_START (LAUNCH_KEY_A_LINE_Y_START)
#define LAUNCH_KEY_F_X (LAUNCH_KEY_F_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_F_Y (LAUNCH_KEY_LINE_END_BOTTOM1)
#define LAUNCH_KEY_F_TXT_X (LAUNCH_KEY_F_LINE_X - f_Size.x / 2.0f)
#define LAUNCH_KEY_F_TXT_Y (LAUNCH_KEY_F_Y + LAUNCH_KEY_REC_H / 2.0f - f_Size.y / 2.0f)

#define LAUNCH_KEY_H_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 260)
#define LAUNCH_KEY_H_LINE_Y_START (LAUNCH_KEY_A_LINE_Y_START)
#define LAUNCH_KEY_H_X (LAUNCH_KEY_H_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_H_Y (LAUNCH_KEY_LINE_END_BOTTOM1)
#define LAUNCH_KEY_H_TXT_X (LAUNCH_KEY_H_LINE_X - h_Size.x / 2.0f)
#define LAUNCH_KEY_H_TXT_Y (LAUNCH_KEY_H_Y + LAUNCH_KEY_REC_H / 2.0f - h_Size.y / 2.0f)

#define LAUNCH_KEY_J_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 296)
#define LAUNCH_KEY_J_LINE_Y_START (LAUNCH_KEY_A_LINE_Y_START)
#define LAUNCH_KEY_J_X (LAUNCH_KEY_J_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_J_Y (LAUNCH_KEY_LINE_END_BOTTOM2)
#define LAUNCH_KEY_J_TXT_X (LAUNCH_KEY_J_LINE_X - j_Size.x / 2.0f)
#define LAUNCH_KEY_J_TXT_Y (LAUNCH_KEY_J_Y + LAUNCH_KEY_REC_H / 2.0f - j_Size.y / 2.0f)

#define LAUNCH_KEY_K_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 330)
#define LAUNCH_KEY_K_LINE_Y_START (LAUNCH_KEY_A_LINE_Y_START)
#define LAUNCH_KEY_K_X (LAUNCH_KEY_K_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_K_Y (LAUNCH_KEY_LINE_END_BOTTOM1)
#define LAUNCH_KEY_K_TXT_X (LAUNCH_KEY_K_LINE_X - k_Size.x / 2.0f)
#define LAUNCH_KEY_K_TXT_Y (LAUNCH_KEY_K_Y + LAUNCH_KEY_REC_H / 2.0f - k_Size.y / 2.0f)

#define LAUNCH_KEY_L_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 366)
#define LAUNCH_KEY_L_LINE_Y_START (LAUNCH_KEY_A_LINE_Y_START)
#define LAUNCH_KEY_L_X (LAUNCH_KEY_L_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_L_Y (LAUNCH_KEY_LINE_END_BOTTOM2)
#define LAUNCH_KEY_L_TXT_X (LAUNCH_KEY_L_LINE_X - l_Size.x / 2.0f)
#define LAUNCH_KEY_L_TXT_Y (LAUNCH_KEY_L_Y + LAUNCH_KEY_REC_H / 2.0f - l_Size.y / 2.0f)
//Other keys
#define LAUNCH_KEY_ENTER_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 500)
#define LAUNCH_KEY_ENTER_LINE_Y_START (LAUNCH_KEY_Y + Var_GetMonitorHeight() / 800.0f * 55)
#define LAUNCH_KEY_ENTER_X (LAUNCH_KEY_ENTER_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_ENTER_Y (LAUNCH_KEY_LINE_END_TOP1 - LAUNCH_KEY_REC_H)
#define LAUNCH_KEY_ENTER_TXT_X (LAUNCH_KEY_ENTER_LINE_X - enter_Size.x / 2.0f)
#define LAUNCH_KEY_ENTER_TXT_Y (LAUNCH_KEY_ENTER_Y + LAUNCH_KEY_REC_H / 2.0f - enter_Size.y / 2.0f)

#define LAUNCH_KEY_RSHIFT_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 490)
#define LAUNCH_KEY_RSHIFT_LINE_Y_START (LAUNCH_KEY_Y + Var_GetMonitorHeight() / 800.0f * 98)
#define LAUNCH_KEY_RSHIFT_X (LAUNCH_KEY_RSHIFT_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_RSHIFT_Y (LAUNCH_KEY_LINE_END_BOTTOM1)
#define LAUNCH_KEY_RSHIFT_TXT_X (LAUNCH_KEY_RSHIFT_LINE_X - rShift_Size.x / 2.0f)
#define LAUNCH_KEY_RSHIFT_TXT_Y (LAUNCH_KEY_RSHIFT_Y + LAUNCH_KEY_REC_H / 2.0f - rShift_Size.y / 2.0f)

#define LAUNCH_KEY_LSHIFT_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 45)
#define LAUNCH_KEY_LSHIFT_LINE_Y_START (LAUNCH_KEY_RSHIFT_LINE_Y_START)
#define LAUNCH_KEY_LSHIFT_X (LAUNCH_KEY_LSHIFT_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_LSHIFT_Y (LAUNCH_KEY_LINE_END_BOTTOM1)
#define LAUNCH_KEY_LSHIFT_TXT_X (LAUNCH_KEY_LSHIFT_LINE_X - lShift_Size.x / 2.0f)
#define LAUNCH_KEY_LSHIFT_TXT_Y (LAUNCH_KEY_LSHIFT_Y + LAUNCH_KEY_REC_H / 2.0f - lShift_Size.y / 2.0f)

#define LAUNCH_KEY_SPACE_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 224)
#define LAUNCH_KEY_SPACE_LINE_Y_START (LAUNCH_KEY_Y + Var_GetMonitorHeight() / 800.0f * 125)
#define LAUNCH_KEY_SPACE_X (LAUNCH_KEY_SPACE_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_SPACE_Y (LAUNCH_KEY_LINE_END_BOTTOM2)
#define LAUNCH_KEY_SPACE_TXT_X (LAUNCH_KEY_SPACE_LINE_X - space_Size.x / 2.0f)
#define LAUNCH_KEY_SPACE_TXT_Y (LAUNCH_KEY_SPACE_Y + LAUNCH_KEY_REC_H / 2.0f - space_Size.y / 2.0f)
//Mouse
#define LAUNCH_KEY_MOUSE_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 636)
#define LAUNCH_KEY_MOUSE_LINE_Y_START (LAUNCH_MOUSE_Y - Var_GetMonitorHeight() / 800.0f * 1)
#define LAUNCH_KEY_MOUSE_X (LAUNCH_KEY_MOUSE_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_MOUSE_Y (LAUNCH_KEY_LINE_END_TOP1 - LAUNCH_KEY_REC_H)
#define LAUNCH_KEY_MOUSE_TXT_X (LAUNCH_KEY_MOUSE_LINE_X - mouse_Size.x / 2.0f)
#define LAUNCH_KEY_MOUSE_TXT_Y (LAUNCH_KEY_MOUSE_Y + LAUNCH_KEY_REC_H / 2.0f - mouse_Size.y / 2.0f)

#define LAUNCH_KEY_LCLICK_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 615)
#define LAUNCH_KEY_LCLICK_LINE_Y_START (LAUNCH_MOUSE_Y + Var_GetMonitorHeight() / 800.0f * 70)
#define LAUNCH_KEY_LCLICK_X (LAUNCH_KEY_LCLICK_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_LCLICK_Y (LAUNCH_KEY_LINE_END_BOTTOM1)
#define LAUNCH_KEY_LCLICK_TXT_X (LAUNCH_KEY_LCLICK_LINE_X - lClick_Size.x / 2.0f)
#define LAUNCH_KEY_LCLICK_TXT_Y (LAUNCH_KEY_LCLICK_Y + LAUNCH_KEY_REC_H / 2.0f - lClick_Size.y / 2.0f)

#define LAUNCH_KEY_RCLICK_LINE_X (LAUNCH_KEY_X + Var_GetMonitorWidth() / 1600.0f * 657)
#define LAUNCH_KEY_RCLICK_LINE_Y_START (LAUNCH_MOUSE_Y + Var_GetMonitorHeight() / 800.0f * 70)
#define LAUNCH_KEY_RCLICK_X (LAUNCH_KEY_RCLICK_LINE_X - LAUNCH_KEY_REC_W / 2)
#define LAUNCH_KEY_RCLICK_Y (LAUNCH_KEY_LINE_END_BOTTOM2)
#define LAUNCH_KEY_RCLICK_TXT_X (LAUNCH_KEY_RCLICK_LINE_X - rClick_Size.x / 2.0f)
#define LAUNCH_KEY_RCLICK_TXT_Y (LAUNCH_KEY_RLCLICK_Y + LAUNCH_KEY_REC_H / 2.0f - rClick_Size.y / 2.0f)
//Arrows
#define LAUNCH_KEY_ARROWS_LINE_X1 (LAUNCH_ARROWS_X + LAUNCH_ARROWS_W + Var_GetMonitorWidth() / 200.0f)
#define LAUNCH_KEY_ARROWS_LINE_X2 (LAUNCH_KEY_ARROWS_LINE_X1 + Var_GetMonitorWidth() / 20.0f)
#define LAUNCH_KEY_ARROWS_LINE_Y (LAUNCH_ARROWS_Y + LAUNCH_ARROWS_H / 2.0f)
#define LAUNCH_KEY_ARROWS_REC_X (LAUNCH_KEY_ARROWS_LINE_X2)
#define LAUNCH_KEY_ARROWS_REC_Y (LAUNCH_KEY_ARROWS_LINE_Y - LAUNCH_KEY_REC_H / 2.0f)
#define LAUNCH_KEY_ARROWS_TXT_X (LAUNCH_KEY_ARROWS_REC_X + LAUNCH_KEY_REC_W / 2.0f - arrows_Size.x / 2.0f)
#define LAUNCH_KEY_ARROWS_TXT_Y (LAUNCH_KEY_ARROWS_LINE_Y - arrows_Size.y / 2.0f)

#define LAUNCH_CONTROL_OUTLINE_THICKNESS (Var_GetMonitorHeight() / 200.0f)
#pragma endregion



//Y coordinate for bottom section
//#define BOTTOM_Y (Var_GetMonitorHeight() * (7.0f / 8))

#endif