#include <raylib.h>
#define ENTITY_H
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

void SpawnEntity(struct Entity *entity);

struct Entity *AllocateEntity(struct Entity *entity);
void UpdateEntities();
