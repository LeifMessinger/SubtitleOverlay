#ifndef SUBTITLES_H
#define SUBTITLES_H

#include "raylib.h"
#include "subtitle_settings.h"
#include <string.h>	//strcat
#include <stdio.h>	//puts
#include <stdlib.h>	//malloc

void LoadSubtitles(SubtitleSettings settings);
void UpdateSubtitleTexture(const char* subtitleText, SubtitleSettings settings);
void DrawSubtitleTexture(SubtitleSettings settings);
void UnloadSubtitles();

#endif