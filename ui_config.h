#pragma region UI_GENERAL
//General setup definitions
#define FPS 60
#define BACKGROUND_CLR (Color){ 60, 64, 72, 255 }
#define SCREEN_W 800.0f
#define SCREEN_H 450.0f
#define CENTER_X (SCREEN_W / 2)
#define RIGHT 1
#define LEFT 0
#define MENU_IMG_Y_CENTER (SCREEN_H * (1.0f / 2))
#define BTN_RADIUS (SCREEN_W / 100.0f)
#define SCROLL_THRESHOLD 3.0f
#define BTN_TXT_SIZE (SCREEN_W / 60.0f)

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

#pragma region UI_CTRLS_L
//Definitions for the controls at the top left of the menu
#define MIDDLE_ARROW_X (SCREEN_W / 8.0f)
#define L_ARROW_X (MIDDLE_ARROW_X - (SCREEN_W / 64.0f))
#define R_ARROW_X (MIDDLE_ARROW_X + (SCREEN_W / 64.0f))
#define ARROW_Y (SCREEN_W / 16.0f)
#define ARROW_TXT "SCROLL"
#define ARROW_TXT_X (UI_CenterText_X(ARROW_TXT, BTN_TXT_SIZE, MIDDLE_ARROW_X))
#define ARROW_TXT_Y (ARROW_Y + (3.0f * SELECT_PADDING_Y / 4))
#pragma endregion

#pragma region UI_CTRLS_R
#define BTN_BACK_Y (ARROW_Y)
#define MIDDLE_BACK_X (SCREEN_W * (7 / 8.0f))
#define XBOX_B_X (MIDDLE_BACK_X - MeasureText(SELECT_TXT2, BTN_TXT_SIZE) / 2 - BTN_RADIUS - SELECT_PADDING_X)
#define XBOX_B_TXT_X (UI_CenterText_X("B", BTN_TXT_SIZE, XBOX_B_X))
#define XBOX_B_TXT_Y (BTN_BACK_Y - BTN_TXT_SIZE * 0.5)
#define PS_O_X (MIDDLE_BACK_X + MeasureText(SELECT_TXT2, BTN_TXT_SIZE) / 2 + BTN_RADIUS + SELECT_PADDING_X)
#define PS_O_TXT_X (UI_CenterText_X("O", BTN_TXT_SIZE, PS_O_X) + 0.1f)
#define PS_O_TXT_Y (BTN_BACK_Y - BTN_TXT_SIZE * 0.5)
#define BTN_BACK_SLASH_X (UI_CenterText_X("/", BTN_TXT_SIZE, MIDDLE_BACK_X))
#define BTN_BACK_TXT ("GO BACK")
#define BTN_BACK_TXT_X (UI_CenterText_X(BTN_BACK_TXT, BTN_TXT_SIZE, MIDDLE_BACK_X))
#define BTN_BACK_TXT_Y (ARROW_TXT_Y)
#pragma endregion

#pragma region UI_GAME
//Game screen coordinates
#define LEFT2_GAME (LEFT1_GAME - (SIDE2_GAME_W * (19 / 20.0f)))
#define LEFT1_GAME (CENTER_X - (SIDE1_GAME_W * (19 / 20.0f)))
#define RIGHT1_GAME (CENTER_X + (SIDE1_GAME_W * (19 / 20.0f)))
#define RIGHT2_GAME (RIGHT1_GAME + (SIDE2_GAME_W * (19 / 20.0f)))
//Game screen constants
#define GAME_ASPECT_RATIO (3.0f/4)
//Center game coordinates
#define CENTER_GAME_W (SCREEN_W * (2.0f / 8))
#define CENTER_GAME_H ((1 / GAME_ASPECT_RATIO) * CENTER_GAME_W)
#define CENTER_GAME_Y (UI_CenterImg_Y(CENTER_GAME_H, MENU_IMG_Y_CENTER))
#define CENTER_GAME_X (UI_CenterImg_X(CENTER_GAME_W, CENTER_X))
//First side games coordiantes
#define SIDE1_GAME_W (CENTER_GAME_W * (4/5.0f))
#define SIDE1_GAME_H ((1 / GAME_ASPECT_RATIO) * SIDE1_GAME_W)
#define SIDE1_GAME_Y (UI_CenterImg_Y(SIDE1_GAME_H, MENU_IMG_Y_CENTER))
#define LEFT1_GAME_X (UI_CenterImg_X(SIDE1_GAME_W, LEFT1_GAME))
#define RIGHT1_GAME_X (UI_CenterImg_X(SIDE1_GAME_W, RIGHT1_GAME))
//Second side games coordinates
#define SIDE2_GAME_W (CENTER_GAME_W * (3/5.0f))
#define SIDE2_GAME_H ((1 / GAME_ASPECT_RATIO) * SIDE2_GAME_W)
#define SIDE2_GAME_Y (UI_CenterImg_Y(SIDE2_GAME_H, MENU_IMG_Y_CENTER))
#define LEFT2_GAME_X (UI_CenterImg_X(SIDE2_GAME_W, LEFT2_GAME))
#define RIGHT2_GAME_X (UI_CenterImg_X(SIDE2_GAME_W, RIGHT2_GAME))
//Third side games coordinates
#define SIDE3_GAME_W (CENTER_GAME_W * (2/5.0f))
#define SIDE3_GAME_H ((1 / GAME_ASPECT_RATIO) * SIDE3_GAME_W)
#define SIDE3_GAME_Y (UI_CenterImg_Y(SIDE3_GAME_H, MENU_IMG_Y_CENTER))
#define LEFT3_GAME_X (UI_CenterImg_X(SIDE3_GAME_W, (LEFT1_GAME + LEFT2_GAME) / 2))
#define RIGHT3_GAME_X (UI_CenterImg_X(SIDE3_GAME_W, (RIGHT2_GAME + RIGHT1_GAME) / 2))

#define SCROLL_GAMES_SPEED 0.15f
#pragma endregion

#pragma region UI_GAMES_TOP
#define TOP_Y (SCREEN_H * (1.0f / 7))
#define CATEG_Y (40 - size.y / 2)
#define LEFT1_CATEG (SCREEN_W * (1.0f / 4))
#define RIGHT1_CATEG (SCREEN_W * (3.0f / 4))

#define CENTER_CATEG_SIZE (SCREEN_W / 38.0f)
#define SIDE1_CATEG_SIZE (CENTER_CATEG_SIZE * (3.0f / 4))

#define CENTER_CATEG_X (UI_CenterText_X(categoriesDisplayed[2].name, CENTER_CATEG_SIZE, CENTER_X))

#define LEFT1_CATEG_X (UI_CenterText_X(categoriesDisplayed[1].name, SIDE1_CATEG_SIZE, LEFT1_CATEG))
#define RIGHT1_CATEG_X (UI_CenterText_X(categoriesDisplayed[1].name, SIDE1_CATEG_SIZE, RIGHT1_CATEG))
#pragma endregion

#pragma region UI_GAMES_BOTTOM

#define BOTTOM_Y (SCREEN_H * (6.0f / 7))

//Selected game outline constants
#define THICKNESS_SELECT_GAME (SCREEN_W / 200.0f)
#define THICKNESS_OTHER (SCREEN_W / 400.0f)
#define THICKNESS_SELECT_TXT (SCREEN_W / 400.0f)
//Select text constants
#define SELECT_TXT1 "PRESS"
#define SELECT_TXT2 "/"
#define SELECT_TXT3 "TO PLAY"
#define SELECT_PADDING_X (SCREEN_W / 160.0f)
#define SELECT_PADDING_Y (SCREEN_W / 40.0f)
#define SELECT_LEN (MeasureText(SELECT_TXT1, BTN_TXT_SIZE) + MeasureText(SELECT_TXT2, BTN_TXT_SIZE) + MeasureText(SELECT_TXT3, BTN_TXT_SIZE) + (4 * BTN_RADIUS) + (4 * SELECT_PADDING_X))
//Select text coordinates
#define SELECT_TXT1_X (SCREEN_W / 20.0f)
#define SELECT_TXT2_X (XBOX_A_X + BTN_RADIUS + SELECT_PADDING_X)
#define SELECT_TXT3_X (PS_X_X + BTN_RADIUS + SELECT_PADDING_X)

#define GAME_TXT "GAME TITLE"
#define GAME_TXT_Y (SCREEN_H * (18.0f / 20))
#define BOTTOM_TXT "PRESS [A/X] TO PLAY   |   PRESS [OPTIONS] FOR OPTIONS"
#define BOTTOM_TXT_X (SCREEN_W / 20.0f)
#define BOTTOM_TXT_Y (SCREEN_H * (19.0f / 20))

#define SELECT_TXT_Y (SCREEN_H * (19.0f / 20))
//Select button coordinates
#define XBOX_A_X (SELECT_TXT1_X + MeasureText(SELECT_TXT1, BTN_TXT_SIZE) + SELECT_PADDING_X + BTN_RADIUS)
#define PS_X_X (SELECT_TXT2_X + MeasureText(SELECT_TXT2, BTN_TXT_SIZE) + SELECT_PADDING_X + BTN_RADIUS)
#define XBOX_A_Y (SELECT_TXT_Y + BTN_TXT_SIZE * 0.40f)
#define PS_X_Y (SELECT_TXT_Y + BTN_TXT_SIZE * 0.4f)
#define XBOX_A_TXT_X (UI_CenterText_X("A", BTN_TXT_SIZE, XBOX_A_X))
#define PS_X_TXT_X (UI_CenterText_X("X", BTN_TXT_SIZE, PS_X_X))



#pragma endregion