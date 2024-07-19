#include <entity.h>
#include <stdlib.h>

void SpawnEntity(struct Entity *entity) {
  struct Entity *allocated_entity = AllocateEntity(entity);
  (*allocated_entity).next_entity = first_entity;
  first_entity = allocated_entity;
}
struct Entity *AllocateEntity(struct Entity *entity) {
  struct Entity *allocated_entity =
      (struct Entity *)malloc(sizeof(struct Entity));
  *allocated_entity = *entity;
  return allocated_entity;
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
