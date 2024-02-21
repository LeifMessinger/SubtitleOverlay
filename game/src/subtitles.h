#ifndef SUBTITLES_H
#define SUBTITLES_H

#include "raylib.h"
#include "subtitle_settings.h"
#include <string.h>	//strcat
#include <stdio.h>	//puts
#include <stdlib.h>	//malloc

void LoadSubtitles(SubtitleSettings settings);
void UpdateSubtitles();
void DrawSubtitles();
void UpdateSubtitleInstance(SubtitleInstance* settings);
void DrawSubtitleInstance(SubtitleInstance settings);
void UnloadSubtitles();

#endif