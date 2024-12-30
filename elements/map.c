/**
 * @file map.c
 * @brief Implémentation des fonctions liées à la structure case_t & map_t
 */

#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "../utils/ncurses_utils.h"
#include "../utils/files_utils.h"
#include "../utils/other_utils.h"
#include "../utils/threads_utils.h"
#include "../utils/macros.h"
#include "../elements/entity.h"
#include "map.h"

void init_case(case_t *c, int coord_x, int coord_y, char symbol, int biome, int id_entity) {
    if (!c) return;

    c->coord_x = coord_x;
    c->coord_y = coord_y;
    c->symbol = symbol;
    c->biome = biome;
    c->id_entity = id_entity;
}

void free_case(case_t **c) {
    if (!c || !*c) return;
    free(*c);
    *c = NULL;
}

int count_maps() {
    int nb = 0;
    DIR *d = opendir(PATH_LOCATION_MAPS_FILES);
    if (!d) return 0;

    struct dirent *dir;
    while ((dir = readdir(d))) {
        if (strstr(dir->d_name, ".bin")) nb++;
    }
    closedir(d);
    return nb;
}

map_t *load_all_maps() {
    int nb_maps = count_maps();
    if (nb_maps <= 0) return NULL;

    map_t *maps = malloc_check(sizeof(map_t) * nb_maps);
    if (!maps) return NULL;

    DIR *d = opendir(PATH_LOCATION_MAPS_FILES);
    if (!d) {
        free(maps);
        return NULL;
    }

    int index = 0;
    struct dirent *dir;
    while ((dir = readdir(d)) && index < nb_maps) {
        if (strstr(dir->d_name, ".bin")) {
            char file[PATH_MAX] = PATH_LOCATION_MAPS_FILES;
            strcat(file, dir->d_name);

            map_t *loaded_map = load_map(file);
            if (loaded_map) {
                maps[index] = *loaded_map;
                free(loaded_map);
                index++;
            }
        }
    }
    closedir(d);
    return maps;
}

map_t *get_loaded_map_by_name(map_t *list_maps, int nb_maps, char *name) {
    if (!list_maps || !name || nb_maps <= 0) return NULL;

    for (int i = 0; i < nb_maps; i++) {
        if (list_maps[i].name && strcmp(list_maps[i].name, name) == 0) {
            return &list_maps[i];
        }
    }
    return NULL;
}

map_t *load_map(char *filename) {
    if (!filename) return NULL;

    char filepath[PATH_MAX] = PATH_LOCATION_MAPS_FILES;
    strcat(filepath, filename);

    int fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        if (errno != ENOENT) {
            fprintf(stderr, "Erreur lors de l'ouverture du fichier %s: %s\n",
                    filepath, strerror(errno));
        }
        return NULL;
    }

    map_t *map = malloc_check(sizeof(map_t));
    if (!map) {
        close(fd);
        return NULL;
    }

    // Initialize map
    memset(map, 0, sizeof(map_t));

    // Read map data
    if (read_map(fd, map) == -1) {
        free(map);
        close(fd);
        return NULL;
    }

    // Initialize mutexes
    for (int i = 0; i < NB_COL * NB_LIGNE; i++) {
        mutex_init_check(&map->MUTEX_CASES[i]);
    }

    close(fd);
    return map;
}

void save_map(char *filename, map_t *map) {
    if (!filename || !map) return;

    char filepath[PATH_MAX] = PATH_LOCATION_MAPS_FILES;
    strcat(filepath, filename);

    int fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier %s: %s\n",
                filepath, strerror(errno));
        return;
    }

    write_map(fd, map);
    close(fd);
}

map_t *init_empty_map(char *name, unsigned short height, unsigned short width) {
    if (!name || height == 0 || width == 0) return NULL;

    map_t *map = malloc_check(sizeof(map_t));
    if (!map) return NULL;

    // Initialize map structure
    memset(map, 0, sizeof(map_t));
    map->name = strdup(name);
    map->height = height;
    map->width = width;
    map->pos_x = -1;
    map->pos_y = -1;

    // Allocate and initialize cases
    map->cases = malloc_check(sizeof(case_t) * height * width);
    if (!map->cases) {
        free(map->name);
        free(map);
        return NULL;
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            init_case(&map->cases[i * width + j], i, j, SYMB_EMPTY_CASE, HERBE, -1);
        }
    }

    // Initialize mutexes
    for (int i = 0; i < NB_COL * NB_LIGNE; i++) {
        mutex_init_check(&map->MUTEX_CASES[i]);
    }

    return map;
}

void add_entity(map_t *map, case_t *ca, entity_t *entity) {
    if (!map || !ca || !entity) return;

    entity_t *new_entities = NULL;
    if (map->nb_entity == 0) {
        new_entities = malloc_check(sizeof(entity_t));
    } else {
        new_entities = realloc(map->entities, sizeof(entity_t) * (map->nb_entity + 1));
    }

    if (!new_entities) return;

    map->entities = new_entities;
    map->entities[map->nb_entity] = *entity;
    ca->id_entity = map->nb_entity;
    map->nb_entity++;
}

entity_t get_entity(map_t *map, case_t *ca) {
    entity_t empty = {0};
    if (!map || !ca || ca->id_entity < 0 || ca->id_entity >= map->nb_entity) {
        return empty;
    }
    return map->entities[ca->id_entity];
}

int get_nb_monster_on_map(map_t *map) {
    if (!map || map->nb_entity == 0) return 0;

    int count = 0;
    for (int i = 0; i < map->nb_entity; i++) {
        if (map->entities[i].type == MONSTER_TYPE) {
            count++;
        }
    }
    return count;
}

int get_nb_artefact_on_map(map_t *map) {
    if (!map || map->nb_entity == 0) return 0;

    int count = 0;
    for (int i = 0; i < map->nb_entity; i++) {
        if (map->entities[i].type == ARTEFACT_TYPE) {
            count++;
        }
    }
    return count;
}

entity_t *get_all_monster_on_map(map_t *map) {
    if (!map || map->nb_entity == 0) return NULL;

    int nb_monsters = get_nb_monster_on_map(map);
    if (nb_monsters == 0) return NULL;

    entity_t *monsters = malloc_check(sizeof(entity_t) * nb_monsters);
    if (!monsters) return NULL;

    int index = 0;
    for (int i = 0; i < map->nb_entity && index < nb_monsters; i++) {
        if (map->entities[i].type == MONSTER_TYPE) {
            monsters[index++] = map->entities[i];
        }
    }

    return monsters;
}

void add_case(map_t *map, case_t *c, int x, int y) {
    if (!map || !c || x < 0 || x >= NB_LIGNE || y < 0 || y >= NB_COL) return;
    map->cases[y * map->width + x] = *c;
}

case_t *get_case(map_t *map, int x, int y) {
    if (!map || x < 0 || x >= NB_LIGNE || y < 0 || y >= NB_COL) return NULL;
    return &map->cases[x * map->width + y];
}

pthread_mutex_t *get_mutex_case(map_t *map, int x, int y) {
    if (!map || x < 0 || x >= NB_LIGNE || y < 0 || y >= NB_COL) return NULL;
    return &map->MUTEX_CASES[x * map->width + y];
}

void lock_map(map_t *map) {
    if (!map) return;
    for (int i = 0; i < NB_COL * NB_LIGNE; i++) {
        mutex_lock_check(&map->MUTEX_CASES[i]);
    }
}

void unlock_map(map_t *map) {
    if (!map) return;
    for (int i = 0; i < NB_COL * NB_LIGNE; i++) {
        mutex_unlock_check(&map->MUTEX_CASES[i]);
    }
}

void trylock_map(map_t *map) {
    if (!map) return;
    for (int i = 0; i < NB_COL * NB_LIGNE; i++) {
        pthread_mutex_trylock(&map->MUTEX_CASES[i]);
    }
}

case_t *get_random_case(map_t *map) {
    if (!map) return NULL;

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand((unsigned int)(ts.tv_nsec));

    int x = abs(rand() % NB_LIGNE);
    int y = abs(rand() % NB_COL);

    return get_case(map, x, y);
}

int is_spawnable_spawn(case_t *ca) {
    return ca && ca->biome != EAU && ca->symbol == SYMB_EMPTY_CASE;
}

case_t *get_random_case_to_spawn(map_t *map) {
    if (!map) return NULL;

    case_t *ca;
    int max_attempts = NB_LIGNE * NB_COL;
    int attempts = 0;

    do {
        ca = get_random_case(map);
        attempts++;
    } while (!is_spawnable_spawn(ca) && attempts < max_attempts);

    return (attempts < max_attempts) ? ca : NULL;
}

int can_move(map_t *map, int x, int y) {
    case_t *c = get_case(map, x, y);
    return c && c->biome != EAU && c->symbol != SYMB_OBSTACLE;
}

int is_out_of_map(int x, int y) {
    return x < 0 || y < 0 || y >= NB_COL || x >= NB_LIGNE;
}

int get_direction_out_of_map(int x, int y) {
    if (x < 0) return TOP;
    if (x >= NB_LIGNE) return BOTTOM;
    if (y < 0) return LEFT;
    if (y >= NB_COL) return RIGHT;
    return -1;
}

int is_monster(map_t *map, int x, int y) {
    case_t *c = get_case(map, x, y);
    return c && c->symbol == SYMB_MONSTRE;
}

int is_artefact(map_t *map, int x, int y) {
    case_t *c = get_case(map, x, y);
    return c && c->symbol == SYMB_ARTEFACT;
}

int is_tresor(map_t *map, int x, int y) {
    case_t *c = get_case(map, x, y);
    return c && c->symbol == SYMB_TRESOR;
}

void spawn_tresor(map_t *map) {
    if (!map) return;
    case_t *c = get_random_case_to_spawn(map);
    if (c) c->symbol = SYMB_TRESOR;
}

int is_hero(map_t *map, int x, int y) {
    case_t *c = get_case(map, x, y);
    return c && c->symbol == SYMB_HERO;
}

void free_map(map_t **map) {
    if (!map || !*map) return;

    // Free entities
    if ((*map)->entities) {
        for (int i = 0; i < (*map)->nb_entity; i++) {
            if ((*map)->entities[i].name) {
                free((*map)->entities[i].name);
            }
        }
        free((*map)->entities);
    }

    // Free cases
    if ((*map)->cases) {
        free((*map)->cases);
    }

    // Free name
    if ((*map)->name) {
        free((*map)->name);
    }

    // Free map structure
    free(*map);
    *map = NULL;
}