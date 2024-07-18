#include "raylib.h"
#include "raymath.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

const int screenWidth = 800;
const int screenHeight = 600;

struct Entity {
  struct Entity *next_entity;
  float x;
  float y;
  float scaleX;
  float scaleY;
  Texture texture;
  bool dead;

  void (*Update)(struct Entity *entity);
};

struct Entity *first_entity;

void UpdateDrawFrame();
void UpdateEntities();

//------------------ASSETS--------------

int main() {

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "Raylib basic window");

#ifndef PLATFORM_ANDROID
  ChangeDirectory("assets");
#endif

  SetExitKey(0);

  SetTargetFPS(60);

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else

  // first_entity = malloc(sizeof(struct Entity));
  //  #ifndef PLATFORM_ANDROID
  //  ChangeDirectory("..");
  //  #endif
  while (!WindowShouldClose()) {
    UpdateDrawFrame();
  }
#endif

  CloseWindow();
  return 0;
}

void UpdateDrawFrame() {
  // Draw
  //----------------------------------------------------------------------------------
  BeginDrawing();
  ClearBackground(RAYWHITE);
  DrawFPS(10, 10);
  UpdateEntities();
  EndDrawing();
}

void UpdateEntities() {
  struct Entity *entity = first_entity;
  struct Entity *previous_entity = NULL;

  while (entity != NULL) {
    if (entity->dead) {
      printf("Killing entity\n");
      struct Entity *next_entity = entity->next_entity;
      struct Entity *dead_entity = entity;
      if (first_entity != NULL && previous_entity != NULL) {
        (*previous_entity).next_entity = entity->next_entity;
        entity = entity->next_entity;
      } else {
        first_entity = next_entity;
      }
      free(dead_entity);
    } else {
      entity->Update(entity);
      previous_entity = entity;
      entity = entity->next_entity;
    }
  }
}
