#include "game.h"

Vector2 GlobalMousePosition() {
  return (Vector2){(float)GetMouseX() - camera.offset.x + camera.target.x,
                   (float)GetMouseY() - camera.offset.y + camera.target.y};
}
