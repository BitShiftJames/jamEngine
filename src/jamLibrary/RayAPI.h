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

typedef void (*tUsersafeDeleteFile)(const char *path); // Has to be double null terminatad
typedef void *(*tLoadFunctionFromDLL)(void *dll_handle, const char *function_name);
typedef void *(*tLoadDLLFromPath)(const char *path);
typedef void (*tUnloadDLLFromPath)(void *dll_handle);

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
  
};

#endif 
