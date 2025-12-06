#ifndef JAM_SCENE_H
#define JAM_SCENE_H

#include "jamTypes.h"

#include <cstring>

#define MAX_FILE_PATH 1024

void ClearAnArena(memoryArena *arena) {
  memset(arena->memory, 0, arena->Used);
  arena->Used = 0;
};


struct Scene;

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

  v2 MousePos;
  v2 ScreenSize;

  char save_directory[1024];
  char parent_directory[1024];

  memoryArena *arena;

  memoryArena *temp_arena;

};


static Scene *global_curr_scene = 0;

Scene *GetCurrScene() {
  if (global_curr_scene) {
    return global_curr_scene;
  }
  return 0;
}

// Needs an init call because it's the engines job to keep the Scene memory alive.
void InitScene(Scene *currScene) {
  global_curr_scene = currScene;
  if (global_curr_scene->onEnter) {
    global_curr_scene->onEnter(global_curr_scene);
  }
};


// Some solution to store the previous scene maybe a global.
void SetScene(Scene *setScene) {
  
  if (global_curr_scene->onExit) {
    global_curr_scene->onExit(global_curr_scene);
  }

  global_curr_scene->update = setScene->update;
  global_curr_scene->render = setScene->render;
  global_curr_scene->onEnter = setScene->onEnter;
  global_curr_scene->onExit = setScene->onExit;

  if (global_curr_scene->onEnter) {
    global_curr_scene->onEnter(global_curr_scene);
  }

}

#endif // !JAM_SCENE_H
