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
static Scene *global_aux_scene = 0;

Scene *GetCurrScene() {
  if (global_curr_scene) {
    return global_curr_scene;
  }
  return 0;
}

Scene *GetAuxScene() {
  if (global_aux_scene) {
    return global_aux_scene;
  }
  return 0;
}

// Needs an init call because it's the engines job to keep the Scene memory alive.
void InitScene(Scene *currScene, Scene *auxScene) {
  global_curr_scene = currScene;
  global_aux_scene = auxScene;
};


// Some solution to store the previous scene maybe a global.
void SetScene(Scene *globalScene, Scene *setScene) {
  
  if (globalScene->onExit) {
    globalScene->onExit(globalScene);
  }

  globalScene->update = setScene->update;
  globalScene->render = setScene->render;
  globalScene->onEnter = setScene->onEnter;
  globalScene->onExit = setScene->onExit;

  if (globalScene->onEnter) {
    globalScene->onEnter(globalScene);
  }

}

#endif // !JAM_SCENE_H
