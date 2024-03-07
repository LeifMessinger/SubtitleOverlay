#ifndef SUBTITLES_H
#define SUBTITLES_H

#include "raylib.h"
#include "subtitle_settings.h"
#include <string.h>	//strcat
#include <stdio.h>	//puts
#include <stdlib.h>	//malloc

void SelectFont(SubtitleInstance bruh);
void SelectTextColor(SubtitleInstance bruh);
void SelectBackgroundColor(SubtitleInstance bruh);
void SelectOutlineColor(SubtitleInstance bruh);

void LoadMenu(SubtitleSettings settings);
void UpdateSubtitles();
void DrawSubtitles();
void UpdateSubtitleInstance(SubtitleInstance* settings);
void DrawSubtitleInstance(SubtitleInstance settings);
void LoadShaders();
void UnloadShaders();
void UnloadSubtitles();
void ReloadShaders();

#endif