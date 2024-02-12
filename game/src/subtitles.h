#ifndef SUBTITLES_H
#define SUBTITLES_H

#include "raylib.h"
#include <string.h>	//strcat
#include <stdio.h>	//puts
#include <stdlib.h>	//malloc

void LoadSubtitles();
void UpdateSubtitleTexture(const char* subtitleText);
void DrawSubtitleTexture();
void UnloadSubtitles();

#endif