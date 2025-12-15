#ifndef JAM_SCENE_H
#define JAM_SCENE_H

#include "jamTypes.h"
#include "jamText.h"

#include "RayAPI.h"
#include "raylib.h"

#define MAX_FILE_PATH 1024

typedef void (*sceneUpdate)(struct Scene *self);
typedef void (*sceneRender)(struct Scene *self, RayAPI *engineCTX);
typedef void (*sceneOnEnter)(struct Scene *self, RayAPI *engineCTX);
typedef void (*sceneOnExit)(struct Scene *self);

struct Scene {
  void (*update)(struct Scene *self);
  void (*render)(struct Scene *self, RayAPI *engineCTX);
  void (*onEnter)(struct Scene *self, RayAPI *engineCTX);
  void (*onExit)(struct Scene *self);

  void *data;

  memoryArena *arena;
};

struct ActiveScene {
  char *scene_name;
  Scene *scene;
  memoryArena *arena;
  ActiveScene *next;
};

struct SceneList {
  u32 scene_count;
  char **scene_name;
  char **scene_path;
  u64 *scene_mod_time;
  Scene *scenes;
  void **dll_handles;
  // Stored in seperate memory.
  ActiveScene *list;
};

Scene *GetScene(SceneList *sceneList, char *name);
void AddScene(SceneList *sceneList, char *name, memoryArena *active_scene_memory, 
              memoryArena *sceneMemory, u64 size, RayAPI *engineCTX);
Scene load_a_scene(char *path, void *dll_handle);
void Unload_scenes(SceneList *sceneList);
SceneList Construct_scene_table(memoryArena *arena, u32 max_scenes, char *scene_path, FilePathList *List);

#endif // !JAM_SCENE_H
