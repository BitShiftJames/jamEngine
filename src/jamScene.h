#ifndef JAM_SCENE_H
#define JAM_SCENE_H

#include "jamTypes.h"

#include <cstring>

void ClearAnArena(memoryArena *arena) {
  memset(arena->memory, 0, arena->Used);
  arena->Used = 0;
};

struct Scene;
struct Scene {
  void (*update)(struct Scene *self);
  void (*render)(struct Scene *self); // Generalize cursor init and just have it created on scene start up.
  void (*onEnter)(struct Scene *self);
  void (*onExit)(struct Scene *self);

  void *data;

  v2 MousePos;
  memoryArena *arena;

  memoryArena *temp_arena;

};

static Scene *global_curr_scene = 0;

Scene *GetScene() {
  if (global_curr_scene) {
    return global_curr_scene;
  }
  return 0;
}

// Needs an init call because it's the engines job to keep the Scene memory alive.
void InitScene(Scene *currScene) {
  global_curr_scene = currScene;
};

// Some solution to store the previous scene maybe a global.
void SetScene(Scene *scene) {
  Scene *currScene = GetScene();

  if (currScene && currScene->onExit) {
    currScene->onExit(currScene);
  }

  currScene->update = scene->update;
  currScene->render = scene->render;
  currScene->onEnter = scene->onEnter;
  currScene->onExit = scene->onExit;

  if (currScene && currScene->onEnter) {
    currScene->onEnter(currScene);
  }
}

#endif // !JAM_SCENE_H
