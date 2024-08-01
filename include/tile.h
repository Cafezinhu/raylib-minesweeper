#ifndef ENTITY_H
#include "entity.h"
#endif
#ifndef GAME_H
#include "game.h"
#endif
#define TILE_H
enum TileState { Closed, Flagged, Revealed };

struct Tile {
  enum TileState state;
  bool is_bomb;
  int number;
  struct Entity *entity;
};

struct Tile *tiles;

enum TileState TILE_CLOSED = Closed;
enum TileState TILE_FLAGGED = Flagged;
enum TileState TILE_REVEALED = Revealed;

void RevealTile(int x, int y);

Vector2 GlobalToTilePosition(float x, float y);
int TilePositionToIndex(float x, float y);
int GlobalPositionToTileIndex(float x, float y);
void UpdateTile(struct Entity *tile);
