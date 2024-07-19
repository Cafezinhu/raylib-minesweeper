#include "tile.h"
#include "raylib.h"
#include <stdio.h>

Vector2 GlobalToTilePosition(float x, float y) {
  return (Vector2){x / (TILE_SIZE_CONSTANT / rows),
                   y / (TILE_SIZE_CONSTANT / rows)};
}

int TilePositionToIndex(float x, float y) { return (int)x * columns + (int)y; }

int GlobalPositionToTileIndex(float x, float y) {
  Vector2 tile_position = GlobalToTilePosition(x, y);
  return TilePositionToIndex(tile_position.x, tile_position.y);
}

void RevealTile(int x, int y) {

  int index = TilePositionToIndex(x, y);

  tiles[index].state = TILE_REVEALED;
  if (tiles[index].is_bomb) {
    lost = true;
    return;
  }
  tiles_revealed += 1;
  int bombs = 0;
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      int tilex = i + x;
      int tiley = j + y;
      if (tilex >= 0 && tilex < rows && tiley >= 0 && tiley < columns) {
        int index = TilePositionToIndex(tilex, tiley);
        if (tiles[index].is_bomb) {
          bombs += 1;
        }
      }
    }
  }
  tiles[index].number = bombs;

  if (bombs == 0) {
    for (int i = -1; i <= 1; i++) {

      for (int j = -1; j <= 1; j++) {

        int tilex = i + x;
        int tiley = j + y;

        int index = TilePositionToIndex(tilex, tiley);
        if (tilex >= 0 && tilex < columns && tiley >= 0 && tiley < rows &&
            !(tilex == x && tiley == y) && tiles[index].state == TILE_CLOSED) {
          RevealTile(tilex, tiley);
        }
      }
    }
  }
}

void UpdateTile(struct Entity *tile) {

  int tile_index = GlobalPositionToTileIndex(tile->x, tile->y);

  Color outline_color = GRAY;
  Vector2 mouse_position = GlobalMousePosition();

  float size = TILE_SIZE_CONSTANT / rows;

  if (mouse_position.x >= tile->x && mouse_position.x <= tile->x + size &&
      mouse_position.y >= tile->y && mouse_position.y <= tile->y + size &&
      tiles_revealed != rows * columns - bombs_amt && !lost) {
    outline_color = RAYWHITE;
    if (IsMouseButtonDown(0) || IsMouseButtonDown(1)) {
      outline_color = BLACK;
    }
    if (IsMouseButtonReleased(0) && tiles[tile_index].state == TILE_CLOSED) {
      Vector2 tile_position = GlobalToTilePosition(tile->x, tile->y);

      RevealTile((int)tile_position.x, (int)tile_position.y);
    } else if (IsMouseButtonReleased(1) &&
               tiles[tile_index].state == TILE_CLOSED) {
      tiles[tile_index].state = TILE_FLAGGED;
    } else if (IsMouseButtonReleased(1) &&
               tiles[tile_index].state == TILE_FLAGGED) {
      tiles[tile_index].state = TILE_CLOSED;
    }
  }

  Rectangle rec = {tile->x, tile->y, size, size};
  DrawRectangleRec(rec, LIGHTGRAY);
  if (tiles[tile_index].state != TILE_REVEALED) {

    DrawRectangleLinesEx(rec, size / 10.0, outline_color);
  }

  if (tiles[tile_index].state == TILE_REVEALED) {
    char string[sizeof(int)] = "";
    Color text_color = BLACK;
    if (tiles[tile_index].is_bomb) {
      Rectangle rec = {tile->x + size / 1.4f, tile->y + size / 5, size / 20,
                       size / 6};

      DrawRectanglePro(rec, (Vector2){0, 0}, 45, ORANGE);
      DrawCircle(tile->x + size / 2, tile->y + size / 2, size / 4, BLACK);
      sprintf(string, "B");
    } else if (tiles[tile_index].number != 0) {
      int number = tiles[tile_index].number;
      Color colors[] = {BLACK, BLUE,    DARKGREEN, RED, DARKBLUE,
                        BROWN, SKYBLUE, PURPLE,    GRAY};
      text_color = colors[number];
      sprintf(string, "%d", number);
      DrawText(string, tile->x + size / 2.7, tile->y + size / 4, size / 2,
               text_color);
    }

  } else if (tiles[tile_index].state == TILE_FLAGGED) {
    DrawRectangle(tile->x + size / 2.605, tile->y + size / 3, 4, size / 2,
                  BLACK);
    DrawPoly((Vector2){tile->x + size / 2, tile->y + size / 3}, 3, size / 4, 0,
             RED);
    // DrawText("F", tile->x + size / 2.7, tile->y + size / 4, size / 2, RED);
  }
}
