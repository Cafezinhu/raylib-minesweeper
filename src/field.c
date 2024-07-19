#include "field.h"
#include "game.c"
#include "tile.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void CreateField() {
  tiles = (struct Tile *)malloc(sizeof(struct Tile) * rows * columns);
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

  int *tiles_without_bombs = (int *)malloc(sizeof(int) * rows * columns);
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
}

void UpdateFieldOutline(struct Entity *outline) {
  Rectangle rec = {outline->x, outline->y, -outline->x * 2 + TILE_SIZE_CONSTANT,
                   -outline->y * 2 + TILE_SIZE_CONSTANT};
  DrawRectangleLinesEx(rec, -outline->x, GRAY);
}
