#ifndef KV_STORE_MAP_H
#define KV_STORE_MAP_H

#include <stdlib.h>

typedef struct {
    char * key;
    char * value;
} MapNode;

typedef struct {
    MapNode * nodes;
    size_t    length;
} Map;

Map *
mapCreate(void);
void
mapDestroy(Map * map);
void
mapPut(Map * map, const char * key, char * value);
char *
mapGet(Map * map, const char * key);

#endif // KV_STORE_MAP_H
