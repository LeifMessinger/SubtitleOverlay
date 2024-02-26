#include "subtitles.h"	//Includes subtitle_settings.h too
#include "live_subtitles.h"
#include <assert.h>
#include <math.h>       // Required for: sinf(), cosf(), tan(), atan2f(), sqrtf(), floor(), fminf(), fmaxf(), fabsf()

Shader outlineShader, aroundShadowShader;

Font subtitleFont;
Font** fontArray = NULL;
int numFonts = 0;

SubtitleInstance* subtitleArray = NULL;
int numSubtitles = 0;

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

void LoadSubtitles(SubtitleSettings settings){
	subtitleFont = LoadFontEx("resources/fonts/RoadgeekMittelschrift.ttf", settings.SUBTITLE_FONT_SIZE, NULL, 0);
	assert(IsFontReady(subtitleFont));
	
	fontArray = (Font**)calloc(1, sizeof(Font*));
	assert(fontArray != NULL);
	fontArray[0] = &subtitleFont;
	numFonts = 1;
	
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
	
	subtitleArray = (SubtitleInstance*)calloc(numSubtitles, sizeof(SubtitleInstance));
	assert(subtitleArray != NULL);
	for(size_t i = 0; i < numSubtitles; ++i){
		subtitleArray[i] = initSubtitleInstance(bunchOfSettings[i], DEFAULT_FONT);
	}

	outlineShader = LoadShader(0, TextFormat("resources/shaders/glsl%i/outline.fs", GLSL_VERSION));
	aroundShadowShader = LoadShader(0, TextFormat("resources/shaders/glsl%i/aroundShadow.fs", GLSL_VERSION));

	//printf("Screen: (%d, %d)\t Monitor: (%d, %d)\n", GetScreenWidth(), GetScreenHeight(), GetMonitorWidth(0), GetMonitorHeight(0));
}

void UpdateSubtitles(){
	if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){	//Doesn't work if in OVERLAY mode
		for(size_t i = 0; i < numSubtitles; ++i){
			if(pointIsInRectangle(GetMousePosition(), subtitleInstanceDestination(subtitleArray[i]))){
		const Vector2 center = {GetScreenWidth() / 2, (GetScreenHeight() / 2) + 10};	//For whatever reason, I gotta add 10.
				SubtitleSettings favorite = subtitleArray[i].settings;
				
				UnloadSubtitles();
				LoadOverlayWindow(true);
				LoadSubtitles(favorite);
				
				subtitleArray[0].settings = favorite;	//Should already be like that
				subtitleArray[0].settings.position = (Vector2){center.x, GetScreenHeight() - (favorite.SUBTITLE_FONT_SIZE * favorite.textScale * 1.5)};	//Center of the subtitles
				numSubtitles = 1;	//This'll probably cause a memory leak, but who cares if it's at the end of the program.
				subtitleArray[0].text = "What the hell is this!";
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
	Font* font = fontArray[instance->font];
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
	UnloadFont(subtitleFont);
	UnloadSubtitleInstance(subtitleArray[0]);
	free(subtitleArray);
	free(fontArray);

	//UnloadShader(outlineShader);
	//UnloadShader(aroundShadowShader);
}