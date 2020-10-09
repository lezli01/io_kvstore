#include "map.h"

#include <string.h>

static const size_t MAP_NODE_SIZE = sizeof(MapNode);
static const size_t MAP_SIZE      = sizeof(Map);

int
mapSortCompare(const void * nodeA, const void * nodeB)
{
    const MapNode * castedNodeA = nodeA;
    const MapNode * castedNodeB = nodeB;

    return strcmp(castedNodeA->key, castedNodeB->key);
}

int
mapSearchCompare(const void * key, const void * node)
{
    const MapNode * castedNode = node;
    const char * castedKey     = key;

    return strcmp(castedKey, castedNode->key);
}

MapNode *
mapGetNode(Map * map, const char * key)
{
    if (!map || !map->nodes || !key) {
        return NULL;
    }

    return bsearch(
        key,
        map->nodes,
        map->length,
        MAP_NODE_SIZE,
        mapSearchCompare
    );
}

void
mapSortNodes(Map * map)
{
    if (!map || !map->nodes || map->length <= 1) {
        return;
    }

    qsort(
        map->nodes,
        map->length,
        MAP_NODE_SIZE,
        mapSortCompare
    );
}

void
mapResizeNodes(Map * map)
{
    if (!map) {
        return;
    }

    map->nodes = realloc(map->nodes, MAP_NODE_SIZE * map->length);
}

void
mapCreateNode(Map * map, const char * key, char * value)
{
    if (!map || !key || !value) {
        return;
    }

    map->length++;
    mapResizeNodes(map);

    MapNode * pair = &map->nodes[map->length - 1];

    pair->key = malloc(sizeof(char) * strlen(key) + 1);
    strcpy(pair->key, key);
    pair->value = value;

    mapSortNodes(map);
}

Map *
mapCreate()
{
    Map * map = malloc(MAP_SIZE);

    map->nodes  = NULL;
    map->length = 0;

    return map;
}

void
mapDestroy(Map * map)
{
    if (!map) {
        return;
    }

    for (int i = 0; i < map->length; ++i) {
        free(map->nodes[i].key);
        free(map->nodes[i].value);
    }

    if (map->nodes) {
        free(map->nodes);
    }

    free(map);
}

void
mapPut(Map * map, const char * key, char * value)
{
    MapNode * node = mapGetNode(map, key);

    if (!value) {
        return;
    }

    char * copy = malloc(sizeof(char) * strlen(value) + 1);
    strcpy(copy, value);

    if (node) {
        free(node->value);
        node->value = copy;
    } else {
        mapCreateNode(map, key, copy);
    }
}

char *
mapGet(Map * map, const char * key)
{
    MapNode * node = mapGetNode(map, key);

    return node ? node->value : NULL;
}
