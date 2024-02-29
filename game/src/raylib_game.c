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
#include "subtitle_settings.h" //OVERLAY_MODE
#include "subtitles.h" //LoadSubtitles UpdateSubtitleTexture DrawSubtitleTexture UnloadSubtitles

#include <string.h>	//strcat
#include <stdio.h>	//puts
#include <stdlib.h>	//malloc

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void ChangeToScreen(int screen);     // Change to screen, no transition effect

static void TransitionToScreen(int screen); // Request transition to next screen
static void UpdateTransition(void);         // Update transition effect
static void DrawTransition(void);           // Draw transition effect (full-screen rectangle)

static void UpdateDrawFrame(void);          // Update and draw one frame

void LoadGameShit(){
	SubtitleSettings settings = getSubtitleSettings();
	LoadOverlayWindow(false);
	LoadSubtitles(settings);
}

void UpdateDrawFrame(){	//GetFrameTime gives you frame time
	UpdateSubtitles();	//Subtitle logic
	
	BeginDrawing();
	
	//if(isOverlayMode()){
		ClearBackground(BLANK);
		ClearBackground((Color){100, 100, 100, 0});	//Transparent grey
		//ClearBackground((Color){255, 255, 255, 100});	//Transparent white
	//}else{
		//ClearBackground(PINK);
	//}
	
	DrawSubtitles();
	EndDrawing();
}

void UnloadGameShit(){
	UnloadSubtitles();
	CloseWindow();
}

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------

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