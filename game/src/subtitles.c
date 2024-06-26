#include "subtitles.h"	//Includes subtitle_settings.h too
#include "live_subtitles.h"
#include "listFiles.h"
#include <assert.h>
#include <math.h>       // Required for: sinf(), cosf(), tan(), atan2f(), sqrtf(), floor(), fminf(), fmaxf(), fabsf()

#define ifnt(x) if(!(x))
	
//#define COLOR_PRESETS
#ifndef COLOR_PRESETS
RenderTexture2D colorSpectrumTexture;
Rectangle colorSpectrumDestinations[3];
#endif

#ifndef COLOR_PRESETS
Shader colorSpectrumShader;
#endif
Shader outlineShader, aroundShadowShader;

//shut up, it works
Font* fontArray = NULL;
int numFonts = 0;
#define numFontSizes 7

SubtitleInstance* subtitleArray = NULL;
int numSubtitles = 0;

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

const float LOADED_FONT_SIZE = 90.0f;	//Some fonts might be giant, but you can make it smaller
void LoadFonts(){
	int extraCodePoints[] = {0x0025, 0x2018, 0x2019, 0x201A, 0x201B, 0x201C, 0x201D, 0x201E, 0x201F, 0x2047, 0x2048, 0x2049};
	size_t extraCodePointsSize = (sizeof(extraCodePoints) / sizeof(int));
	int* codepoints = (int *)RL_MALLOC((95*sizeof(int)) + extraCodePointsSize);
	for (int i = 0; i < 95; i++) codepoints[i] = i + 32;
	for (int i = 0; i < extraCodePointsSize; i++) codepoints[i] = extraCodePoints[i];
	
	char** fontPaths = listFiles("./resources/fonts/", &numFonts);
	//numFonts = FONT_ENUM_SIZE;	//Edit subtitle_settings.h enum to add a font.
	fontArray = (Font*)RL_CALLOC(numFonts, sizeof(Font));
	assert(fontArray != NULL);
	for(size_t i = 0; i < numFonts; ++i){
		fontArray[i] = LoadFontEx(fontPaths[i], LOADED_FONT_SIZE, codepoints, 95 + extraCodePointsSize);
		assert(IsFontReady(fontArray[i]));
	}
	
	RL_FREE(codepoints);
    freeFileList(fontPaths, numFonts);
}

void SelectPreset(SubtitleInstance bruh){
	const Vector2 center = {GetScreenWidth() / 2, (GetScreenHeight() / 2) + 10};	//For whatever reason, I gotta add 10.
	SubtitleSettings favorite = bruh.settings;
	
	setSubtitleSettings(favorite);
	
	UnloadSubtitles();	//Have to unload the subtitles because loadOverlayWindow closes the window, which unloads raylib.
	LoadOverlayWindow(true);
	//LoadSubtitles(favorite);	Why even do this
	LoadFonts();
	LoadShaders();
	
	subtitleArray = (SubtitleInstance*)RL_CALLOC(1, sizeof(SubtitleInstance));	//Load a single subtitle
	numSubtitles = 1;
	
	subtitleArray[0].font = bruh.font;
	subtitleArray[0].settings = favorite;	//Should already be like that
	// ^ Sets textScale too
	subtitleArray[0].settings.position = (Vector2){center.x, GetScreenHeight() - (favorite.SUBTITLE_FONT_SIZE * favorite.textScale * 1.5)};	//Center of the subtitles
	subtitleArray[0].text = "What the hell is this!";
}
void SelectFont(SubtitleInstance bruh){
	for(size_t i = 0; i < numSubtitles; ++i){
		if(subtitleArray[i].onclick != (*SelectFont)){
			subtitleArray[i].font = bruh.font;
			subtitleArray[i].hasToUpdate = true;
		}
	}
}
void SelectFontSize(SubtitleInstance bruh){
	for(size_t i = 0; i < numSubtitles; ++i){
		if(subtitleArray[i].onclick != (*SelectFontSize)){
			subtitleArray[i].settings.textScale = bruh.settings.textScale;
			subtitleArray[i].hasToUpdate = true;
		}
	}
}
void SelectTextColor(SubtitleInstance bruh){
	for(size_t i = 0; i < numSubtitles; ++i){
		if(subtitleArray[i].onclick != (*SelectTextColor)){
			subtitleArray[i].settings.textColor = bruh.settings.textColor;
			subtitleArray[i].settings.textRainbow = bruh.settings.textRainbow;
			subtitleArray[i].hasToUpdate = true;
		}
	}
}
void SelectBackgroundColor(SubtitleInstance bruh){
	for(size_t i = 0; i < numSubtitles; ++i){
		if(subtitleArray[i].onclick != (*SelectBackgroundColor)){
			subtitleArray[i].settings.subtitleBoxColor.r = bruh.settings.subtitleBoxColor.r;
			subtitleArray[i].settings.subtitleBoxColor.g = bruh.settings.subtitleBoxColor.g;
			subtitleArray[i].settings.subtitleBoxColor.b = bruh.settings.subtitleBoxColor.b;
			subtitleArray[i].settings.subtitleBoxRainbow = bruh.settings.subtitleBoxRainbow;
			subtitleArray[i].hasToUpdate = true;
		}
	}
}
void SelectOutlineColor(SubtitleInstance bruh){
	for(size_t i = 0; i < numSubtitles; ++i){
		if(subtitleArray[i].onclick != (*SelectOutlineColor)){
			subtitleArray[i].settings.outlineColor.r = bruh.settings.outlineColor.r;
			subtitleArray[i].settings.outlineColor.g = bruh.settings.outlineColor.g;
			subtitleArray[i].settings.outlineColor.b = bruh.settings.outlineColor.b;
			subtitleArray[i].settings.outlineRainbow = bruh.settings.outlineRainbow;
			subtitleArray[i].hasToUpdate = true;
		}
	}
}

void LoadMenu(SubtitleSettings settings){
	SubtitleSettings bunchOfSettings[] = {
		(SubtitleSettings){	//Default (translucent background with white text)
			settings.SUBTITLE_FONT_SIZE,	//Font size
			settings.textScale,	//Font size
			settings.position,	//Text position
			
			settings.textColor,
			settings.textRainbow,
			
			settings.BACKGROUND,	//Background
			settings.subtitleBoundingBoxExtra,
			settings.subtitleBoxColor,
			settings.subtitleBoxRainbow,
	
			settings.OUTLINE,	//Outline
			settings.outlineColor,	//Outline
			settings.outlineRainbow,
			settings.OUTLINE_DISTANCE,		//Outline thiccness (pixels)
			
			settings.AROUND_SHADOW,	//Around shadow
			settings.AROUND_SHADOW_DISTANCE,		//Around shadow distance
			
		}, (SubtitleSettings){	//Nothing
			settings.SUBTITLE_FONT_SIZE,	//Font size
			settings.textScale,	//Font size
			settings.position,	//Text position
			
			settings.textColor,
			settings.textRainbow,
			
			false,	//Background
			settings.subtitleBoundingBoxExtra,
			settings.subtitleBoxColor,
			settings.subtitleBoxRainbow,
	
			false,	//Outline
			settings.outlineColor,	//Outline
			settings.outlineRainbow,
			settings.OUTLINE_DISTANCE,		//Outline thiccness (pixels)
			
			false,	//Around shadow
			settings.AROUND_SHADOW_DISTANCE,		//Around shadow distance
		}, (SubtitleSettings){	//Background full opaque
			settings.SUBTITLE_FONT_SIZE,	//Font size
			settings.textScale,	//Font size
			settings.position,	//Text position
			
			settings.textColor,
			settings.textRainbow,
			
			true,	//Background
			settings.subtitleBoundingBoxExtra,
			BLACK,
			settings.subtitleBoxRainbow,
	
			false,	//Outline
			settings.outlineColor,	//Outline
			settings.outlineRainbow,
			settings.OUTLINE_DISTANCE,		//Outline thiccness (pixels)
			
			false,	//Around shadow
			settings.AROUND_SHADOW_DISTANCE,		//Around shadow distance
			
		}, (SubtitleSettings){	//Outline only
			settings.SUBTITLE_FONT_SIZE,	//Font size
			settings.textScale,	//Font size
			settings.position,	//Text position
			
			settings.textColor,
			settings.textRainbow,
			
			false,	//Background
			settings.subtitleBoundingBoxExtra,
			settings.subtitleBoxColor,
			settings.subtitleBoxRainbow,
	
			true,	//Outline
			BLACK,	//Outline
			settings.outlineRainbow,
			settings.OUTLINE_DISTANCE,		//Outline thiccness (pixels)
			
			false,	//Around shadow
			settings.AROUND_SHADOW_DISTANCE,		//Around shadow distance
			
		}, (SubtitleSettings){	//Background and outline
			settings.SUBTITLE_FONT_SIZE,	//Font size
			settings.textScale,	//Font size
			settings.position,	//Text position
			
			settings.textColor,
			settings.textRainbow,
			
			true,	//Background
			settings.subtitleBoundingBoxExtra,
			settings.subtitleBoxColor,
			settings.subtitleBoxRainbow,
	
			true,	//Outline
			BLACK,	//Outline
			settings.outlineRainbow,
			settings.OUTLINE_DISTANCE,		//Outline thiccness (pixels)
			
			false,	//Around shadow
			settings.AROUND_SHADOW_DISTANCE,		//Around shadow distance
			
		}, (SubtitleSettings){	//Around shadow
			settings.SUBTITLE_FONT_SIZE,	//Font size
			settings.textScale,	//Font size
			settings.position,	//Text position
			
			settings.textColor,
			settings.textRainbow,
			
			false,	//Background
			settings.subtitleBoundingBoxExtra,
			settings.subtitleBoxColor,
			settings.subtitleBoxRainbow,
	
			false,	//Outline
			settings.outlineColor,	//Outline
			settings.outlineRainbow,
			settings.OUTLINE_DISTANCE,		//Outline thiccness (pixels)
			
			true,	//Around shadow
			settings.AROUND_SHADOW_DISTANCE,		//Around shadow distance
		}
	};
	//Spread them out
	//numSubtitles = 1;
	const size_t numPresets = (sizeof(bunchOfSettings) / sizeof(SubtitleSettings));
	numSubtitles = numPresets;
	for(size_t i = 0; i < numSubtitles; ++i){
		const Vector2 center = {(GetScreenWidth() / 2) - 100, (GetScreenHeight() / 2) + 10};	//For whatever reason, I gotta add 10.
		const float turns = (float)i / (float)numSubtitles;
		const float radius = 300.0f;
		bunchOfSettings[i].position = (Vector2){center.x + cosf(turns * 2 * PI) * radius, center.y + sinf(turns * 2 * PI) * radius};
		//printVector2("Sub position", bunchOfSettings[i].position);
	}
	#ifdef COLOR_PRESETS
	Color customizableColors[] = {LIGHTGRAY,
		//GRAY,
		//DARKGRAY,
		YELLOW,
		GOLD,
		ORANGE,
		//PINK,
		RED,
		MAROON,
		GREEN,
		LIME,
		DARKGREEN,
		SKYBLUE,
		BLUE,
		DARKBLUE,
		PURPLE,
		VIOLET,
		DARKPURPLE,
		BEIGE,
		//BROWN,
		//DARKBROWN,
		WHITE,
		BLACK,
		//BLANK,	//If you want to disable an option, pick the matching preset in the center.
		MAGENTA
	};
	size_t customizableColorsSize = (sizeof(customizableColors) / sizeof(Color));
	numSubtitles += (customizableColorsSize * 3U) + 3;	//*3U because outline, text color and background color + 3 rainbow
	#endif
	
	LoadFonts();
	numSubtitles += numFonts;
	numSubtitles += numFontSizes;
	
	subtitleArray = (SubtitleInstance*)calloc(numSubtitles, sizeof(SubtitleInstance));
	assert(subtitleArray != NULL);
	//-----------	Presets
	const char* presetTitles[] = {
		"Translucent Background",
		"No Background",
		"Solid Background",
		"Outline Only",
		"Background and Outline",
		"Around Shadow"
	};
	assert((sizeof(presetTitles) / sizeof(const char*)) >= numPresets);
	for(size_t i = 0; i < numPresets; ++i){
		subtitleArray[i] = initSubtitleInstance(bunchOfSettings[i], DEFAULT_FONT);
		subtitleArray[i].onclick = *SelectPreset;
		subtitleArray[i].text = presetTitles[i];
	}
	
	const float spread = 1.5f;
	
	//-----------	Font choices
	SubtitleInstance* fontChoices = subtitleArray + numPresets;
	for(size_t i = 0; i < numFonts; ++i){
		fontChoices[i] = initSubtitleInstance(settings, i);
		const Vector2 padding = {200, 100};	//For whatever reason, I gotta add 10.
		fontChoices[i].settings.position = (Vector2){padding.x, padding.y + (i * spread * fontChoices[i].settings.SUBTITLE_FONT_SIZE * fontChoices[i].settings.textScale)};
		//printVector2("Sub position", bunchOfSettings[i].position);
		fontChoices[i].onclick = *SelectFont;
		fontChoices[i].settings.SUBTITLE_FONT_SIZE = LOADED_FONT_SIZE;
		fontChoices[i].settings.OUTLINE = true;
		fontChoices[i].settings.outlineColor = BLACK;
	}
	
	//-----------	Font choices
	SubtitleInstance* fontSizes = fontChoices + numFonts;
	const char* labels[numFontSizes] = {"40%", "45%", "50%", "55%", "60%", "65%", "70%"};
	//assert(sizeof(labels)/sizeof(const char*) >= numFontSizes);
	for(size_t i = 0; i < numFontSizes; ++i){
		fontSizes[i] = initSubtitleInstance(settings, 0);
		const Vector2 padding = {200, 200};	//For whatever reason, I gotta add 10.
		fontSizes[i].settings.textScale = (.40f) + (i * .05);
		fontSizes[i].settings.position = (Vector2){padding.x, padding.y + (GetScreenHeight() / 4.0f) + (i * spread * LOADED_FONT_SIZE * fontSizes[i].settings.textScale)};
		//printVector2("Sub position", bunchOfSettings[i].position);
		fontSizes[i].onclick = *SelectFontSize;
		fontSizes[i].text = labels[i];
		
		//TODO: Investigate missing numbers
		//TODO: give text like "25%", "50%", etc
	}
	
	#ifdef COLOR_PRESETS
	SubtitleInstance* outlineColorChoices = fontSizes + numFontSizes;
	for(size_t i = 0; i < customizableColorsSize + 1; ++i){
		outlineColorChoices[i] = initSubtitleInstance(settings, DEFAULT_FONT);
		const Vector2 padding = {300, 50};	//For whatever reason, I gotta add 10.
		outlineColorChoices[i].settings.position = (Vector2){GetScreenWidth() - padding.x, padding.y + (i * spread * outlineColorChoices[i].settings.SUBTITLE_FONT_SIZE * outlineColorChoices[i].settings.textScale)};
		//const Vector2 center = {GetScreenWidth() / 2, (GetScreenHeight() / 2) + 10};	//For whatever reason, I gotta add 10.
		//outlineColorChoices[i].settings.position = (Vector2){GetScreenWidth() - padding.x, center.y + (i * outlineColorChoices[i].settings.SUBTITLE_FONT_SIZE * outlineColorChoices[i].settings.textScale)};
		//printVector2("Sub position", bunchOfSettings[i].position);
		outlineColorChoices[i].onclick = *SelectOutlineColor;
		if(i < customizableColorsSize){
			outlineColorChoices[i].settings.outlineColor = customizableColors[i];
		}else{
			outlineColorChoices[i].settings.outlineRainbow = true;
		}
		outlineColorChoices[i].settings.OUTLINE = true;
	}
	
	SubtitleInstance* backgroundColorChoices = outlineColorChoices + customizableColorsSize + 1;	//+1 for rainbow
	for(size_t i = 0; i < customizableColorsSize + 1; ++i){
		backgroundColorChoices[i] = initSubtitleInstance(settings, DEFAULT_FONT);
		const Vector2 padding = {200, 50};	//For whatever reason, I gotta add 10.
		backgroundColorChoices[i].settings.position = (Vector2){GetScreenWidth() - padding.x, padding.y + (i * spread * backgroundColorChoices[i].settings.SUBTITLE_FONT_SIZE * backgroundColorChoices[i].settings.textScale)};
		//const Vector2 center = {GetScreenWidth() / 2, (GetScreenHeight() / 2) + 10};	//For whatever reason, I gotta add 10.
		//backgroundColorChoices[i].settings.position = (Vector2){GetScreenWidth() - padding.x, center.y + (i * backgroundColorChoices[i].settings.SUBTITLE_FONT_SIZE * backgroundColorChoices[i].settings.textScale)};
		//printVector2("Sub position", bunchOfSettings[i].position);
		backgroundColorChoices[i].onclick = *SelectBackgroundColor;
		if(i < customizableColorsSize){
			backgroundColorChoices[i].settings.subtitleBoxColor = customizableColors[i];
		}else{
			backgroundColorChoices[i].settings.subtitleBoxRainbow = true;
		}
	}
	
	SubtitleInstance* textColorChoices = backgroundColorChoices + customizableColorsSize + 1;
	for(size_t i = 0; i < customizableColorsSize + 1; ++i){
		textColorChoices[i] = initSubtitleInstance(settings, DEFAULT_FONT);
		const Vector2 padding = {100, 50};	//For whatever reason, I gotta add 10.
		textColorChoices[i].settings.position = (Vector2){GetScreenWidth() - padding.x, padding.y + (i * spread * textColorChoices[i].settings.SUBTITLE_FONT_SIZE * textColorChoices[i].settings.textScale)};
		//printVector2("Sub position", bunchOfSettings[i].position);
		textColorChoices[i].onclick = *SelectTextColor;
		if(i < customizableColorsSize){
			textColorChoices[i].settings.textColor = customizableColors[i];
		}else{
			textColorChoices[i].settings.textRainbow = true;
		}
	}
	#else
	//if COLOR_PRESETS is not defined
	for(size_t i = 0; i < 3; ++i){
		colorSpectrumDestinations[i] = rectangleFromSizeCenteredAroundPosition((Vector2){255 * 2, 255}, (Vector2){(GetScreenWidth() - 255), ((GetScreenHeight() * (i + 1)) / 4)});
	}
	colorSpectrumTexture = LoadRenderTexture(colorSpectrumDestinations[0].width, colorSpectrumDestinations[0].height);
	
	//Init texture
	RenderTexture2D whitePixel = LoadRenderTexture(1, 1);
	BeginTextureMode(whitePixel);
	ClearBackground(WHITE);	//Hopefully this just writes every pixel.
	EndTextureMode();
	
	BeginTextureMode(colorSpectrumTexture);
	BeginShaderMode(colorSpectrumShader);
	
	DrawTexturePro(whitePixel.texture,
		(Rectangle){0.0, 0.0, 1.0, 1.0},
		(Rectangle){0.0, 0.0, colorSpectrumDestinations[0].width, colorSpectrumDestinations[0].height},
		(Vector2){0, 0},
		0,
		WHITE
	);
	
	EndShaderMode();
	EndTextureMode();
	EndDrawing();
	UnloadRenderTexture(whitePixel);
	#endif

	//printf("Screen: (%d, %d)\t Monitor: (%d, %d)\n", GetScreenWidth(), GetScreenHeight(), GetMonitorWidth(0), GetMonitorHeight(0));
}

void UpdateSubtitles(){
	bool hoveringOver = false;
	if(IsCursorOnScreen()){
		#pragma omp parallel for
		for(size_t i = 0; i < numSubtitles; ++i){
			if(pointIsInRectangle(GetMousePosition(), rectangleFromSizeCenteredAroundPosition(subtitleInstanceDestinationSize(subtitleArray[i]), subtitleArray[i].settings.position))){
				if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){	//Doesn't work if in OVERLAY mode
					void (*onclick)(SubtitleInstance) = subtitleArray[i].onclick;
					if(onclick){
						#pragma omp critical
						(*onclick)(subtitleArray[i]);	//Could possibly cause problems if two subtitles overlap
					}
				}
				hoveringOver = true;
			}
		}
	}
	
	#ifndef COLOR_PRESETS
	if(!isOverlayMode()){
		for(int i = 0; i < 3; ++i){
			if(pointIsInRectangle(GetMousePosition(), colorSpectrumDestinations[i])){
				if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){	//Doesn't work if in OVERLAY mode
					Vector2 uv = (Vector2){(GetMousePosition().x - colorSpectrumDestinations[i].x) / colorSpectrumDestinations[i].width, (GetMousePosition().y - colorSpectrumDestinations[i].y) / colorSpectrumDestinations[i].height};
					uv.y = 1.0 - uv.y;
					Vector3 hsv = (uv.y < .5)? (Vector3){uv.x,1.0, (uv.y * 2.0)} : (Vector3){uv.x, 2.0 - (uv.y * 2.0), 1.0};
					Color fromSpectrum = ColorFromHSV(hsv.x * 360.0, hsv.y, hsv.z);
					//Color fromSpectrum = (Color){uv.x * 255, uv.y * 255, 0, 255};	//Mango
					SubtitleSettings settings = initSubtitleSettings();
					SubtitleInstance bruh;
					bruh.settings = settings;
					switch(i){
						case 0:
							bruh.settings.textColor = fromSpectrum;
							SelectTextColor(bruh);
							break;
						case 1:
							bruh.settings.subtitleBoxColor = fromSpectrum;
							SelectBackgroundColor(bruh);
							break;
						case 2:
							bruh.settings.outlineColor = fromSpectrum;
							SelectOutlineColor(bruh);
							break;
					}
				}
				hoveringOver = true;
			}
		}
	}
	#endif
	
	LoadLiveSubtitles();
	const bool updateSubtitleText = thereIsLiveSubtitles();
	#pragma omp parallel for
	for(size_t i = 0; i < numSubtitles; ++i){	//Only the first one
		if(isOverlayMode() && updateSubtitleText){	//Sample text if not overlay
			//TODO: Implement wait time here
			subtitleArray[i].text = getLiveSubtitles();
			subtitleArray[i].hasToUpdate = true;
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
	
	#ifndef COLOR_PRESETS
	//BeginShaderMode(colorSpectrumShader);
	if(!isOverlayMode()){
		for(int i = 0; i < 3; ++i){
			//DrawRectangleRec(colorSpectrumDestinations[i], WHITE);
			DrawTexturePro(
				colorSpectrumTexture.texture,
				(Rectangle){0, 0, colorSpectrumDestinations[i].width, colorSpectrumDestinations[i].height},
				colorSpectrumDestinations[i],
				(Vector2){0, 0},
				0, //Rotation
				WHITE	//Color
			);
		}
	}
	//EndShaderMode();
	#endif
}

bool vector2Equals(const Vector2 a, const Vector2 b){
	return a.x == b.x && a.y == b.y;
}

void UpdateSubtitleInstance(SubtitleInstance* instance){
	ifnt(instance->hasToUpdate || subtitleInstanceIsRainbow(*instance)) return;
	instance->hasToUpdate = false;
	
	Font* font = fontArray + instance->font;
	const Vector2 subtitleBoundingBox = MeasureTextEx(*font, instance->text, instance->settings.SUBTITLE_FONT_SIZE * instance->settings.textScale, 5);
	//printVector2("Subtitle bounding box", subtitleBoundingBox);
	const Vector2 subtitleBoundingBoxExtra = instance->settings.subtitleBoundingBoxExtra;
	//printVector2("Subtitle bounding box extra", subtitleBoundingBoxExtra);
	const Color subtitleTextColor = (instance->settings.textRainbow)? ColorFromHSV(GetTime() * 500.0, 1, 1) : instance->settings.textColor;
	
	//const Vector2 subtitlePosition = {(subtitleBoundingBoxExtra.x / 2), (subtitleBoundingBoxExtra.y / 2) + subtitleBoundingBox.y};	//Position of the subtitles inside the texture buffer
	const Vector2 subtitlePosition = {subtitleBoundingBoxExtra.x / 2, subtitleBoundingBoxExtra.y / 2};
	//printVector2("Subtitle position", subtitlePosition);
	
	Vector2 newTargetDimensions = (Vector2){subtitleBoundingBox.x + subtitleBoundingBoxExtra.x, subtitleBoundingBox.y + subtitleBoundingBoxExtra.y};
	ifnt(vector2Equals(instance->targetDimensions, newTargetDimensions)){
		UnloadRenderTexture(instance->target);
		instance->targetDimensions = newTargetDimensions;
		instance->target = LoadRenderTexture(instance->targetDimensions.x, instance->targetDimensions.y);
	}
	//Drawing to the texture
	BeginTextureMode(instance->target);
	if(instance->settings.BACKGROUND){
		Color backgroundColor = (instance->settings.subtitleBoxRainbow)? ColorFromHSV(GetTime() * 500.0, 1, 1) : instance->settings.subtitleBoxColor;
		if(instance->settings.subtitleBoxRainbow) backgroundColor.a = instance->settings.subtitleBoxColor.a;
		ClearBackground(backgroundColor);
	}else{
		ClearBackground(BLANK);	//Still need to clear it to get rid of anything
	}
	DrawTextEx(*font, instance->text, subtitlePosition, instance->settings.SUBTITLE_FONT_SIZE * instance->settings.textScale, 5, subtitleTextColor);
	EndTextureMode();
}

void DrawSubtitleInstance(SubtitleInstance instance){
	const float scale = 0.5f;
	const Vector2 destinationSize = subtitleInstanceDestinationSize(instance);
	const Rectangle subtitleDestination = subtitleInstanceDestination(instance);
	//printRectangle("Subtitle destination: ", subtitleDestination);
	
	if(instance.settings.OUTLINE){
		//We put this in the draw loop so the amount of border can change while running.
		const float tScale[2] = { (float)destinationSize.x, (float)destinationSize.y };
		SetShaderValue(outlineShader, GetShaderLocation(outlineShader, "textureSize"), tScale, SHADER_UNIFORM_VEC2);
		const float outlineSize = instance.settings.OUTLINE_DISTANCE;	//GLSL max int is 255
		SetShaderValue(outlineShader, GetShaderLocation(outlineShader, "outlineSize"), &outlineSize, SHADER_UNIFORM_FLOAT);
		float* outlineColor = RaylibColorToShaderColor((instance.settings.outlineRainbow)? ColorFromHSV(GetTime() * 500.0, 1, 1) : instance.settings.outlineColor);	//Hoping that 1 is 100% opacity
		SetShaderValue(outlineShader, GetShaderLocation(outlineShader, "outlineColor"), outlineColor, SHADER_UNIFORM_VEC4);
		RL_FREE(outlineColor);	//I hope we don't need this still
		
		BeginShaderMode(outlineShader);
	}
	if(instance.settings.AROUND_SHADOW){
		//We put this in the draw loop so the amount of border can change while running.
		const float tScale[2] = { (float)destinationSize.x, (float)destinationSize.y };
		SetShaderValue(aroundShadowShader, GetShaderLocation(aroundShadowShader, "textureSize"), tScale, SHADER_UNIFORM_VEC2);
		const float outlineSize = instance.settings.AROUND_SHADOW_DISTANCE;	//GLSL max int is 255
		SetShaderValue(aroundShadowShader, GetShaderLocation(aroundShadowShader, "maxDistance"), &outlineSize, SHADER_UNIFORM_FLOAT);
		const float spread = 1.0f;
		SetShaderValue(aroundShadowShader, GetShaderLocation(aroundShadowShader, "spread"), &spread, SHADER_UNIFORM_FLOAT);
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

void LoadShaders(){
	#ifndef COLOR_PRESETS
	colorSpectrumShader = LoadShader(0, TextFormat("resources/shaders/glsl%i/colorSpectrum.fs", GLSL_VERSION));
	#endif
	outlineShader = LoadShader(0, TextFormat("resources/shaders/glsl%i/outline.fs", GLSL_VERSION));
	aroundShadowShader = LoadShader(0, TextFormat("resources/shaders/glsl%i/aroundShadow.fs", GLSL_VERSION));
}

void UnloadShaders(){
	#ifndef COLOR_PRESETS
	UnloadShader(colorSpectrumShader);
	#endif
	UnloadShader(outlineShader);
	UnloadShader(aroundShadowShader);
}

void ReloadShaders(){
	UnloadShaders();
	LoadShaders();
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

	UnloadShaders();
}