#include "hash_table.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define MAX_LOAD_FACTOR .75f




typedef size_t hash_t;

static hash_t hash(const char* s) {
  hash_t value = 0;
  int c;
  
  while((c = *s++))
  value = c + (value << 6) + (value << 16) - value;
  
  return value;
}

static double loadFactor(HashTable* self) {
  return self->capacity == 0 ? 1.0 : self->size/(double)self->capacity;
}

void printHashTable(HashTable table) {
  printf("=========== HashTable BEGIN ===========\n");

 for(int i = 0; i < table.capacity; i++) {
    Entry* list = table.entries[i];
  
    if(list != NULL) printf("[%d]: ", i);

    while(list != NULL) {
      printf("[code: %s, name: %s, (x: %.5lf, y: %.5lf)]%s", list->city_code, list->city_name, list->x, list->y, list->next != NULL ? " -> " : "");
      list = list->next;
    }
    putchar('\n');
  }

  printf("=========== HashTable END ===========\n");
}

static void hashTableRehash(HashTable* self) {
  HashTable rehashed = { 0 };
  rehashed.capacity = self->capacity > 0 ? self->capacity * 2 : 16;
  rehashed.entries = calloc(rehashed.capacity, sizeof(Entry*));

  for(int i = 0; i < self->capacity; i++) {
    Entry* list = self->entries[i];

    while(list != NULL) {
      Entry* newEntry = hashTableAt(&rehashed, list->city_code);
      newEntry->city_name = list->city_name;
      newEntry->x = list->x;
      newEntry->y = list->y;

      list = list->next;
    }
  }

  destroyHashTable(self);
  memcpy(self, &rehashed, sizeof(HashTable));
}

Entry* hashTableAt(HashTable* self, const char* key) {
  if(loadFactor(self) > MAX_LOAD_FACTOR) {
    hashTableRehash(self);
  }

  hash_t hash_code = hash(key) % self->capacity;
  Entry* list = self->entries[hash_code];

  while(list != NULL && strcmp(list->city_code, key) != 0) {    
    list = list->next;
  }

  if(list != NULL) {
    return list;
  } else {
    Entry* head = (Entry*)calloc(1, sizeof(Entry));
    head->next = self->entries[hash_code];
    self->entries[hash_code] = head;
    head->city_code = key;
    self->size++;

    return head;
  }
}

int hashTableContains(HashTable* self, const char* key) {
  if(self->entries == NULL || self->capacity == 0 || self->size == 0) return 0;

  hash_t hash_code = hash(key) % self->capacity;
  Entry* list = self->entries[hash_code];

  while(list != NULL && strcmp(list->city_code, key) != 0) {
    list = list->next;
  }

  return list != NULL;
}

void destroyHashTable(HashTable* table) {
  for(int i = 0; i < table->capacity; i++) {
    Entry* list = table->entries[i];
    while(list != NULL) {
      Entry* temp = list->next;
      free(list);
      list = temp;
    }
  }

  free(table->entries);
  memset(table, 0, sizeof(HashTable));
}