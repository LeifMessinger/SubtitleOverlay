#include "subtitle_settings.h"
#include "stdio.h" //printf
//Stupid C things
#define true 1
#define false 0

//Have to call it MY_CONFIG because config.h exists in raylib

const SubtitleSettings defaults = {
	100.0f,	//Font size
	0.5f,	//Text scale
	{0, 0},	//Text position
	
	WHITE, //Subtitle color
	false,	//Rainbow
	
	true,	//Background
	{30, 40}, //Extra border
	{0, 0, 0, 200}, //Border color
	false,	//Rainbow
	
	false,	//Outline
	BLANK,	//Outline
	false,	//Rainbow
	2,		//Outline thiccness (pixels)
	
	false,	//Around shadow
	5.0,		//Around shadow distance
	
};

//Remember to RL_FREE it
float* RaylibColorToShaderColor(Color color){
	float* glslColor = RL_CALLOC(4, sizeof(float));
	glslColor[0] = (float)color.r / 255.0f;
	glslColor[1] = (float)color.g / 255.0f;
	glslColor[2] = (float)color.b / 255.0f;
	glslColor[3] = (float)color.a / 255.0f;
	return glslColor;
}

const SubtitleSettings initSubtitleSettings(){
	return defaults;
}

SubtitleSettings settings = defaults;	//In theory, we're phasing out the settings global object for OOP in C

const SubtitleSettings getSubtitleSettings(){
	return settings;
}
void setSubtitleSettings(const SubtitleSettings otherSettings){
	settings = otherSettings;
}

//WINDOW
#define FRAME_RATE 200
#define GET_INTO_IT false
#define BORDERLESS_WINDOW_MODE false

bool overlayMode = false;
bool isOverlayMode(){
	return overlayMode;
}

bool firstTimeOpeningWindow = true;	//This does *not* work
void LoadOverlayWindow(bool overlayModeArg){
	if(!firstTimeOpeningWindow){
		CloseWindow();
	}else{
		firstTimeOpeningWindow = false;
	}
	SetConfigFlags(FLAG_WINDOW_TRANSPARENT); // Configures window to be transparent
	SetConfigFlags(FLAG_WINDOW_TOPMOST); //Always on top
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	if(overlayModeArg){
		SetConfigFlags(FLAG_WINDOW_UNFOCUSED);	//Probably just use less processing power
		SetConfigFlags(FLAG_WINDOW_MOUSE_PASSTHROUGH);
		overlayMode = true;
	}
	SetConfigFlags(FLAG_VSYNC_HINT);	//Prevent runaway fps
	InitWindow(GetScreenWidth(), GetScreenHeight(), "Transparent");	//It doesn't actually set the height to the monitor height. Frustrating.
	SetWindowPosition(0, 0);
	//SetWindowState(FLAG_WINDOW_UNDECORATED); // Breaks on some computers
	//SetWindowState(FLAG_WINDOW_UNDECORATED); // Breaks on some computers
	if(BORDERLESS_WINDOW_MODE){
		SetWindowState(FLAG_BORDERLESS_WINDOWED_MODE); // Hide border/titlebar; omit if you want them there.
	}else{
		SetMouseOffset(0, 10);
	}
	SetTargetFPS(FRAME_RATE);
}



SubtitleInstance initSubtitleInstance(SubtitleSettings settings, int font){
	return (SubtitleInstance){LoadRenderTexture(1, 1), {1, 1}, settings, true, font, "Bruh", NULL};
}

Vector2 subtitleInstanceDestinationSize(const SubtitleInstance instance){
	return (Vector2){instance.targetDimensions.x * instance.settings.textScale, instance.targetDimensions.y * instance.settings.textScale};
}

bool pointIsInRectangle(const Vector2 point, const Rectangle rect){
	return (point.x >= rect.x && point.x <= rect.x + rect.width) && (point.y >= rect.y && point.y <= rect.y + rect.height);
}
Rectangle rectangleFromSizeAndPosition(const Vector2 size, const Vector2 position){
	return (Rectangle){position.x, position.y, size.x, size.y};
}
Rectangle rectangleFromSizeCenteredAroundPosition(const Vector2 size, const Vector2 position){
	return rectangleFromSizeAndPosition(size, (Vector2){position.x - (size.x / 2), position.y - (size.y / 2)});
}
Rectangle subtitleInstanceDestination(const SubtitleInstance instance){
	return rectangleFromSizeCenteredAroundPosition(subtitleInstanceDestinationSize(instance), instance.settings.position);
}
bool subtitleInstanceIsRainbow(const SubtitleInstance instance){
	return instance.settings.textRainbow || (instance.settings.BACKGROUND && instance.settings.subtitleBoxRainbow) || (instance.settings.OUTLINE && instance.settings.outlineRainbow);
}
void printVector2(const char* message, Vector2 bruh){
	printf(message);
	printf(": %f %f\n", bruh.x, bruh.y);
}
void printRectangle(const char* message, Rectangle bruh){
	printf(message);
	printf(": Pos %f %f. Size %f %f\n", bruh.x, bruh.y, bruh.width, bruh.height);
}

void UnloadSubtitleInstance(SubtitleInstance instance){
	UnloadRenderTexture(instance.target);
}