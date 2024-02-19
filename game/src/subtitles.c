#include "subtitles.h"

RenderTexture2D target;
Vector2 targetDimensions = {1, 1};
Font subtitleFont;
Shader outlineShader, aroundShadowShader;

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

void LoadSubtitles(SubtitleSettings settings){
	subtitleFont = LoadFontEx("resources/fonts/RoadgeekMittelschrift.ttf", settings.SUBTITLE_FONT_SIZE, NULL, 0);
	
	target = LoadRenderTexture(1, 1);

	outlineShader = LoadShader(0, TextFormat("resources/shaders/glsl%i/outline.fs", GLSL_VERSION));
	aroundShadowShader = LoadShader(0, TextFormat("resources/shaders/glsl%i/aroundShadow.fs", GLSL_VERSION));

	//printf("Screen: (%d, %d)\t Monitor: (%d, %d)\n", GetScreenWidth(), GetScreenHeight(), GetMonitorWidth(0), GetMonitorHeight(0));
}

void UpdateSubtitleTexture(const char* subtitleText, SubtitleSettings settings){
	const Vector2 subtitleBoundingBox = MeasureTextEx(subtitleFont, subtitleText, settings.SUBTITLE_FONT_SIZE, 5);
	const Vector2 subtitleBoundingBoxExtra = {30, 40};
	const Color subtitleBoxColor = {0, 0, 0, 220};
	const Color subtitleTextColor = (settings.RAINBOW)? ColorFromHSV(GetTime() * 500.0, 1, 1) : WHITE;
	
	const Vector2 subtitlePosition = {(subtitleBoundingBoxExtra.x / 2), (subtitleBoundingBoxExtra.y / 2) + subtitleBoundingBox.y};	//Position of the subtitles inside the texture buffer
	
	UnloadRenderTexture(target);
	targetDimensions = (Vector2){subtitleBoundingBox.x + subtitleBoundingBoxExtra.x, subtitleBoundingBox.y + subtitleBoundingBoxExtra.y};
	target = LoadRenderTexture(targetDimensions.x, targetDimensions.y);
	//Drawing to the texture
	BeginTextureMode(target);
	if(settings.BACKGROUND){
		ClearBackground(subtitleBoxColor);
	}else{
		ClearBackground(BLANK);	//Still need to clear it to get rid of anything
	}
	DrawTextEx(subtitleFont, subtitleText, (Vector2){subtitleBoundingBoxExtra.x / 2, subtitleBoundingBoxExtra.y / 2}, settings.SUBTITLE_FONT_SIZE, 5, subtitleTextColor);
	EndTextureMode();
}

Rectangle rectangleFromSizeAndPosition(Vector2 size, Vector2 position){
	return (Rectangle){position.x, position.y, size.x, size.y};
}
Rectangle rectangleFromSizeCenteredAroundPosition(Vector2 size, Vector2 position){
	return rectangleFromSizeAndPosition(size, (Vector2){position.x - (size.x / 2), position.y - (size.y / 2)});
}

bool pointIsInRectangle(Vector2 point, Rectangle rect){
	return (point.x >= rect.x && point.x <= rect.x + rect.width) && (point.y >= rect.y && point.y <= rect.y + rect.height);
}

void DrawSubtitleTexture(SubtitleSettings settings){
	BeginDrawing();
	//Drawing the texture to the frame buffer
	if(settings.OVERLAY_MODE){
		ClearBackground(BLANK);
	}else{
		ClearBackground(PINK);
	}
	const Vector2 center = {GetScreenWidth() / 2, (GetScreenHeight() / 2) + 10};	//For whatever reason, I gotta add 10.
	const float scale = 0.5f;
	const Vector2 destinationSize = {targetDimensions.x * scale, targetDimensions.y * scale};
	const Vector2 subtitlePosition = {center.x, GetScreenHeight() - (destinationSize.y * 1.5)};	//Center of the subtitles
	const Rectangle subtitleDestination = rectangleFromSizeCenteredAroundPosition(destinationSize, subtitlePosition);
	
	if(settings.OUTLINE){
		//We put this in the draw loop so the amount of border can change while running.
		const float tScale[2] = { (float)destinationSize.x, (float)destinationSize.y };
		SetShaderValue(outlineShader, GetShaderLocation(outlineShader, "textureSize"), tScale, SHADER_UNIFORM_VEC2);
		const float outlineSize = settings.OUTLINE_DISTANCE;	//GLSL max int is 255
		SetShaderValue(outlineShader, GetShaderLocation(outlineShader, "outlineSize"), &outlineSize, SHADER_UNIFORM_FLOAT);
		const float outlineColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};	//Hoping that 1 is 100% opacity
		SetShaderValue(outlineShader, GetShaderLocation(outlineShader, "outlineColor"), outlineColor, SHADER_UNIFORM_VEC4);
		
		BeginShaderMode(outlineShader);
	}
	if(settings.AROUND_SHADOW){
		//We put this in the draw loop so the amount of border can change while running.
		const float tScale[2] = { (float)destinationSize.x, (float)destinationSize.y };
		SetShaderValue(aroundShadowShader, GetShaderLocation(aroundShadowShader, "textureSize"), tScale, SHADER_UNIFORM_VEC2);
		const float outlineSize = settings.AROUND_SHADOW_DISTANCE;	//GLSL max int is 255
		SetShaderValue(aroundShadowShader, GetShaderLocation(aroundShadowShader, "outlineSize"), &outlineSize, SHADER_UNIFORM_FLOAT);
		const float outlineColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};	//Hoping that 1 is 100% opacity
		SetShaderValue(aroundShadowShader, GetShaderLocation(aroundShadowShader, "outlineColor"), outlineColor, SHADER_UNIFORM_VEC4);
		
		BeginShaderMode(aroundShadowShader);
	}
	DrawTexturePro(
		target.texture,
		(Rectangle){0, 0, targetDimensions.x, -targetDimensions.y},
		subtitleDestination,
		(Vector2){0, 0},
		0, //Rotation
		WHITE	//Color
	);
	if(settings.OUTLINE || settings.AROUND_SHADOW){
		EndShaderMode();
	}
	
	if(IsCursorOnScreen()){
		if(pointIsInRectangle(GetMousePosition(), subtitleDestination)){
			SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
		}else{
			SetMouseCursor(MOUSE_CURSOR_DEFAULT);
		}
	}
	
	EndDrawing();
}

void UnloadSubtitles(){
	UnloadFont(subtitleFont);
	UnloadRenderTexture(target);

	UnloadShader(outlineShader);
	UnloadShader(aroundShadowShader);
}