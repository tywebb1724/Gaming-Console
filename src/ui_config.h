#pragma region UI_GENERAL
//General setup definitions
#define FPS 60
#define BACKGROUND_CLR BLACK
#define SCREEN_W monitorWidth
#define SCREEN_H monitorHeight
//Position macros
#define CENTER_X (SCREEN_W / 2)
#define RIGHT 1
#define LEFT 0
#define MENU_IMG_Y_CENTER (SCREEN_H * (11.0f / 20))
#pragma endregion

#pragma region UI_BOOT
//Width and length of logo
#define LOGO_SIZE (SCREEN_H / 5 * 6)
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
//Macros for scrolling animation
#define SCROLL_GAMES_SPEED 0.225f
#define SCROLL_GAMES_THRESHOLD 10.0f
#pragma endregion

#pragma region UI_CATEGORIES
//Categories coordinates
#define TOP_Y (CENTER_GAME_Y - (SCREEN_H / 12.0f))
#define CENTER_CATEG_Y (TOP_Y - MeasureTextEx(fontBold, "L", CENTER_CATEG_SIZE, BOTTOM_TXT_SPACE).y / 2)
#define SIDE_CATEG_Y (TOP_Y -  MeasureTextEx(fontBold, "L", SIDE1_CATEG_SIZE, BOTTOM_TXT_SPACE).y / 2)
#define LEFT1_CATEG (SCREEN_W * (1.0f / 4))
#define RIGHT1_CATEG (SCREEN_W * (3.0f / 4))
//Category sizes
#define CENTER_CATEG_SIZE (SCREEN_W / 35.0f)
#define SIDE1_CATEG_SIZE (CENTER_CATEG_SIZE * (1.0f / 2))
//X coordinates of categories
#define CENTER_CATEG_X (CENTER_X - size.x / 2)
#define LEFT1_CATEG_X (UI_CenterText_X(categoriesDisplayed[1].name, SIDE1_CATEG_SIZE, LEFT1_CATEG))
#define RIGHT1_CATEG_X (UI_CenterText_X(categoriesDisplayed[1].name, SIDE1_CATEG_SIZE, RIGHT1_CATEG))
#pragma endregion

#pragma region UI_BOTTOM
//Y coordinate for bottom section
#define BOTTOM_Y (SCREEN_H * (7.0f / 8))
//Selected game outline constants
#define THICKNESS_SELECT_GAME (SCREEN_W / 200.0f)
#define THICKNESS_OTHER (SCREEN_W / 400.0f)
#define THICKNESS_SELECT_TXT (SCREEN_W / 400.0f)

#define LS_LEFT_X (CENTER_X - SCREEN_W / 55.0f)
#define LS_RIGHT_X (CENTER_X + SCREEN_W / 55.0f)
#define LS_Y (CENTER_GAME_Y + CENTER_GAME_H + SCREEN_H / 23.0f)
#define LS_TRIANGLE_SIZE (SCREEN_W / 80.0f)
#define LS_RADIUS (SCREEN_W / 90.0f)

#define BOTTOM_TXT_SIZE (SCREEN_W / 72.0f)
#define BOTTOM_TXT_SPACE (SCREEN_W / 750.0f)
#define BOTTOM_TXT_SPACING_Y (SCREEN_H / 120.0f)

#define BOTTOM_ROUND 0.1f
#define BOTTOM_SEGMENTS 25

#define GAME_TXT_Y (SCREEN_H * (7.0f / 8))
#define BOTTOM_TXT "Press [A/X] to play"
#define BOTTOM_TXT_X (SCREEN_W / 40.0f)

#define CONSOLE_TXT_Y (GAME_TXT_Y + gameText_Size.y + BOTTOM_TXT_SPACING_Y)
#define BOTTOM_TXT_Y (CONSOLE_TXT_Y + gameText_Size.y + BOTTOM_TXT_SPACING_Y)

#define BOTTOM_RECT_W (maxLen + 4 * BOTTOM_TXT_SPACING_Y)
#define BOTTOM_RECT_H (bottomText_Size.y * 3 + 4 * BOTTOM_TXT_SPACING_Y)
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

#define SMALL_LOGO_SIZE (SCREEN_W / 12.0f)


#define START_SECTION_Y (TOP_Y + MeasureTextEx(fontBold, "L", CENTER_CATEG_SIZE, BOTTOM_TXT_SPACE).y / 2 + 2 * BOTTOM_TXT_SPACING_Y) 
#define END_SECTION_Y (CENTER_CATEG_Y - 2 * BOTTOM_TXT_SPACING_Y)
#define THICKNESS_LINE (SCREEN_H / 500.0f)
#pragma endregion

#pragma region UI_DIAGNOSTICS
#define DIAGNOSTICS_TITLE "System Diagnostics"
#define DIAGNOSTICS_TITLE_SIZE (SCREEN_W / 25.0f)
#define DIAGNOSTICS_TITLE_X (CENTER_X - diagnosticsTitle_Size.x / 2)
#define DIAGNOSTICS_TITLE_Y (SCREEN_H / 12.0f)
#define DIAGNOSTICS_TITLE_TXT_Y (DIAGNOSTICS_TITLE_Y - diagnosticsTitle_Size.x / 2)

#define DIAGNOSTICS_SIZE (SCREEN_W / 35.0f)
#define DIAGNOSTICS_SPACING (SCREEN_W / 400.0f)
#define RESOLUTION_TXT_X (FPS_TXT_X)
#define RESOLUTION_TXT_Y (SCREEN_H / 4.0f)


#define FPS_TXT_X (SCREEN_W / 10.0f)
#define FPS_TXT_Y (RESOLUTION_TXT_Y + SCREEN_H / 12.0f)

#pragma endregion

#pragma region UI_OPTIONS
#define OPTION_BTN_X (SCREEN_W * 19.0f / 20)
#define OPTION_BTN_Y (SCREEN_H / 18.0f)
#define OPTION_LINE_START (OPTION_BTN_X - LS_RADIUS / 2 )
#define OPTION_LINE_END (OPTION_BTN_X + LS_RADIUS / 2 )
#define OPTION_LINE1_Y (OPTION_BTN_Y - LS_RADIUS * (2.0f / 5))
#define OPTION_LINE2_Y (OPTION_BTN_Y)
#define OPTION_LINE3_Y (OPTION_BTN_Y + LS_RADIUS * (2.0f / 5))
#define OPTION_THICKNESS (SCREEN_H / 650.0f)
#define OPTION_THICKNESS2 (SCREEN_H / 550.0f)
#define OPTION_THICKNESS3 (SCREEN_H / 550.0f)

#define OPTIONS_TXT_SIZE (SCREEN_W / 90.0f)
#define OPTIONS_TXT_SPACE (SCREEN_W / 700.0f)
#define OPTIONS_TXT "[OPTIONS]"
#define OPTIONS_TXT_X (OPTION_BTN_X - options_Size.x / 2.0f)
#define OPTIONS_TXT_Y (OPTION_BTN_Y + LS_RADIUS + SCREEN_H / 100.0f)
#pragma endregion

#pragma region UI_OPTIONS_MENU
#define OPTIONS_RECT_X (CENTER_X - OPTIONS_RECT_W / 2)
#define OPTIONS_RECT_Y (SCREEN_H * (1.0f / 2) - OPTIONS_RECT_H / 2)
#define OPTIONS_RECT_W (SCREEN_W * (2.0f / 5))
#define OPTIONS_RECT_H (SCREEN_H * (2.0f / 5))
#define OPTIONS_ROUND 0.025f
#define OPTIONS_SEGMENTS 15

#define OPTIONS_TXT_SPACING_Y (SCREEN_H / 30.0f)
#define OPTIONS_TITLE "[OPTIONS]"
#define OPTIONS_TITLE_SIZE (SCREEN_W / 40.0f)
#define OPTIONS_TITLE_SPACE (SCREEN_W / 400.0f)
#define OPTIONS_TITLE_X (CENTER_X - optionsTitle_Size.x / 2)
#define OPTIONS_TITLE_Y (OPTIONS_RECT_Y + OPTIONS_TXT_SPACING_Y - optionsTitle_Size.y / 2)

#define OPTIONS_BRIGHTNESS "Adjust brightness"
#define OPTIONS_BRIGHTNESS_SIZE (OPTIONS_TITLE_SIZE * (3.0f / 4))
#define OPTIONS_BRIGHTNESS_X (OPTIONS_RECT_X + OPTIONS_TXT_SPACING_Y)
#define OPTIONS_BRIGHTNESS_Y (OPTIONS_TITLE_Y + optionsTitle_Size.y + OPTIONS_TXT_SPACING_Y * (4.0 / 3) - optionsBrightness_Size.y / 2)

#define OPTIONS_THEME "Select theme"
#define OPTIONS_THEME_SIZE (OPTIONS_TITLE_SIZE * (3.0f / 4))
#define OPTIONS_THEME_X (OPTIONS_RECT_X + OPTIONS_TXT_SPACING_Y)
#define OPTIONS_THEME_Y (OPTIONS_BRIGHTNESS_Y + optionsBrightness_Size.y + OPTIONS_TXT_SPACING_Y * (4.0 / 3) - optionsTheme_Size.y / 2)

#define OPTIONS_DISPLAY_DIAGNOSTICS "Display diagnostics"
#define OPTIONS_DISPLAY_DIAGNOSTICS_SIZE (OPTIONS_TITLE_SIZE * (3.0f / 4))
#define OPTIONS_DISPLAY_DIAGNOSTICS_X (OPTIONS_RECT_X + OPTIONS_TXT_SPACING_Y)
#define OPTIONS_DISPLAY_DIAGNOSTICS_Y (OPTIONS_THEME_Y + optionsTheme_Size.y + OPTIONS_TXT_SPACING_Y * (4.0 / 3) - optionsDisplayDiagnostics_Size.y / 2)

#define OPTIONS_DIAGNOSTICS "View diagnostics menu"
#define OPTIONS_DIAGNOSTICS_SIZE (OPTIONS_TITLE_SIZE * (3.0f / 4))
#define OPTIONS_DIAGNOSTICS_X (OPTIONS_RECT_X + OPTIONS_TXT_SPACING_Y)
#define OPTIONS_DIAGNOSTICS_Y (OPTIONS_DISPLAY_DIAGNOSTICS_Y + optionsDisplayDiagnostics_Size.y + OPTIONS_TXT_SPACING_Y * (4.0 / 3) - optionsDiagnostics_Size.y / 2)

#define OPTIONS_SELECT "[A/X] SELECT"
#define OPTIONS_SELECT_SIZE (OPTIONS_TITLE_SIZE * (1.0f / 2))
#define OPTIONS_SELECT_X (OPTIONS_RECT_X + OPTIONS_TXT_SPACING_Y)
#define OPTIONS_SELECT_Y (OPTIONS_RECT_Y + OPTIONS_RECT_H - OPTIONS_TXT_SPACING_Y / 2 - optionsSelect_Size.y)

#define OPTIONS_BACK "[B/?] BACK"
#define OPTIONS_BACK_SIZE (OPTIONS_TITLE_SIZE * (1.0f / 2))
#define OPTIONS_BACK_X (OPTIONS_RECT_X + OPTIONS_RECT_W - OPTIONS_TXT_SPACING_Y - optionsBack_Size.x)
#define OPTIONS_BACK_Y (OPTIONS_RECT_Y + OPTIONS_RECT_H - OPTIONS_TXT_SPACING_Y / 2 - optionsBack_Size.y)

#define OPTIONS_SELECT_RECT_X (OPTIONS_RECT_X + OPTIONS_TXT_SPACING_Y / 2)
#define OPTIONS_SELECT_RECT_W (OPTIONS_RECT_W - OPTIONS_TXT_SPACING_Y)
#define OPTIONS_SELECT_RECT_H (optionsBrightness_Size.y + OPTIONS_TXT_SPACING_Y / 4)
#define OPTIONS_BRIGHTNESS_RECT_Y (OPTIONS_BRIGHTNESS_Y - OPTIONS_TXT_SPACING_Y / 16)
#define OPTIONS_THEME_RECT_Y (OPTIONS_THEME_Y - OPTIONS_TXT_SPACING_Y / 16)
#define OPTIONS_DISPLAY_DIAGNOSTICS_RECT_Y (OPTIONS_DISPLAY_DIAGNOSTICS_Y - OPTIONS_TXT_SPACING_Y / 16)
#define OPTIONS_DIAGNOSTICS_RECT_Y (OPTIONS_DIAGNOSTICS_Y - OPTIONS_TXT_SPACING_Y / 16)
#pragma endregion

#pragma region GAME_LAUNCHER
#define PATH_GBA "assets/cores/mgba_libretro.so"
#define PATH_NES "assets/cores/fceumm_libretro.so"
#define PATH_GENESIS "assets/cores/genesis_plus_gx_libretro.so"
#define PATH_SNES "assets/cores/bsnes_libretro.so"
#define PATH_GAMEBOY "assets/cores/gambatte_libretro.so"
#define PATH_ARCADE "assets/cores/fbnea_libretro.so"
#define PATH_PS1 "assets/cores/swanstation_libretro.so"
#define PATH_N64 "assets/cores/mupen64plus_next_libretro.so"
#define PATH_PSP "assets/cores/ppsspp_libretro.so"
#define PATH_DS "assets/cores/melonds_libretro.so"
#define PATH_DREAMCAST "assets/cores/flycast_libretro.so"
#define PATH_SATURN "assets/cores/yabause_libretro.so"
#define PATH_GBC "assets/cores/sameboy_libretro.so"
#define PATH_LYNX "assets/cores/handy_libretro.so"
#define PATH_NGPC "assets/cores/mednafen_ngp_libretro.so"
#define PATH_WSWAN "assets/cores/mednafen_wswan_libretro.so"
#define PATH_TG16 "assets/cores/mednafen_pce_fast_libretro.so"
#define PATH_ATARI2600 "assets/cores/stella_libretro.so"

#pragma enregion