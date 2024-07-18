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

const int screenWidth = 1000;
const int screenHeight = 800;

int rows = 10;
int columns = 10;
const float TILE_SIZE_CONSTANT = 600.0;
int bombs_amt = 10;

Camera2D camera;

Vector2 GlobalMousePosition();
Vector2 GlobalMousePosition() {
  return (Vector2){(float)GetMouseX() - camera.offset.x + camera.target.x,
                   (float)GetMouseY() - camera.offset.y + camera.target.y};
}

int PositionToTileIndex(float x, float y);
int PositionToTileIndex(float x, float y) {
  return (int)x / (int)(TILE_SIZE_CONSTANT / rows) * columns +
         (int)y / (int)(TILE_SIZE_CONSTANT / rows);
}

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

enum TileState { Closed, Flagged, Revealed };

struct Tile {
  enum TileState state;
  bool is_bomb;
  int number;
  struct Entity *entity;
};

struct Tile *tiles;

struct Entity *first_entity;

void SpawnEntity(struct Entity *entity);

struct Entity *AllocateEntity(struct Entity *entity);

void SpawnEntity(struct Entity *entity) {
  struct Entity *allocated_entity = AllocateEntity(entity);
  (*allocated_entity).next_entity = first_entity;
  first_entity = allocated_entity;
}
struct Entity *AllocateEntity(struct Entity *entity) {
  struct Entity *allocated_entity = malloc(sizeof(struct Entity));
  *allocated_entity = *entity;
  return allocated_entity;
}

void UpdateFieldOutline(struct Entity *outline);
void UpdateTile(struct Entity *tile);
void UpdateTile(struct Entity *tile) {
  int tile_index = PositionToTileIndex(tile->x, tile->y);
  Color outline_color = GRAY;
  Vector2 mouse_position = GlobalMousePosition();
  float size = TILE_SIZE_CONSTANT / rows;
  if (mouse_position.x >= tile->x && mouse_position.x <= tile->x + size &&
      mouse_position.y >= tile->y && mouse_position.y <= tile->y + size) {
    outline_color = RAYWHITE;
    if (IsMouseButtonDown(0) || IsMouseButtonDown(1)) {
      outline_color = BLACK;
    }
  }
  Rectangle rec = {tile->x, tile->y, size, size};
  DrawRectangleRec(rec, LIGHTGRAY);
  DrawRectangleLinesEx(rec, size / 10.0, outline_color);
}

struct Entity *CreateField();
struct Entity *CreateField() {
  tiles = malloc(sizeof(struct Tile) * rows * columns);
  struct Entity *last_tile = NULL;
  for (int x = 0; x < rows; x++) {
    for (int y = 0; y < columns; y++) {
      struct Entity *allocated_tile = NULL;
      struct Entity tile = {};
      tile.x = x * TILE_SIZE_CONSTANT / rows;
      tile.y = y * TILE_SIZE_CONSTANT / rows;
      tile.Update = &UpdateTile;
      if (last_tile == NULL) {

        first_entity = AllocateEntity(&tile);
        last_tile = first_entity;
        allocated_tile = last_tile;
      } else {

        allocated_tile = AllocateEntity(&tile);
        (*last_tile).next_entity = allocated_tile;
      }
      last_tile = allocated_tile;

      struct Tile tile_data = {};
      tile_data.entity = last_tile;
      tiles[x * columns + y] = tile_data;
    }
  }

  int *tiles_without_bombs = malloc(sizeof(int) * rows * columns);
  for (int i = 0; i < rows * columns; i++) {
    tiles_without_bombs[i] = i;
  }
  int tiles_without_bombs_len = rows * columns;
  for (int i = 0; i < bombs_amt; i++) {
    int r = rand() % tiles_without_bombs_len;
    int random_tile = tiles_without_bombs[r];
    tiles[random_tile].is_bomb = true;

    for (int j = r; j < tiles_without_bombs_len - 1; j++) {
      tiles_without_bombs[j] = tiles_without_bombs[j + 1];
    }

    tiles_without_bombs_len -= 1;
  }
  free(tiles_without_bombs);

  struct Entity field_outline = {};
  field_outline.Update = &UpdateFieldOutline;
  field_outline.x = -TILE_SIZE_CONSTANT / rows / 10.0f;
  field_outline.y = field_outline.x;

  SpawnEntity(&field_outline);

  return last_tile;
}

void UpdateFieldOutline(struct Entity *outline) {
  Rectangle rec = {outline->x, outline->y, -outline->x * 2 + TILE_SIZE_CONSTANT,
                   -outline->y * 2 + TILE_SIZE_CONSTANT};
  DrawRectangleLinesEx(rec, -outline->x, GRAY);
}

void UpdateDrawFrame();
void UpdateEntities();

int main() {
  srand(time(NULL));
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
  CreateField();
  camera = (Camera2D){0};
  camera.target = (Vector2){TILE_SIZE_CONSTANT / 2, TILE_SIZE_CONSTANT / 2};
  camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
  camera.zoom = 1.0f;

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
  ClearBackground(LIGHTGRAY);
  DrawFPS(10, 10);

  BeginMode2D(camera);

  UpdateEntities();

  EndMode2D();

  EndDrawing();
}

void UpdateEntities() {
  struct Entity *entity = first_entity;
  struct Entity *previous_entity = NULL;

  while (entity != NULL) {
    if (entity->dead) {
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
