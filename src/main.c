#include "raylib.h"

#include "entity.c"
#include "field.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef PLATFORM_WEB
#include <time.h>
#endif

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
// #define rand() ((int)(emscripten_random() * 100))
#endif

const int screenWidth = 1000;
const int screenHeight = 800;

void UpdateDrawFrame();

int main() {

#ifndef PLATFORM_WEB
  srand(time(NULL));
#else
  srand(emscripten_get_now());
#endif
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "Minesweeper");

#ifndef PLATFORM_ANDROID
  // ChangeDirectory("assets");
#endif

  SetExitKey(0);

  SetTargetFPS(60);

  CreateField();
  camera = (Camera2D){0};
  camera.target = (Vector2){TILE_SIZE_CONSTANT / 2, TILE_SIZE_CONSTANT / 2};
  camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
  camera.zoom = 1.0f;

//  #ifndef PLATFORM_ANDROID
//  ChangeDirectory("..");
//  #endif
#ifdef PLATFORM_WEB
  emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#endif
  while (!WindowShouldClose()) {
    UpdateDrawFrame();
  }

  CloseWindow();
  return 0;
}

void RestartGame();
void RestartGame() {
  free(tiles);
  lost = false;
  tiles_revealed = 0;
  struct Entity *entity = first_entity;
  while (entity != NULL) {
    struct Entity *dead_entity = entity;
    entity = entity->next_entity;
    free(dead_entity);
  }

  CreateField();
}

void UpdateDrawFrame() {
  // Draw
  //----------------------------------------------------------------------------------
  BeginDrawing();
  ClearBackground(LIGHTGRAY);
  DrawFPS(10, 10);
  BeginMode2D(camera);
  if (IsKeyPressed(KEY_R)) {

    RestartGame();
  }

  UpdateEntities();
  char bombs_left_str[30];
  int bombs_left = remaining_bombs;
  if (bombs_left < 0)
    bombs_left = 0;
  sprintf(bombs_left_str, "Bombs left: %d", bombs_left);

  EndMode2D();
  DrawText(bombs_left_str, 400, 20, 30, BLACK);
  if (tiles_revealed == rows * columns - bombs_amt && !lost) {
    DrawText("YOU WON! :)", 400, 730, 30, DARKGREEN);
    DrawText("Press R to restart.", 350, 760, 30, BLACK);
  } else if (lost) {
    DrawText("YOU LOST! :(", 400, 730, 30, RED);
    DrawText("Press R to restart.", 350, 760, 30, BLACK);
  }

  EndDrawing();
}
