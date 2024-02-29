#include "subtitles.h"	//Includes subtitle_settings.h too
#include "live_subtitles.h"
#include <assert.h>
#include <math.h>       // Required for: sinf(), cosf(), tan(), atan2f(), sqrtf(), floor(), fminf(), fmaxf(), fabsf()

Shader outlineShader, aroundShadowShader;

//shut up, it works
Font* fontArray = NULL;
int numFonts = 0;

SubtitleInstance* subtitleArray = NULL;
int numSubtitles = 0;

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

void SelectPreset(SubtitleInstance bruh){
	const Vector2 center = {GetScreenWidth() / 2, (GetScreenHeight() / 2) + 10};	//For whatever reason, I gotta add 10.
	SubtitleSettings favorite = bruh.settings;
	
	UnloadSubtitles();
	LoadOverlayWindow(true);
	LoadSubtitles(favorite);
	
	subtitleArray[0].font = bruh.font;
	subtitleArray[0].settings = favorite;	//Should already be like that
	subtitleArray[0].settings.position = (Vector2){center.x, GetScreenHeight() - (favorite.SUBTITLE_FONT_SIZE * favorite.textScale * 1.5)};	//Center of the subtitles
	numSubtitles = 1;	//This'll probably cause a memory leak, but who cares if it's at the end of the program.
	subtitleArray[0].text = "What the hell is this!";
}
void SelectFont(SubtitleInstance bruh);
void SelectFont(SubtitleInstance bruh){
	for(size_t i = 0; i < numSubtitles; ++i){
		if(subtitleArray[i].onclick != (*SelectFont)){
			subtitleArray[i].font = bruh.font;
			subtitleArray[i].settings.SUBTITLE_FONT_SIZE = bruh.settings.SUBTITLE_FONT_SIZE;
		}
	}
}

void LoadSubtitles(SubtitleSettings settings){
	int extraCodePoints[] = {0x2018, 0x2019, 0x201A, 0x201B, 0x201C, 0x201D, 0x201E, 0x201F, 0x2047, 0x2048, 0x2049};
	size_t extraCodePointsSize = (sizeof(extraCodePoints) / sizeof(int));
	int* codepoints = (int *)RL_MALLOC((95*sizeof(int)) + extraCodePointsSize);
	for (int i = 0; i < 95; i++) codepoints[i] = i + 32;
	for (int i = 0; i < extraCodePointsSize; i++) codepoints[i] = extraCodePoints[i];
	
	const char* fonts[] = {"resources/fonts/RoadgeekMittelschrift.ttf", "resources/fonts/amigaForeverPro.ttf"};
	const int fontSizes[] = {100, 50};
	numFonts = FONT_ENUM_SIZE;	//Edit subtitle_settings.h enum to add a font.
	fontArray = (Font*)RL_CALLOC(numFonts, sizeof(Font));
	assert(fontArray != NULL);
	for(size_t i = 0; i < numFonts; ++i){
		fontArray[i] = LoadFontEx(fonts[i], fontSizes[i], codepoints, 95 + extraCodePointsSize);
		assert(IsFontReady(fontArray[i]));
	}
	
	RL_FREE(codepoints);
	
	SubtitleSettings bunchOfSettings[] = {
		(SubtitleSettings){	//Default
			initSubtitleSettings().SUBTITLE_FONT_SIZE,	//Font size
			initSubtitleSettings().textScale,	//Font size
			initSubtitleSettings().position,	//Text position
			
			initSubtitleSettings().textColor,
			
			initSubtitleSettings().BACKGROUND,	//Background
			initSubtitleSettings().subtitleBoundingBoxExtra,
			initSubtitleSettings().subtitleBoxColor,
	
			initSubtitleSettings().OUTLINE,	//Outline
			initSubtitleSettings().OUTLINE_DISTANCE,		//Outline thiccness (pixels)
			
			initSubtitleSettings().AROUND_SHADOW,	//Around shadow
			initSubtitleSettings().AROUND_SHADOW_DISTANCE,		//Around shadow distance
			
			initSubtitleSettings().RAINBOW	//Rainbow
		}, (SubtitleSettings){	//Nothing
			initSubtitleSettings().SUBTITLE_FONT_SIZE,	//Font size
			initSubtitleSettings().textScale,	//Font size
			initSubtitleSettings().position,	//Text position
			
			initSubtitleSettings().textColor,
			
			false,	//Background
			initSubtitleSettings().subtitleBoundingBoxExtra,
			initSubtitleSettings().subtitleBoxColor,
	
			false,	//Outline
			initSubtitleSettings().OUTLINE_DISTANCE,		//Outline thiccness (pixels)
			
			false,	//Around shadow
			initSubtitleSettings().AROUND_SHADOW_DISTANCE,		//Around shadow distance
			
			false	//Rainbow
		}, (SubtitleSettings){	//Background only
			initSubtitleSettings().SUBTITLE_FONT_SIZE,	//Font size
			initSubtitleSettings().textScale,	//Font size
			initSubtitleSettings().position,	//Text position
			
			initSubtitleSettings().textColor,
			
			true,	//Background
			initSubtitleSettings().subtitleBoundingBoxExtra,
			initSubtitleSettings().subtitleBoxColor,
	
			false,	//Outline
			initSubtitleSettings().OUTLINE_DISTANCE,		//Outline thiccness (pixels)
			
			false,	//Around shadow
			initSubtitleSettings().AROUND_SHADOW_DISTANCE,		//Around shadow distance
			
			false	//Rainbow
		}, (SubtitleSettings){	//Outline only
			initSubtitleSettings().SUBTITLE_FONT_SIZE,	//Font size
			initSubtitleSettings().textScale,	//Font size
			initSubtitleSettings().position,	//Text position
			
			initSubtitleSettings().textColor,
			
			false,	//Background
			initSubtitleSettings().subtitleBoundingBoxExtra,
			initSubtitleSettings().subtitleBoxColor,
	
			true,	//Outline
			initSubtitleSettings().OUTLINE_DISTANCE,		//Outline thiccness (pixels)
			
			false,	//Around shadow
			initSubtitleSettings().AROUND_SHADOW_DISTANCE,		//Around shadow distance
			
			false	//Rainbow
		}, (SubtitleSettings){	//Background and outline
			initSubtitleSettings().SUBTITLE_FONT_SIZE,	//Font size
			initSubtitleSettings().textScale,	//Font size
			initSubtitleSettings().position,	//Text position
			
			initSubtitleSettings().textColor,
			
			true,	//Background
			initSubtitleSettings().subtitleBoundingBoxExtra,
			initSubtitleSettings().subtitleBoxColor,
	
			true,	//Outline
			initSubtitleSettings().OUTLINE_DISTANCE,		//Outline thiccness (pixels)
			
			false,	//Around shadow
			initSubtitleSettings().AROUND_SHADOW_DISTANCE,		//Around shadow distance
			
			initSubtitleSettings().RAINBOW	//Rainbow
		}, /*(SubtitleSettings){	//Around shadow
			initSubtitleSettings().SUBTITLE_FONT_SIZE,	//Font size
			initSubtitleSettings().position,	//Text position
			initSubtitleSettings().OVERLAY_MODE,	//Overlay
			initSubtitleSettings().BORDERLESS_WINDOW_MODE,	//Borderless window (buggy)
			initSubtitleSettings().FRAME_RATE,	//Frame rate
			
			initSubtitleSettings().textColor,
			
			initSubtitleSettings().BACKGROUND,	//Background
			initSubtitleSettings().subtitleBoundingBoxExtra,
			initSubtitleSettings().subtitleBoxColor,
	
			initSubtitleSettings().OUTLINE,	//Outline
			initSubtitleSettings().OUTLINE_DISTANCE,		//Outline thiccness (pixels)
			
			initSubtitleSettings().AROUND_SHADOW,	//Around shadow
			initSubtitleSettings().AROUND_SHADOW_DISTANCE,		//Around shadow distance
			
			initSubtitleSettings().RAINBOW	//Rainbow
		},*/
		(SubtitleSettings){	//Rainbow
			initSubtitleSettings().SUBTITLE_FONT_SIZE,	//Font size
			initSubtitleSettings().textScale,	//Font size
			initSubtitleSettings().position,	//Text position
			
			initSubtitleSettings().textColor,
			
			initSubtitleSettings().BACKGROUND,	//Background
			initSubtitleSettings().subtitleBoundingBoxExtra,
			initSubtitleSettings().subtitleBoxColor,
	
			initSubtitleSettings().OUTLINE,	//Outline
			initSubtitleSettings().OUTLINE_DISTANCE,		//Outline thiccness (pixels)
			
			initSubtitleSettings().AROUND_SHADOW,	//Around shadow
			initSubtitleSettings().AROUND_SHADOW_DISTANCE,		//Around shadow distance
			
			true	//Rainbow
		}
	};
	//Spread them out
	//numSubtitles = 1;
	numSubtitles = (sizeof(bunchOfSettings) / sizeof(SubtitleSettings));
	for(size_t i = 0; i < numSubtitles; ++i){
		const Vector2 center = {GetScreenWidth() / 2, (GetScreenHeight() / 2) + 10};	//For whatever reason, I gotta add 10.
		const float turns = (float)i / (float)numSubtitles;
		const float radius = 300.0f;
		bunchOfSettings[i].position = (Vector2){center.x + cosf(turns * 2 * PI) * radius, center.y + sinf(turns * 2 * PI) * radius};
		//printVector2("Sub position", bunchOfSettings[i].position);
	}
	numSubtitles += numFonts;
	
	subtitleArray = (SubtitleInstance*)calloc(numSubtitles, sizeof(SubtitleInstance));
	assert(subtitleArray != NULL);
	for(size_t i = 0; i < numSubtitles - numFonts; ++i){
		subtitleArray[i] = initSubtitleInstance(bunchOfSettings[i], DEFAULT_FONT);
		subtitleArray[i].onclick = *SelectPreset;
	}
	for(size_t i = numSubtitles - numFonts; i < numSubtitles; ++i){
		const size_t fontId = i - (numSubtitles - numFonts);
		subtitleArray[i] = initSubtitleInstance(initSubtitleSettings(), fontId);
		const Vector2 padding = {200, 100};	//For whatever reason, I gotta add 10.
		subtitleArray[i].settings.position = (Vector2){padding.x, padding.y + (fontId * subtitleArray[i].settings.SUBTITLE_FONT_SIZE)};
		//printVector2("Sub position", bunchOfSettings[i].position);
		subtitleArray[i].onclick = *SelectFont;
		subtitleArray[i].settings.SUBTITLE_FONT_SIZE = fontSizes[fontId];
	}

	outlineShader = LoadShader(0, TextFormat("resources/shaders/glsl%i/outline.fs", GLSL_VERSION));
	aroundShadowShader = LoadShader(0, TextFormat("resources/shaders/glsl%i/aroundShadow.fs", GLSL_VERSION));

	//printf("Screen: (%d, %d)\t Monitor: (%d, %d)\n", GetScreenWidth(), GetScreenHeight(), GetMonitorWidth(0), GetMonitorHeight(0));
}

void UpdateSubtitles(){
	if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){	//Doesn't work if in OVERLAY mode
		for(size_t i = 0; i < numSubtitles; ++i){
			if(pointIsInRectangle(GetMousePosition(), subtitleInstanceDestination(subtitleArray[i]))){
				void (*onclick)(SubtitleInstance) = subtitleArray[i].onclick;
				if(onclick){
					(*onclick)(subtitleArray[i]);
				}
			}
		}
	}
	
	LoadLiveSubtitles();
	bool hoveringOver = false;
	for(size_t i = 0; i < numSubtitles; ++i){	//Only the first one
		if(IsCursorOnScreen()){
			if(pointIsInRectangle(GetMousePosition(), rectangleFromSizeCenteredAroundPosition(subtitleInstanceDestinationSize(subtitleArray[i]), subtitleArray[i].settings.position))){
				hoveringOver = true;
			}
		}
		if(isOverlayMode()){	//Sample text if not overlay
			//TODO: Implement wait time here
			subtitleArray[i].text = getLiveSubtitles();
		}
		UpdateSubtitleInstance(subtitleArray + i);
	}
	SetMouseCursor(hoveringOver? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
}

void DrawSubtitles(){
	if(isOverlayMode() && (!isThereLiveSubtitles())) return;
	for(size_t i = 0; i < numSubtitles; ++i){	//Only the first one
		DrawSubtitleInstance(subtitleArray[i]);
	}
}

void UpdateSubtitleInstance(SubtitleInstance* instance){
	Font* font = fontArray + instance->font;
	const Vector2 subtitleBoundingBox = MeasureTextEx(*font, instance->text, instance->settings.SUBTITLE_FONT_SIZE, 5);
	//printVector2("Subtitle bounding box", subtitleBoundingBox);
	const Vector2 subtitleBoundingBoxExtra = instance->settings.subtitleBoundingBoxExtra;
	//printVector2("Subtitle bounding box extra", subtitleBoundingBoxExtra);
	const Color subtitleTextColor = (instance->settings.RAINBOW)? ColorFromHSV(GetTime() * 500.0, 1, 1) : instance->settings.textColor;
	
	//const Vector2 subtitlePosition = {(subtitleBoundingBoxExtra.x / 2), (subtitleBoundingBoxExtra.y / 2) + subtitleBoundingBox.y};	//Position of the subtitles inside the texture buffer
	const Vector2 subtitlePosition = {subtitleBoundingBoxExtra.x / 2, subtitleBoundingBoxExtra.y / 2};
	//printVector2("Subtitle position", subtitlePosition);
	
	UnloadRenderTexture(instance->target);
	instance->targetDimensions = (Vector2){subtitleBoundingBox.x + subtitleBoundingBoxExtra.x, subtitleBoundingBox.y + subtitleBoundingBoxExtra.y};
	instance->target = LoadRenderTexture(instance->targetDimensions.x, instance->targetDimensions.y);
	//Drawing to the texture
	BeginTextureMode(instance->target);
	if(instance->settings.BACKGROUND){
		ClearBackground(instance->settings.subtitleBoxColor);
	}else{
		ClearBackground(BLANK);	//Still need to clear it to get rid of anything
	}
	DrawTextEx(*font, instance->text, subtitlePosition, instance->settings.SUBTITLE_FONT_SIZE, 5, subtitleTextColor);
	EndTextureMode();
}

void DrawSubtitleInstance(SubtitleInstance instance){
	const float scale = 0.5f;
	const Vector2 destinationSize = subtitleInstanceDestinationSize(instance);
	const Rectangle subtitleDestination = subtitleInstanceDestination(instance);
	printRectangle("Subtitle destination: ", subtitleDestination);
	
	if(instance.settings.OUTLINE){
		//We put this in the draw loop so the amount of border can change while running.
		const float tScale[2] = { (float)destinationSize.x, (float)destinationSize.y };
		SetShaderValue(outlineShader, GetShaderLocation(outlineShader, "textureSize"), tScale, SHADER_UNIFORM_VEC2);
		const float outlineSize = instance.settings.OUTLINE_DISTANCE;	//GLSL max int is 255
		SetShaderValue(outlineShader, GetShaderLocation(outlineShader, "outlineSize"), &outlineSize, SHADER_UNIFORM_FLOAT);
		const float outlineColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};	//Hoping that 1 is 100% opacity
		SetShaderValue(outlineShader, GetShaderLocation(outlineShader, "outlineColor"), outlineColor, SHADER_UNIFORM_VEC4);
		
		BeginShaderMode(outlineShader);
	}
	if(instance.settings.AROUND_SHADOW){
		//We put this in the draw loop so the amount of border can change while running.
		const float tScale[2] = { (float)destinationSize.x, (float)destinationSize.y };
		SetShaderValue(aroundShadowShader, GetShaderLocation(aroundShadowShader, "textureSize"), tScale, SHADER_UNIFORM_VEC2);
		const float outlineSize = instance.settings.AROUND_SHADOW_DISTANCE;	//GLSL max int is 255
		SetShaderValue(aroundShadowShader, GetShaderLocation(aroundShadowShader, "outlineSize"), &outlineSize, SHADER_UNIFORM_FLOAT);
		const float outlineColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};	//Hoping that 1 is 100% opacity
		SetShaderValue(aroundShadowShader, GetShaderLocation(aroundShadowShader, "outlineColor"), outlineColor, SHADER_UNIFORM_VEC4);
		
		BeginShaderMode(aroundShadowShader);
	}
	DrawTexturePro(
		instance.target.texture,
		(Rectangle){0, 0, instance.targetDimensions.x, -instance.targetDimensions.y},
		subtitleDestination,
		(Vector2){0, 0},
		0, //Rotation
		WHITE	//Color
	);
	if(instance.settings.OUTLINE || instance.settings.AROUND_SHADOW){
		EndShaderMode();
	}
}

void UnloadSubtitles(){
	for(size_t i = 0; i < numSubtitles; ++i){
		UnloadSubtitleInstance(subtitleArray[i]);
	}
	UnloadSubtitleInstance(subtitleArray[0]);
	
	free(subtitleArray);
	for(size_t i = 0; i < numFonts; ++i){
		UnloadFont(fontArray[i]);
	}
	free(fontArray);

	//UnloadShader(outlineShader);
	//UnloadShader(aroundShadowShader);
}