#ifndef LIVE_SUBTITLES_H
#define LIVE_SUBTITLES_H

#if (defined(__STDC__) && __STDC_VERSION__ >= 199901L) || (defined(_MSC_VER) && _MSC_VER >= 1800)
    #include <stdbool.h>
#elif !defined(__cplusplus) && !defined(bool)
	//From raylib.h. Hopefully that ^ means the redefinition is fine
    typedef enum bool { false = 0, true = !false } bool;
    #define RL_BOOL_TYPE
#endif

#define LIVE_SUBTITLES_BUFFER_SIZE (1024 * 3)

void LiveSubtitlesStartup();
void UnloadLiveSubtitles();

const char* LoadLiveSubtitles();

const char* getLiveSubtitles();
long long getLiveSubtitlesWaitTime();
bool isThereLiveSubtitles();
bool thereIsLiveSubtitles();	//Subtitle text changed since the last time called

#endif