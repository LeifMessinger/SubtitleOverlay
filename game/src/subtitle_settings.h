#ifndef SUBTITLE_SETTINGS_H
#define SUBTITLE_SETTINGS_H

#include "raylib.h"	//Literally just for the bools

//Have to call it MY_CONFIG because config.h exists in raylib

typedef struct {
	float SUBTITLE_FONT_SIZE;
	bool OVERLAY_MODE;
	bool BORDERLESS_WINDOW_MODE;	//Kinda breaks HDR. Breaks everything. Would not suggest
	int FRAME_RATE;

	bool BACKGROUND;

	bool OUTLINE;
	float OUTLINE_DISTANCE;

	bool AROUND_SHADOW;
	float AROUND_SHADOW_DISTANCE;


	bool RAINBOW;
} SubtitleSettings;

const SubtitleSettings getSubtitleSettings();	//We're gonna hope that the C compiler removes the unnecessary copying
void setSubtitleSettings(const SubtitleSettings);

#endif