#ifndef RAY_API_H 
#define RAY_API_H

#include "jamTypes.h"

// NONE OF THESE STRUCTS ARE INHERIENTLY MINE.
// THEY ALL ORIGINATE FROM RAYLIB. THESE ARE JUST SO
// I CAN CROSS THE DLL BOUNDS AND STILL KEEP GLOBAL
// RAYLIB STATE. I COULD EVENTUALLY CONSTRUCT MY OWN BACKEND
// BUT I DO NOT FEEL LIKE DOING SO TODAY.

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

struct Camera3D_ {
    v3 position;
    v3 target;
    v3 up;
    f32 fovy;
    s32 projection;
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

struct Ray_ {
  v3 pos;
  v3 direction;
};

struct BoundingBox_ {
  v3 min;
  v3 max;
};

struct Font_ {
  s32 baseSize;
  s32 glyphCount;
  s32 glyphPadding;
  Texture2D_ texture;
  Rectangle_ *recs;
  GlyphInfo_ *glyphs;
};

struct Matrix_ {
    f32 m0, m4, m8, m12;
    f32 m1, m5, m9, m13;
    f32 m2, m6, m10, m14;
    f32 m3, m7, m11, m15;
};

struct Mesh_ {
    s32 vertexCount;
    s32 triangleCount;

    f32 *vertices;
    f32 *texcoords;
    f32 *texcoords2;
    f32 *normals;
    f32 *tangents;
    unsigned char *colors;
    unsigned short *indices;

    f32 *animVertices;
    f32 *animNormals;
    unsigned char *boneIds;
    f32 *boneWeights;
    Matrix_ *boneMatrices;
    s32 boneCount;

    unsigned int vaoId;
    unsigned int *vboId;
};

struct Shader_ {
    unsigned int id;
    s32 *locs;
};

struct MaterialMap_ {
    Texture2D_ texture;
    Color_ color;
    f32 value;
};

struct Material_ {
    Shader_ shader;
    MaterialMap_ *maps;
    f32 params[4];
};

struct BoneInfo_ {
    char name[32];
    s32 parent;
};

struct Transform_ {
    v3 translation;
    v4 rotation;
    v3 scale;
};

struct Model_ {
    Matrix_ transform;

    s32 meshCount;  
    s32 materialCount;
    Mesh_ *meshes;    
    Material_ *materials;
    s32 *meshMaterial; 

    s32 boneCount;    
    BoneInfo_ *bones; 
    Transform_ *bindPose;
};

struct FilePathList_ {
  u32 capacity;
  u32 count;
  char **paths;
};

enum Keyboard_keys {
    K_NULL            = 0,
    K_APOSTROPHE      = 39,
    K_COMMA           = 44,
    K_MINUS           = 45,
    K_PERIOD          = 46,
    K_SLASH           = 47,
    K_ZERO            = 48,
    K_ONE             = 49,
    K_TWO             = 50,
    K_THREE           = 51,
    K_FOUR            = 52,
    K_FIVE            = 53,
    K_SIX             = 54,
    K_SEVEN           = 55,
    K_EIGHT           = 56,
    K_NINE            = 57,
    K_SEMICOLON       = 59,
    K_EQUAL           = 61,
    K_A               = 65,
    K_B               = 66,
    K_C               = 67,
    K_D               = 68,
    K_E               = 69,
    K_F               = 70,
    K_G               = 71,
    K_H               = 72,
    K_I               = 73,
    K_J               = 74,
    K_K               = 75,
    K_L               = 76,
    K_M               = 77,
    K_N               = 78,
    K_O               = 79,
    K_P               = 80,
    K_Q               = 81,
    K_R               = 82,
    K_S               = 83,
    K_T               = 84,
    K_U               = 85,
    K_V               = 86,
    K_W               = 87,
    K_X               = 88,
    K_Y               = 89,
    K_Z               = 90,
    K_LEFT_BRACKET    = 91,
    K_BACKSLASH       = 92,
    K_RIGHT_BRACKET   = 93,
    K_GRAVE           = 96,
    K_SPACE           = 32,
    K_ESCAPE          = 256,
    K_ENTER           = 257,
    K_TAB             = 258,
    K_BACKSPACE       = 259,
    K_INSERT          = 260,
    K_DELETE          = 261,
    K_RIGHT           = 262,
    K_LEFT            = 263,
    K_DOWN            = 264,
    K_UP              = 265,
    K_PAGE_UP         = 266,
    K_PAGE_DOWN       = 267,
    K_HOME            = 268,
    K_END             = 269,
    K_CAPS_LOCK       = 280,
    K_SCROLL_LOCK     = 281,
    K_NUM_LOCK        = 282,
    K_PRINT_SCREEN    = 283,
    K_PAUSE           = 284,
    K_F1              = 290,
    K_F2              = 291,
    K_F3              = 292,
    K_F4              = 293,
    K_F5              = 294,
    K_F6              = 295,
    K_F7              = 296,
    K_F8              = 297,
    K_F9              = 298,
    K_F10             = 299,
    K_F11             = 300,
    K_F12             = 301,
    K_LEFT_SHIFT      = 340,
    K_LEFT_CONTROL    = 341,
    K_LEFT_ALT        = 342,
    K_LEFT_SUPER      = 343,
    K_RIGHT_SHIFT     = 344,
    K_RIGHT_CONTROL   = 345,
    K_RIGHT_ALT       = 346,
    K_RIGHT_SUPER     = 347,
    K_KB_MENU         = 348,
    K_KP_0            = 320,
    K_KP_1            = 321,
    K_KP_2            = 322,
    K_KP_3            = 323,
    K_KP_4            = 324,
    K_KP_5            = 325,
    K_KP_6            = 326,
    K_KP_7            = 327,
    K_KP_8            = 328,
    K_KP_9            = 329,
    K_KP_DECIMAL      = 330,
    K_KP_DIVIDE       = 331,
    K_KP_MULTIPLY     = 332,
    K_KP_SUBTRACT     = 333,
    K_KP_ADD          = 334,
    K_KP_ENTER        = 335,
    K_KP_EQUAL        = 336,
    K_BACK            = 4,
    K_MENU            = 5,
    K_VOLUME_UP       = 24,
    K_VOLUME_DOWN     = 25
};

enum Mouse_button_keys {
    M1 = 0,       // Mouse button left
    M2 = 1,       // Mouse button right
    M3 = 2,       // Mouse button middle (pressed wheel)
    M6 = 3,       // Mouse button side (advanced mouse device)
    M7 = 4,       // Mouse button extra (advanced mouse device)
    M4 = 5,       // Mouse button forward (advanced mouse device)
    M5 = 6,       // Mouse button back (advanced mouse device)
};

struct RayAPI;

typedef void (*tClearBackground)(Color_ color);
typedef void (*tBeginDrawing)(void);
typedef void (*tEndDrawing)(void);
typedef void (*tBeginMode2D)(Camera2D_ camera);
typedef void (*tEndMode2D)(void);

typedef void (*tBeginMode3D)(Camera3D_ camera);
typedef void (*tEndMode3D)(void);

typedef void (*tDrawLine)(v2 startpos, v2 endpos, f32 thick, Color_ color);
typedef void (*tDrawCircle)(v2 center, f32 radius, f32 startangle, f32 endangle, s32 segments, Color_ color);
typedef void (*tDrawEllipse)(v2 center, f32 radiusH, f32 radiusV, Color_ color);
typedef void (*tDrawRectangle)(v2 position, v2 size, Color_ color);
typedef void (*tDrawTriangle)(v2 p1, v2 p2, v2 p3, Color_ color);
typedef void (*tDrawPoly)(v2 center, s32 sides, f32 radius, f32 rotation, Color_ color);
typedef void (*tDrawText)(Font_ font, const char *text, v2 position, float fontSize, float spacing, Color_ tint);

typedef Font_ (*tGetFontDefault)(void);
typedef Font_ (*tLoadFont)(const char *fileName);
typedef void (*tUnloadFont)(Font_ font);
typedef s32 (*tMeasureText)(Font_ font, const char *text, f32 fontsize, f32 spacing);

typedef FilePathList_ (*tLoadDirectoryFiles)(const char *basePath, const char *filter, bool scanSubdirs);
typedef void (*tUnloadDirectoryFiles)(FilePathList_ files);
typedef bool (*tIsFileExtension)(const char *fileName, const char *ext);
typedef const char *(*tGetFileNameWithoutExt)(const char *filePath);
typedef const char *(*tGetDirectoryPath)(const char *filePath);
typedef const char *(*tGetWorkingDirectory)(void);
typedef bool (*tFileExist)(const char *fileName);
typedef int (*tFileRemove)(const char *fileName);
typedef int (*tFileCopy)(const char *srcPath, const char *dstPath);
typedef long (*tGetFileModTime)(const char *fileName);

typedef void (*tTextAppend)(char *text, const char *append, int *position);
typedef const char *(*tTextFormat)(const char *text, ...);

typedef void *(*tMemAlloc)(u32 size);
typedef void *(*tMemRealloc)(void *ptr, u32 size);
typedef void (*tMemFree)(void *ptr);

typedef void (*tInitWindow)(s32 width, s32 height, const char *title);
typedef void (*tCloseWindow)(void);
typedef bool (*tWindowShouldClose)(void);
typedef int (*tGetScreenWidth)(void);
typedef int (*tGetScreenHeight)(void);

typedef void (*tSetConfigFlags)(u32 flags);
typedef void (*tSetTraceLogLevel)(s32 logLevel);
typedef void (*tSetTargetFPS)(s32 fps);

typedef void (*tDrawLine3D)(v3 startPos, v3 endPos, Color_ color);
typedef void (*tDrawPoint3D)(v3 pos, Color_ color);
typedef void (*tDrawCircle3D)(v3 center, f32 radius, v3 rotationAxis, f32 rotationAngle, Color_ color);
typedef void (*tDrawTriangle3D)(v3 p1, v3 p2, v3 p3, Color_ color);
typedef void (*tDrawCube)(v3 pos, v3 size, Color_ color);
typedef void (*tDrawSphere)(v3 center, f32 radius, s32 rings, s32 slices, Color_ color);
typedef void (*tDrawCylinder)(v3 startPos, v3 endPos, f32 startRadius, f32 endRadius, s32 sides, Color_ color);
typedef void (*tDrawCapsule)(v3 startPos, v3 endPos, f32 radius, s32 slices, s32 rings, Color_ color);
typedef void (*tDrawPlane)(v3 pos, v3 size, Color_ color);
typedef void (*tDrawRay)(Ray_ ray, Color_ color);
typedef void (*tDrawGrid)(s32 slices, f32 spacing);
typedef void (*tDrawWireframeCube)(v3 position, v3 size, Color_ color);
typedef void (*tDrawWireframeSphere)(v3 pos, f32 radius, s32 rings, s32 slices, Color_ color);
typedef void (*tDrawWireframeCylinder)(v3 startPos, v3 endPos, f32 startRadius, f32 endRadius, s32 sides, Color_ color);
typedef void (*tDrawWireframeCapsule)(v3 startPos, v3 endPOs, f32 radius, s32 slices, s32 rings, Color_ color);

typedef Mesh_ (*tGenMeshPoly)(s32 sides, f32 radius);
typedef Mesh_ (*tGenMeshPlane)(f32 width, f32 length, s32 resX, s32 resZ);
typedef Mesh_ (*tGenMeshCube)(f32 width, f32 height, f32 length);
typedef Mesh_ (*tGenMeshSphere)(f32 radius, s32 rings, s32 slices);
typedef Mesh_ (*tGenMeshCylinder)(f32 radius, f32 height, s32 slices);
typedef Mesh_ (*tGenMeshCone)(f32 radius, f32 height, s32 slices);
typedef Mesh_ (*tGenMeshTorus)(f32 radius, f32 size, s32 radSeg, s32 sides);
typedef Mesh_ (*tGenMeshKnot)(f32 radius, f32 size, s32 radSeg, s32 sides);
typedef Mesh_ (*tGenMeshHeightMap)(Image_ heighmap, v3 size);
typedef Mesh_ (*tGenMeshCubicmap)(Image_ cubicmap, v3 cubeSize);

typedef Model_ (*tLoadModel)(const char *fileName);
typedef Model_ (*tLoadModelFromMesh)(Mesh_ mesh);
typedef bool (*tIsModelValid)(Model_ model);
typedef void (*tUnloadModel)(Model_ model);
typedef BoundingBox_ (*tGetModelBoundingBox)(Model_ model);

typedef void (*tDrawModel)(Model_ model, v3 pos, v3 rotationAxis, f32 rotationAngle, v3 scale, Color_ tint);
typedef void (*tDrawWireframe)(Model_ model, v3 pos, v3 rotationAxis, f32 rotationAngle, v3 scale, Color_ tint);
typedef void (*tDrawModelPoints)(Model_ model, v3 position, v3 rotationAxis, f32 rotationAngle, v3 scale, Color_ tint);
typedef void (*tDrawBillboard)(Camera3D_ camera, Texture2D_ texture, Rectangle_ source, v3 position, v3 up, v2 size, v2 origin, f32 rotation, Color_ tint);
typedef void (*tUploadMesh)(Mesh_ *mesh, bool dynamic);
typedef void (*tUpdateMeshBuffer)(Mesh_ mesh, s32 index, const void *data, s32 dataSize, s32 offset);
typedef void (*tUnloadMesh)(Mesh_ mesh);
typedef void (*tDrawMesh)(Mesh_ mesh, Material_ material, Matrix_ transform);
typedef void (*tDrawMeshInstanced)(Mesh_ mesh, Material_ material, const Matrix_ *transforms, s32 instances);
typedef BoundingBox_ (*tGetMeshBoundingBox)(Mesh_ mesh);
typedef void (*tGenMeshTangents)(Mesh_ *mesh);
typedef bool (*tExportMesh)(Mesh_ mesh, const char *fileName);

typedef Material_ (*tLoadMaterials)(const char *fileName, s32 *materialcount);
typedef Material_ (*tLoadMaterialsDefault)(void);
typedef bool (*tUnloadMaterial)(Material_ material);
typedef void (*tIsMaterialValid)(Material_ material);
typedef void (*tSetMaterialTexture)(Material_ *material, s32 mapType, Texture2D_ texture);
typedef void (*tSetModelMeshMaterial)(Model_ *model, s32 meshId, s32 materialId);

typedef bool (*tIsMouseButtonPressed)(int button);
typedef bool (*tIsMouseButtonDown)(int button);
typedef bool (*tIsMouseButtonReleased)(int button);
typedef bool (*tIsMouseButtonUp)(int button);
typedef v2 (*tGetMouseDelta)(void);
typedef v2 (*tGetMousePosition)(void);
typedef v2 (*tGetWheelMove)(void);

typedef bool (*tIsKeyPressed)(s32 key);
typedef bool (*tIsKeyPressedRepeat)(s32 key);
typedef bool (*tIsKeyDown)(s32 key);
typedef bool (*tIsKeyReleased)(s32 key);
typedef bool (*tIsKeyUp)(s32 key);
typedef int (*tGetKeyPressed)(void);
typedef int (*tGetCharPressed)(void);
typedef const char *(*tGetKeyName)(s32 key);
typedef void (*tSetExitKey)(s32 key);

typedef void (*tUpdateCamera)(Camera3D_ *camera, v3 movement, v3 rotation, f32 zoom);

typedef void (*tShowCursor)(void);
typedef void (*tHideCursor)(void);
typedef bool (*tIsCursorHidden)(void);
typedef void (*tEnableCursor)(void);
typedef void (*tDisableCursor)(void);
typedef bool (*tIsCursorOnScreen)(void);

typedef f32 (*tGetFrameTime)(void);

// platform specific functions.
typedef void (*tUsersafeDeleteFile)(const char *path); // Has to be double null terminatad
typedef void *(*tLoadFunctionFromDLL)(void *dll_handle, const char *function_name);
typedef void *(*tLoadDLLFromPath)(const char *path);
typedef void (*tUnloadDLLFromPath)(void *dll_handle);
typedef void (*tBuild_scenes)(const char *path);
typedef bool (*tDoesDirectoryExist)(const char *path);
typedef bool (*tCreateDirectory)(const char *path);
typedef FilePathList_ (*tSearchDynamicLibraries)(RayAPI *engineCTX, const char *path);
typedef bool (*tIsValidLibraryExtension)(const char *path, RayAPI *engineCTX);
typedef void (*tAppendLibraryExtension)(char *buf, int *text_length, RayAPI *engineCTX);

struct Style {
  Font_ font;
  f32 spacing;
  f32 size;
};

struct RayAPI {
  v2 ScreenSize;
  v2 HalfScreenSize;

  v2 MousePosition;

  Style defaultStyle;

  tClearBackground ClearBackground;

  tBeginDrawing BeginDrawing;
  tEndDrawing EndDrawing;

  tBeginMode2D BeginMode2D;
  tEndMode2D EndMode2D;

  tBeginMode3D BeginMode3D;
  tEndMode3D EndMode3D;

  tDrawLine DrawLine;
  tDrawCircle DrawCircle;
  tDrawEllipse DrawEllipse;
  tDrawRectangle DrawRectangle;
  tDrawTriangle DrawTriangle;
  tDrawPoly DrawPoly;
  tDrawText DrawText;

  tGetFontDefault GetFontDefault;
  tLoadFont LoadFont;
  tUnloadFont UnloadFont;
  tMeasureText MeasureText;

  // FIXME: Switch over to homegrown solution.
  tTextAppend TextAppend;
  tTextFormat TextFormat;

  tLoadDirectoryFiles LoadDirectoryFiles;
  tUnloadDirectoryFiles UnloadDirectoryFiles;
  tIsFileExtension IsFileExtension;
  tGetFileNameWithoutExt GetFileNameWithoutExt;
  tGetDirectoryPath GetDirectoryPath;
  tGetWorkingDirectory GetWorkingDirectory;

  tFileExist FileExists;
  tFileRemove FileRemove;
  tFileCopy FileCopy;
  tGetFileModTime GetFileModTime;

  tUsersafeDeleteFile UsersafeDelete;
  tLoadFunctionFromDLL LoadFunctionFromDLL;
  tLoadDLLFromPath LoadDLLFromPath;
  tUnloadDLLFromPath UnloadDLLFromPath;

  tInitWindow InitWindow;
  tCloseWindow CloseWindow;
  tWindowShouldClose WindowShouldClose;
  tGetScreenWidth GetScreenWidth;
  tGetScreenHeight GetScreenHeight;

  tMemAlloc MemAlloc; 
  tMemRealloc MemRealloc;
  tMemFree MemFree;

  tSetConfigFlags SetConfigFlags;
  tSetTargetFPS SetTargetFPS;
  tSetTraceLogLevel SetTraceLogLevel;

  tDrawLine3D DrawLine3D;
  tDrawPoint3D DrawPoint3D;
  tDrawCircle3D DrawCircle3D;
  tDrawTriangle3D DrawTriangle3D;
  tDrawCube DrawCube;
  tDrawSphere DrawSphere;
  tDrawCylinder DrawCylinder;
  tDrawCapsule DrawCapsule;
  tDrawPlane DrawPlane;
  tDrawRay DrawRay;
  tDrawGrid DrawGrid;
  tDrawWireframeCube DrawWireframeCube;
  tDrawWireframeSphere DrawWireframeSphere;
  tDrawWireframeCylinder DrawWireframeCylinder;
  tDrawWireframeCapsule DrawWireframeCapsule;

  tGenMeshPoly GenMeshPoly;
  tGenMeshPlane GenMeshPlane;
  tGenMeshCube GenMeshCube;
  tGenMeshSphere GenMeshSphere;
  tGenMeshCylinder GenMeshCylinder;
  tGenMeshCone GenMeshCone;
  tGenMeshTorus GenMeshTorus;
  tGenMeshKnot GenMeshKnot;
  tGenMeshHeightMap GenMeshHeightMap;
  tGenMeshCubicmap GenMeshCubicmap;

  tLoadModel LoadModel;
  tLoadModelFromMesh LoadModelFromMesh;
  tIsModelValid IsModelValid;
  tUnloadModel UnloadModel;
  tGetModelBoundingBox GetModelBoundingBox;

  tDrawModel DrawModel;
  tDrawWireframe DrawWireframe;
  tDrawModelPoints DrawModelPoints;
  tDrawBillboard DrawBillboard;

  tUploadMesh UploadMesh;
  tDrawMesh DrawMesh;
  tDrawMeshInstanced DrawMeshInstanced;
  tGetMeshBoundingBox GetMeshBoundingBox;
  tGenMeshTangents GenMeshTangents;
  tExportMesh ExportMesh;

  tLoadMaterials LoadMaterials;
  tLoadMaterialsDefault LoadMaterialsDefault;
  tUnloadMaterial UnloadMaterial;
  tIsMaterialValid IsMaterialValid;
  tSetMaterialTexture SetMaterialTexture;
  tSetModelMeshMaterial SetModelMeshMaterial;

  tIsMouseButtonPressed IsMouseButtonPressed;
  tIsMouseButtonDown IsMouseButtonDown;
  tIsMouseButtonReleased IsMouseButtonReleased;
  tIsMouseButtonUp IsMouseButtonUp;
  tGetMouseDelta GetMouseDelta;
  tGetMousePosition GetMousePosition;
  tGetWheelMove GetWheelMove;

  tIsKeyPressed IsKeyPressed;
  tIsKeyPressedRepeat IsKeyPressedRepeat;
  tIsKeyDown IsKeyDown;
  tIsKeyReleased IsKeyReleased;
  tIsKeyUp IsKeyUp;
  tGetKeyPressed GetKeyPressed;
  tGetCharPressed GetCharPressed;
  tGetKeyName GetKeyName;
  tSetExitKey SetExitKey;

  tUpdateCamera UpdateCamera;
  tBuild_scenes Build_scenes;
  tDoesDirectoryExist DoesDirectoryExist;
  tCreateDirectory CreateDirectory;
  tSearchDynamicLibraries SearchDynamicLibraries;
  tIsValidLibraryExtension IsValidLibraryExtension;
  tAppendLibraryExtension AppendLibraryExtension;

  tShowCursor ShowCursor;
  tHideCursor HideCursor;
  tIsCursorHidden IsCursorHidden;
  tEnableCursor EnableCursor;
  tDisableCursor DisableCursor;
  tIsCursorOnScreen IsCursorOnScreen;
  
  tGetFrameTime GetFrameTime;
};

#endif 
