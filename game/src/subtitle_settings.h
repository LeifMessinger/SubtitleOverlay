#ifndef SUBTITLE_SETTINGS_H
#define SUBTITLE_SETTINGS_H

#include "raylib.h"	//Literally just for the bools

//Have to call it MY_CONFIG because config.h exists in raylib

typedef struct {
	float SUBTITLE_FONT_SIZE;
	float textScale;
	Vector2 position;
	
	Color textColor;

	bool BACKGROUND;
	Vector2 subtitleBoundingBoxExtra;
	Color subtitleBoxColor;

	bool OUTLINE;
	float OUTLINE_DISTANCE;

	bool AROUND_SHADOW;
	float AROUND_SHADOW_DISTANCE;


	bool RAINBOW;
} SubtitleSettings;

typedef struct{
	RenderTexture2D target;
	Vector2 targetDimensions;	//I don't really need this. I can just use target.texture.width height etc, but I already wrote code like this lol
	SubtitleSettings settings;
	int font;
	const char* text;
	
} SubtitleInstance;

enum {
	DEFAULT_FONT = 0,
};

void LoadOverlayWindow(bool overlayMode);

SubtitleInstance initSubtitleInstance(SubtitleSettings settings, int font);	//Font can be NULL

void UnloadSubtitleInstance(SubtitleInstance instance);

bool pointIsInRectangle(const Vector2 point, const Rectangle rect);
Rectangle rectangleFromSizeAndPosition(const Vector2 size, const Vector2 position);
Rectangle rectangleFromSizeCenteredAroundPosition(const Vector2 size, const Vector2 position);
void printVector2(const char* message, Vector2 bruh);
void printRectangle(const char* message, Rectangle bruh);

Vector2 subtitleInstanceDestinationSize(const SubtitleInstance instance);
Rectangle subtitleInstanceDestination(const SubtitleInstance instance);

const SubtitleSettings initSubtitleSettings();
const SubtitleSettings getSubtitleSettings();	//We're gonna hope that the C compiler removes the unnecessary copying
void setSubtitleSettings(const SubtitleSettings);

bool isOverlayMode();

#endif