/**
 * @file map_path.c
 * @brief Implémentation des fonctions nécessaires à la structure des chemins des cartes
 */

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "map_path.h"
#include "../elements/map.h"
#include "../utils/macros.h"
#include "../utils/other_utils.h"

node_map_path_t *create_node_map_path(map_t *current) {
    if (!current) return NULL;

    node_map_path_t *n = (node_map_path_t *)malloc_check(sizeof(node_map_path_t));
    if (!n) return NULL;

    n->map = current;
    n->next = NULL;
    return n;
}

void free_node_map_path(node_map_path_t **node) {
    if (!node || !*node) return;
    free(*node);
    *node = NULL;
}

map_path_t *create_map_path(map_t *spawn) {
    if (!spawn) return NULL;

    spawn->pos_x = 0;
    spawn->pos_y = 0;

    node_map_path_t *spawn_node = create_node_map_path(spawn);
    if (!spawn_node) return NULL;

    map_path_t *mp = (map_path_t *)malloc_check(sizeof(map_path_t));
    if (!mp) {
        free_node_map_path(&spawn_node);
        return NULL;
    }

    mp->head = spawn_node;
    mp->size = 1;
    return mp;
}

void insert_map_path(map_path_t *map_path, map_t *map) {
    if (!map_path || !map) return;

    node_map_path_t *node = create_node_map_path(map);
    if (!node) return;

    node_map_path_t *current = map_path->head;
    while (current->next) {
        current = current->next;
    }

    current->next = node;
    map_path->size++;
}

map_t *get_map_by_position_in_map_path(map_path_t *map_path, int pos_x, int pos_y) {
    if (!map_path) return NULL;

    node_map_path_t *current = map_path->head;
    while (current) {
        if(current->map->pos_x == pos_x && current->map->pos_y == pos_y) {
            return current->map;
        }
        current = current->next;
    }
    return NULL;
}

int contains_map_in_map_path(map_path_t *map_path, map_t *map) {
    if (!map_path || !map || !map->name) return 0;

    node_map_path_t *current = map_path->head;
    while (current) {
        if(current->map && current->map->name &&
           strcmp(current->map->name, map->name) == 0) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void connect_map_neighbour(map_path_t *map_path, map_t *map) {
    if (!map_path || !map) return;
    map_t *neighbour_map;

    // TOP connection
    if(!map->top) {
        neighbour_map = get_map_by_position_in_map_path(map_path, map->pos_x, map->pos_y + 1);
        if (neighbour_map) {
            map->top = neighbour_map;
            neighbour_map->bottom = map;
        }
    }

    // BOTTOM connection
    if(!map->bottom) {
        neighbour_map = get_map_by_position_in_map_path(map_path, map->pos_x, map->pos_y - 1);
        if (neighbour_map) {
            map->bottom = neighbour_map;
            neighbour_map->top = map;
        }
    }

    // LEFT connection
    if(!map->left) {
        neighbour_map = get_map_by_position_in_map_path(map_path, map->pos_x - 1, map->pos_y);
        if (neighbour_map) {
            map->left = neighbour_map;
            neighbour_map->right = map;
        }
    }

    // RIGHT connection
    if(!map->right) {
        neighbour_map = get_map_by_position_in_map_path(map_path, map->pos_x + 1, map->pos_y);
        if (neighbour_map) {
            map->right = neighbour_map;
            neighbour_map->left = map;
        }
    }
}

map_t *get_or_generate_next_map(map_path_t *map_path, map_t *src, map_t *all_loaded_maps, int nb_maps, int direction) {
    if (!map_path || !src || !all_loaded_maps || nb_maps <= 0) return NULL;

    map_t *dest = NULL;
    int new_pos_x = src->pos_x;
    int new_pos_y = src->pos_y;

    // Calculate new position based on direction
    switch(direction) {
        case TOP:    new_pos_y++; break;
        case BOTTOM: new_pos_y--; break;
        case LEFT:   new_pos_x--; break;
        case RIGHT:  new_pos_x++; break;
        default:     return NULL;
    }

    // Try to find existing map at new position
    dest = get_map_by_position_in_map_path(map_path, new_pos_x, new_pos_y);

    // Generate new map if none exists and we haven't used all maps
    if(!dest && map_path->size != nb_maps) {
        int attempts = 0;
        int max_attempts = nb_maps * 2;  // Prevent infinite loop

        do {
            srand(time(NULL) + attempts);
            int random = rand() % nb_maps;
            dest = &all_loaded_maps[random];
            attempts++;
        } while (contains_map_in_map_path(map_path, dest) && attempts < max_attempts);

        if (attempts >= max_attempts) return NULL;

        // Set new position for selected map
        dest->pos_x = new_pos_x;
        dest->pos_y = new_pos_y;

        // Connect with neighbors and insert into path
        connect_map_neighbour(map_path, dest);
        insert_map_path(map_path, dest);
    }

    return dest;
}

void free_map_path(map_path_t **map_path) {
    if (!map_path || !*map_path) return;

    node_map_path_t *current = (*map_path)->head;
    while (current) {
        node_map_path_t *next = current->next;
        free_node_map_path(&current);
        current = next;
    }

    free(*map_path);
    *map_path = NULL;
}