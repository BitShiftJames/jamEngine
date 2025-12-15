#ifndef RAY_API_H 
#define RAY_API_H

#include "jamTypes.h"

// NONE OF THESE STRUCTS ARE INHERIENTLY MINE.
// THEY ALL ORIGINATE FROM RAYLIB. THESE ARE JUST SO
// I CAN CROSS THE DLL BOUNDS AND STILL KEEP GLOBAL
// RAYLIB STATE. I COULD EVENTUALLY CONSTRUCT MY OWN BACKEND
// BUT I DO NOT FEEL LIKE DOING SO TODAY.
// 
// Also side note this is not very flexible to change
// so as I make this eventually things will turn into a bunch
// of void pointers as is the nature of an ABI boundary I know
// sue me.

struct Color_ {
  u8 r;
  u8 g;
  u8 b;
  u8 a;
};

struct Camera2D_ {
  v2 offset;
  v2 target;
  f32 rotation;
  f32 zoom;
};

struct Image_ {
  void *data;
  s32 width;
  s32 height;
  s32 mipmaps;
  s32 format;
};

struct Rectangle_ {
  union {
    struct {
      f32 x;
      f32 y;
      f32 width;
      f32 height;
    };
    struct {
      v2 min;
      v2 max;
    };
  };
};

struct GlyphInfo_ {
  s32 value;
  s32 offsetX;
  s32 offsetY;
  s32 advanceX;
  Image_ image;
};

struct Texture2D_ {
  u32 id;
  s32 width;
  s32 height;
  s32 mipmaps;
  s32 format;
};

struct Font_ {
  s32 baseSize;
  s32 glyphCount;
  s32 glyphPadding;
  Texture2D_ texture;
  Rectangle_ *recs;
  GlyphInfo_ *glyphs;
};

struct RayAPI;

typedef void (*tClearBackground)(Color_ color);
typedef void (*tBeginDrawing)(void);
typedef void (*tEndDrawing)(void);
typedef void (*tBeginMode2D)(Camera2D_ camera);
typedef void (*tEndMode2D)(void);
typedef void (*tDrawLine)(v2 startpos, v2 endpos, f32 thick, Color_ color);
typedef void (*tDrawCircle)(v2 center, f32 radius, f32 startangle, f32 endangle, s32 segments, Color_ color);
typedef void (*tDrawEllipse)(v2 center, f32 radiusH, f32 radiusV, Color_ color);
typedef void (*tDrawRectangle)(v2 position, v2 size, Color_ color);
typedef void (*tDrawTriangle)(v2 p1, v2 p2, v2 p3, Color_ color);
typedef void (*tDrawPoly)(v2 center, s32 sides, f32 radius, f32 rotation, Color_ color);
typedef void (*tDrawText)(Font_ font, const char *text, v2 position, float fontSize, float spacing, Color_ tint);

typedef Font_ (*tGetFontDefault)(void);
typedef Font_ (*tLoadFont)(const char *fileName);
typedef s32 (*tMeasureText)(Font_ font, const char *text, f32 fontsize, f32 spacing);

struct Style {
  Font_ font;
  f32 spacing;
  f32 size;
};

struct RayAPI {
  v2 ScreenSize;
  v2 MousePosition;

  Style defaultStyle;

  tClearBackground ClearBackground;

  tBeginDrawing BeginDrawing;
  tEndDrawing EndDrawing;

  tBeginMode2D BeginMode2D;
  tEndMode2D EndMode2D;

  tDrawLine DrawLine;
  tDrawCircle DrawCircle;
  tDrawEllipse DrawEllipse;
  tDrawRectangle DrawRectangle;
  tDrawTriangle DrawTriangle;
  tDrawPoly DrawPoly;
  tDrawText DrawText;

  tGetFontDefault GetFontDefault;
  tLoadFont LoadFont;
  tMeasureText MeasureText;

};

#endif 
