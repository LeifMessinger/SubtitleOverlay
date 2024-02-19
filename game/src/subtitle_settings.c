#include "subtitle_settings.h"
//Stupid C things
#define true 1
#define false 0

//Have to call it MY_CONFIG because config.h exists in raylib

SubtitleSettings settings = {
	100.0f,	//Font size
	true,	//Overlay
	false,	//Borderless window (buggy)
	60,	//Frame rate
	
	false,	//Background
	true,	//Outline
	5,		//Outline thiccness (pixels)
	
	false,	//Around shadow
	20,		//Around shadow distance
	
	false	//Rainbow
};

const SubtitleSettings getSubtitleSettings(){
	return settings;
}
void setSubtitleSettings(const SubtitleSettings otherSettings){
	settings = otherSettings;
}