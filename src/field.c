#include "field.h"
#ifndef GAME_H
#include "game.c"
#endif
#ifndef TILE_H
#include "tile.c"
#endif
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

  struct Entity field_outline = {};
  field_outline.Update = &UpdateFieldOutline;
  field_outline.x = -TILE_SIZE_CONSTANT / rows / 10.0f;
  field_outline.y = field_outline.x;

  SpawnEntity(&field_outline);
}

enum TilePosition { Corner, Edge, Middle };

void FillFieldWithBombs(int empty_x, int empty_y) {
  int *tiles_to_be_filled_with_bombs;
  int tiles_to_be_filled_with_bombs_len;

  int *tiles_without_bombs;
  int tiles_without_bombs_len;

  enum TilePosition tile_position;

  if (empty_x == 0 && empty_y == 0 || empty_x == 0 && empty_y == rows - 1 ||
      empty_x == columns - 1 && empty_y == 0 ||
      empty_x == columns - 1 && empty_y == rows - 1) {

    tiles_without_bombs_len = 4;
    tile_position = Corner;

  } else if (empty_x == 0 || empty_x == columns - 1 || empty_y == 0 ||
             empty_y == rows - 1) {

    tiles_without_bombs_len = 6;
    tile_position = Edge;
  } else {
    tiles_without_bombs_len = 9;
    tile_position = Middle;
  }

  tiles_to_be_filled_with_bombs_len = rows * columns - tiles_without_bombs_len;

  tiles_to_be_filled_with_bombs = (int *)malloc(
      sizeof(int) * rows * columns - sizeof(int) * tiles_without_bombs_len);
  tiles_without_bombs = (int *)malloc(sizeof(int) * tiles_without_bombs_len);

  int current_tile = 0;
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      int tilex = i + empty_x;
      int tiley = j + empty_y;
      if (tilex >= 0 && tilex < rows && tiley >= 0 && tiley < columns) {
        int index = TilePositionToIndex(tilex, tiley);
        tiles_without_bombs[current_tile] = index;
        current_tile += 1;
      }
    }
  }

  int tile_index = 0;
  for (int i = 0; i < rows * columns - 1; i++) {
    bool can_have_bomb = true;
    for (int j = 0; j < tiles_without_bombs_len; j++) {
      if (i == tiles_without_bombs[j]) {
        can_have_bomb = false;
      }
    }
    if (can_have_bomb) {
      tiles_to_be_filled_with_bombs[tile_index] = i;

      tile_index += 1;
    }
  }

  for (int i = 0; i < bombs_amt; i++) {
    int r = rand() % tiles_to_be_filled_with_bombs_len;

    int random_tile = tiles_to_be_filled_with_bombs[r];

    tiles[random_tile].is_bomb = true;

    for (int j = r; j < tiles_to_be_filled_with_bombs_len - 2; j++) {
      tiles_to_be_filled_with_bombs[j] = tiles_to_be_filled_with_bombs[j + 1];
    }

    tiles_to_be_filled_with_bombs_len -= 1;
  }
  free(tiles_to_be_filled_with_bombs);
  free(tiles_without_bombs);
}

void UpdateFieldOutline(struct Entity *outline) {
  Rectangle rec = {outline->x, outline->y, -outline->x * 2 + TILE_SIZE_CONSTANT,
                   -outline->y * 2 + TILE_SIZE_CONSTANT};
  DrawRectangleLinesEx(rec, -outline->x, GRAY);
}
