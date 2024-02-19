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

void LoadOverlayWindow(SubtitleSettings settings){
	if(settings.OVERLAY_MODE){
		SetConfigFlags(FLAG_WINDOW_TRANSPARENT); // Configures window to be transparent
		SetConfigFlags(FLAG_WINDOW_TOPMOST); //Always on top
		SetConfigFlags(FLAG_WINDOW_MOUSE_PASSTHROUGH);
		SetConfigFlags(FLAG_WINDOW_UNFOCUSED);	//Probably just use less processing power
	}
	InitWindow(GetScreenWidth(), GetScreenHeight(), "Transparent");	//It doesn't actually set the height to the monitor height. Frustrating.
	SetWindowPosition(0, 0);
	SetWindowState(FLAG_WINDOW_UNDECORATED); // Hide border/titlebar; omit if you want them there.
	if(settings.BORDERLESS_WINDOW_MODE){
		SetWindowState(FLAG_BORDERLESS_WINDOWED_MODE); // Hide border/titlebar; omit if you want them there.
	}else{
		SetMouseOffset(0, 10);
	}
	SetTargetFPS(settings.FRAME_RATE);
}

void LoadGameShit(){
	SubtitleSettings settings = getSubtitleSettings();
	LoadOverlayWindow(settings);
	LoadSubtitles(settings);
}

void UpdateDrawFrame(){	//GetFrameTime gives you frame time
	const SubtitleSettings bunchOfSettings[] = {
		getSubtitleSettings(), (SubtitleSettings){	//Nothing
			getSubtitleSettings().SUBTITLE_FONT_SIZE,	//Font size
			getSubtitleSettings().OVERLAY_MODE,	//Overlay
			getSubtitleSettings().BORDERLESS_WINDOW_MODE,	//Borderless window (buggy)
			getSubtitleSettings().FRAME_RATE,	//Frame rate
			
			false,	//Background
			false,	//Outline
			5,		//Outline thiccness (pixels)
			
			false,	//Around shadow
			20,		//Around shadow distance
			
			false	//Rainbow
		}, (SubtitleSettings){	//Background only
			getSubtitleSettings().SUBTITLE_FONT_SIZE,	//Font size
			getSubtitleSettings().OVERLAY_MODE,	//Overlay
			getSubtitleSettings().BORDERLESS_WINDOW_MODE,	//Borderless window (buggy)
			getSubtitleSettings().FRAME_RATE,	//Frame rate
			
			true,	//Background
			false,	//Outline
			5,		//Outline thiccness (pixels)
			
			false,	//Around shadow
			20,		//Around shadow distance
			
			false	//Rainbow
		}, (SubtitleSettings){	//Outline only
			getSubtitleSettings().SUBTITLE_FONT_SIZE,	//Font size
			getSubtitleSettings().OVERLAY_MODE,	//Overlay
			getSubtitleSettings().BORDERLESS_WINDOW_MODE,	//Borderless window (buggy)
			getSubtitleSettings().FRAME_RATE,	//Frame rate
			
			false,	//Background
			true,	//Outline
			5,		//Outline thiccness (pixels)
			
			false,	//Around shadow
			20,		//Around shadow distance
			
			false	//Rainbow
		}, (SubtitleSettings){	//Background and outline
			getSubtitleSettings().SUBTITLE_FONT_SIZE,	//Font size
			getSubtitleSettings().OVERLAY_MODE,	//Overlay
			getSubtitleSettings().BORDERLESS_WINDOW_MODE,	//Borderless window (buggy)
			getSubtitleSettings().FRAME_RATE,	//Frame rate
			
			true,	//Background
			true,	//Outline
			5,		//Outline thiccness (pixels)
			
			false,	//Around shadow
			20,		//Around shadow distance
			
			false	//Rainbow
		}, (SubtitleSettings){	//Around shadow
			getSubtitleSettings().SUBTITLE_FONT_SIZE,	//Font size
			getSubtitleSettings().OVERLAY_MODE,	//Overlay
			getSubtitleSettings().BORDERLESS_WINDOW_MODE,	//Borderless window (buggy)
			getSubtitleSettings().FRAME_RATE,	//Frame rate
			
			false,	//Background
			false,	//Outline
			5,		//Outline thiccness (pixels)
			
			true,	//Around shadow
			20,		//Around shadow distance
			
			false	//Rainbow
		}, (SubtitleSettings){	//Rainbow
			getSubtitleSettings().SUBTITLE_FONT_SIZE,	//Font size
			getSubtitleSettings().OVERLAY_MODE,	//Overlay
			getSubtitleSettings().BORDERLESS_WINDOW_MODE,	//Borderless window (buggy)
			getSubtitleSettings().FRAME_RATE,	//Frame rate
			
			false,	//Background
			false,	//Outline
			5,		//Outline thiccness (pixels)
			
			false,	//Around shadow
			20,		//Around shadow distance
			
			true	//Rainbow
		}
	};

	setSubtitleSettings(bunchOfSettings[((int)GetTime()) % (sizeof(bunchOfSettings) / sizeof(SubtitleSettings))]);
	SubtitleSettings settings = getSubtitleSettings();
	UpdateSubtitleTexture("Bro: Raylib is cool. This didn't even take that long.", settings);
	DrawSubtitleTexture(settings);
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