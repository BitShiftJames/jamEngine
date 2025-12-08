#ifndef JAM_SCENE_H
#define JAM_SCENE_H

#include "jamTypes.h"
#include "jamText.h"

#define MAX_FILE_PATH 1024

typedef void (*sceneUpdate)(struct Scene *self);
typedef void (*sceneRender)(struct Scene *self);
typedef void (*sceneOnEnter)(struct Scene *self);
typedef void (*sceneOnExit)(struct Scene *self);

struct Scene {
  void (*update)(struct Scene *self);
  void (*render)(struct Scene *self);
  void (*onEnter)(struct Scene *self);
  void (*onExit)(struct Scene *self);

  void *data;

  memoryArena *arena;
  memoryArena *temp_arena;
};

struct ActiveScene {
  Scene *scene;
  ActiveScene *next;
};


struct SceneList {
  u32 scene_count;
  char **scene_name;
  Scene *scenes;
  ActiveScene *start;
};

Scene *GetScene(SceneList *sceneList, char *name);
void AddScene(SceneList *sceneList, char *name, memoryArena *arena);
Scene load_a_scene(char *path);
SceneList Construct_scene_table(memoryArena *arena, u32 max_scenes, char *scene_path);

#endif // !JAM_SCENE_H
