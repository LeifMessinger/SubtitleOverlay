#include "subtitles.h"
#include "my_config.h"

RenderTexture2D target;
Vector2 targetDimensions = {1, 1};
Font subtitleFont;
const int SUBTITLE_FONT_SIZE = 50;

void LoadSubtitles(){
	subtitleFont = LoadFontEx("resources/RoadgeekMittelschrift.ttf", SUBTITLE_FONT_SIZE, NULL, 0);
	
	target = LoadRenderTexture(1, 1);

	//printf("Screen: (%d, %d)\t Monitor: (%d, %d)\n", GetScreenWidth(), GetScreenHeight(), GetMonitorWidth(0), GetMonitorHeight(0));
}

void UpdateSubtitleTexture(const char* subtitleText){
	const Vector2 subtitleBoundingBox = MeasureTextEx(subtitleFont, subtitleText, SUBTITLE_FONT_SIZE, 5);
	const Vector2 subtitleBoundingBoxExtra = {20, 10};
	const Color subtitleBoxColor = {0, 0, 0, 220};
	
	const Vector2 subtitlePosition = {(subtitleBoundingBoxExtra.x / 2), (subtitleBoundingBoxExtra.y / 2) + subtitleBoundingBox.y};	//Position of the subtitles inside the texture buffer
	
	UnloadRenderTexture(target);
	targetDimensions = (Vector2){subtitleBoundingBox.x + subtitleBoundingBoxExtra.x, subtitleBoundingBox.y + subtitleBoundingBoxExtra.y};
	target = LoadRenderTexture(targetDimensions.x, targetDimensions.y);
	//Drawing to the texture
	BeginTextureMode(target);
	ClearBackground(subtitleBoxColor);
	DrawTextEx(subtitleFont, subtitleText, (Vector2){subtitleBoundingBoxExtra.x / 2, subtitleBoundingBoxExtra.y / 2}, SUBTITLE_FONT_SIZE, 5, WHITE);
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

void DrawSubtitleTexture(){
	BeginDrawing();
	//Drawing the texture to the frame buffer
	if(OVERLAY_MODE){
		ClearBackground(BLANK);
	}else{
		ClearBackground(PINK);
	}
	const Vector2 center = {GetScreenWidth() / 2, (GetScreenHeight() / 2) + 10};	//For whatever reason, I gotta add 10.
	const float scale = 1.0f;
	const Vector2 destinationSize = {targetDimensions.x * scale, targetDimensions.y * scale};
	const Vector2 subtitlePosition = {center.x, GetScreenHeight() - (destinationSize.y * 1.5)};	//Center of the subtitles
	const Rectangle subtitleDestination = rectangleFromSizeCenteredAroundPosition(destinationSize, subtitlePosition);
	DrawTexturePro(
		target.texture,
		(Rectangle){0, 0, targetDimensions.x, -targetDimensions.y},
		subtitleDestination,
		(Vector2){0, 0},
		0, //Rotation
		WHITE	//Color
	);
	
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
}