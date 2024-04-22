#include "live_subtitles.h"
#include "timekeeper.h"
#include <stdio.h>
#include <stdlib.h>	//atol, getenv
#include <string.h>

char* liveSubtitles = NULL;
long long int waitUntil = 0;

void UnloadLiveSubtitles(){
	free(liveSubtitles);	//Fine to free a null pointer
}

const char* LoadLiveSubtitles(){
	UnloadLiveSubtitles();
	liveSubtitles = (char*)calloc(LIVE_SUBTITLES_BUFFER_SIZE, sizeof(char));
	
	//Might as well do it again before we have to use liveSubtitles.
	strncpy(liveSubtitles, getenv("TEMP"), LIVE_SUBTITLES_BUFFER_SIZE);
	#if defined(WIN32)
		strncat(liveSubtitles, "\\", LIVE_SUBTITLES_BUFFER_SIZE);
	#else
		strncat(liveSubtitles, "/", LIVE_SUBTITLES_BUFFER_SIZE);
	#endif
	strncat(liveSubtitles, "SubtitlesCurrentWaitUntil.txt", LIVE_SUBTITLES_BUFFER_SIZE);
	FILE* subtitleWaitFile = fopen(liveSubtitles, "r");
	UnloadLiveSubtitles();
	liveSubtitles = (char*)calloc(LIVE_SUBTITLES_BUFFER_SIZE, sizeof(char));
	if(subtitleWaitFile == NULL){
		liveSubtitles[0] = '\0';
		waitUntil = 0;
		fclose(subtitleWaitFile);
	}else{
		fread(liveSubtitles, LIVE_SUBTITLES_BUFFER_SIZE, 1, subtitleWaitFile);
		fclose(subtitleWaitFile);
		waitUntil = atoll(liveSubtitles);	//Returns 0 on error, which is fine.
	}
	
	//Build the path in liveSubtitles cause why not
	strncpy(liveSubtitles, getenv("TEMP"), LIVE_SUBTITLES_BUFFER_SIZE);
	#if defined(WIN32)
		strncat(liveSubtitles, "\\", LIVE_SUBTITLES_BUFFER_SIZE);
	#else
		strncat(liveSubtitles, "/", LIVE_SUBTITLES_BUFFER_SIZE);
	#endif
	strncat(liveSubtitles, "SubtitlesCurrent.txt", LIVE_SUBTITLES_BUFFER_SIZE);
	
	FILE* subtitleTextFile = fopen(liveSubtitles, "r");//Build the path in liveSubtitles cause why not
	UnloadLiveSubtitles();
	liveSubtitles = (char*)calloc(LIVE_SUBTITLES_BUFFER_SIZE, sizeof(char));
	if(subtitleTextFile == NULL){
		strncpy(liveSubtitles, "Waiting for game subtitles...", LIVE_SUBTITLES_BUFFER_SIZE);
		waitUntil = 171381647800000;	//some big number
		return getLiveSubtitles();
	}
	
	fread(liveSubtitles, LIVE_SUBTITLES_BUFFER_SIZE, 1, subtitleTextFile);
	fclose(subtitleTextFile);
	
	return getLiveSubtitles();
}

const char* getLiveSubtitles(){
	return liveSubtitles;
}
long long getLiveSubtitlesWaitTime(){	//Alarm time.
	return waitUntil;
}
bool isThereLiveSubtitles(){
	return !isPastTime(getLiveSubtitlesWaitTime());
}
bool thereIsLiveSubtitlesValue = false;
bool thereIsLiveSubtitles(){	//Always returns false
	return true;
	const bool result = thereIsLiveSubtitlesValue;
	thereIsLiveSubtitlesValue = false;
	return result;
}