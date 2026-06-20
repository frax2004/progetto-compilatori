#pragma once


typedef struct Entry {
  char* city_code;
  char* city_name;
  double x;
  double y;
  struct Entry* next;
} Entry;

typedef struct {
  Entry** entries;
  unsigned long long size;
  unsigned long long capacity;
} HashTable;

void printHashTable(HashTable table);
Entry* hashTableAt(HashTable* self, const char* key);
int hashTableContains(HashTable* self, const char* key);
void destroyHashTable(HashTable* table);