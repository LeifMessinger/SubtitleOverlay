/*******************************************************************************************
*
*   raylib game template
*
*   <Game title>
*   <Game description>
*
*   This game has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2021 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include <string.h>	//strcat
#include <stdio.h>	//puts
#include <stdlib.h>	//malloc

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif
static const int FRAME_RATE = 1;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void ChangeToScreen(int screen);     // Change to screen, no transition effect

static void TransitionToScreen(int screen); // Request transition to next screen
static void UpdateTransition(void);         // Update transition effect
static void DrawTransition(void);           // Draw transition effect (full-screen rectangle)

static void UpdateDrawFrame(void);          // Update and draw one frame

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------

RenderTexture2D target;
Font subtitleFont;
const int SUBTITLE_FONT_SIZE = 50;

void LoadGameShit(){
	SetConfigFlags(FLAG_WINDOW_TRANSPARENT); // Configures window to be transparent
	SetConfigFlags(FLAG_WINDOW_TOPMOST); //Always on top
	SetConfigFlags(FLAG_WINDOW_MOUSE_PASSTHROUGH);
	SetConfigFlags(FLAG_WINDOW_UNFOCUSED);	//Probably just use less processing power
	InitWindow(GetScreenWidth(), GetScreenHeight(), "Transparent");	//It doesn't actually set the height to the monitor height. Frustrating.
	SetWindowPosition(0, 0);
	SetWindowState(FLAG_WINDOW_UNDECORATED); // Hide border/titlebar; omit if you want them there.
	SetTargetFPS(FRAME_RATE);

	/*char* subtitlePath = (char*)calloc(100, 1);
	strcpy(subtitlePath, GetWorkingDirectory());
	strcat(subtitlePath, "\\");
	strcat(subtitlePath, "RoadgeekMittelschrift.ttf");
	puts(subtitlePath);*/
	subtitleFont = LoadFontEx("resources/RoadgeekMittelschrift.ttf", SUBTITLE_FONT_SIZE, NULL, 0);
	//free(subtitlePath);

	target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
	printf("Screen: (%d, %d)\t Monitor: (%d, %d)\n", GetScreenWidth(), GetScreenHeight(), GetMonitorWidth(0), GetMonitorHeight(0));
}

void UpdateDrawFrame(){	//GetFrameTime gives you frame time
	//Drawing to the texture
	BeginTextureMode(target);
	ClearBackground(BLANK);
	
	Vector2 center = {GetScreenWidth() / 2, (GetScreenHeight() / 2) + 10};	//For whatever reason, I gotta add 10.
	//DrawRectangle(center.x - 2, center.y - 2, 4, 4, RED);	//Center dot
	
	const char* subtitleText = "Bro: Raylib is cool. This didn't even take that long.";
	Vector2 subtitleBoundingBox = MeasureTextEx(subtitleFont, subtitleText, SUBTITLE_FONT_SIZE, 5);
	Vector2 subtitleBoundingBoxExtra = {20, 10};
	Color subtitleBoxColor = BLACK;
	subtitleBoxColor.a = 220;
	
	Vector2 subtitlePosition = {center.x, GetScreenHeight() - (subtitleBoundingBox.y * 1.5)};	//Center of the subtitles
	
	DrawRectangle(subtitlePosition.x - (subtitleBoundingBox.x / 2) - (subtitleBoundingBoxExtra.x / 2),
		subtitlePosition.y - (subtitleBoundingBox.y / 2) - (subtitleBoundingBoxExtra.y / 2),
		subtitleBoundingBox.x + subtitleBoundingBoxExtra.x,
		subtitleBoundingBox.y + subtitleBoundingBoxExtra.y,
		subtitleBoxColor
	);	//Center dot
	DrawTextEx(subtitleFont, subtitleText, (Vector2){center.x - (subtitleBoundingBox.x / 2), GetScreenHeight() - (subtitleBoundingBox.y * 2)}, SUBTITLE_FONT_SIZE, 5, WHITE);
	EndTextureMode();

	BeginDrawing();
	//Drawing the texture to the frame buffer
	ClearBackground(BLANK);
	DrawTextureRec(
		target.texture,
		(Rectangle){ 0.0f, 0.0f, GetScreenWidth(), -GetScreenHeight() },	//Flip the height, for some reason
		(Vector2){ 0.f, 0.f },	//Origin
		WHITE	//Color
	);
	EndDrawing();
}

void UnloadGameShit(){
	UnloadFont(subtitleFont);
	UnloadRenderTexture(target);
	CloseWindow();
}

int main(){
	LoadGameShit();
	#if defined(PLATFORM_WEB)
		emscripten_set_main_loop(UpdateDrawFrame, FRAME_RATE, 1);
	#else
		while(!WindowShouldClose())
		{
			UpdateDrawFrame();
		}
	#endif
	UnloadGameShit();

	return 0;
}