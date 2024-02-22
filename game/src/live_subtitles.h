#ifndef LIVE_SUBTITLES_H
#define LIVE_SUBTITLES_H

#define LIVE_SUBTITLES_BUFFER_SIZE (1024 * 3)

void UnloadLiveSubtitles();

const char* LoadLiveSubtitles();

const char* getLiveSubtitles();
long getLiveSubtitlesWaitTime();

#endif