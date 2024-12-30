/**
 * @file files_utils.c
 * @brief Fonctions utiles aux fichiers
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "macros.h"
#include "../elements/map.h"
#include "../elements/entity.h"
#include "../elements/hero.h"
#include "other_utils.h"
#include "ncurses_utils.h"
#include "include.h"

static void handle_write_error(const char *operation) {
    ncurses_stop();
    fprintf(stderr, "Erreur d'écriture %s: %s\n", operation, strerror(errno));
    exit(EXIT_FAILURE);
}

void write_string(int fd, char *str) {
    if (fd < 0 || !str) return;

    size_t taille = (strlen(str) + 1) * sizeof(char);
    if (write(fd, &taille, sizeof(size_t)) == -1) {
        handle_write_error("taille string");
    }
    if (write(fd, str, taille) == -1) {
        handle_write_error("string");
    }
}

void write_us(int fd, unsigned short us) {
    if (fd < 0) return;

    if (write(fd, &us, sizeof(unsigned short)) == -1) {
        handle_write_error("unsigned short");
    }
}

void write_int(int fd, int i) {
    if (fd < 0) return;

    if (write(fd, &i, sizeof(int)) == -1) {
        handle_write_error("int");
    }
}

void write_cases(int fd, case_t *cases) {
    if (fd < 0 || !cases) return;

    if (write(fd, cases, sizeof(case_t) * NB_COL * NB_LIGNE) == -1) {
        handle_write_error("cases");
    }
}

void write_entity(int fd, entity_t *entity) {
    if (fd < 0 || !entity) return;

    write_int(fd, entity->type);
    write_string(fd, entity->name);
    write_int(fd, entity->health);
    write_int(fd, entity->armor);
    write_int(fd, entity->strength);
    write_int(fd, entity->move_speed);
    write_int(fd, entity->hit_speed);
    write_int(fd, entity->coord_x);
    write_int(fd, entity->coord_y);
    write_int(fd, entity->index);
    write_int(fd, entity->can_attack);
}

void write_map(int fd, map_t *map) {
    if (fd < 0 || !map) return;

    write_string(fd, map->name);
    write_int(fd, map->pos_x);
    write_int(fd, map->pos_y);
    write_us(fd, map->height);
    write_us(fd, map->width);
    write_us(fd, map->nb_entity);

    if(map->nb_entity > 0 && map->entities) {
        for (int i = 0; i < map->nb_entity; i++) {
            write_entity(fd, &map->entities[i]);
        }
    }

    write_cases(fd, map->cases);
}

void write_hero(int fd, hero_t *hero) {
    if (fd < 0 || !hero) return;

    int fields[] = {
        hero->map_x, hero->map_y,
        hero->x_spawn, hero->y_spawn,
        hero->coord_x, hero->coord_y,
        hero->max_health, hero->health,
        hero->armor, hero->strength,
        hero->move_speed, hero->hit_speed,
        hero->xp, hero->nb_piece
    };

    for (size_t i = 0; i < sizeof(fields)/sizeof(fields[0]); i++) {
        write_int(fd, fields[i]);
    }
}

static void handle_read_error(const char *operation) {
    ncurses_stop();
    fprintf(stderr, "Erreur de lecture %s: %s\n", operation, strerror(errno));
    exit(EXIT_FAILURE);
}

void read_string(int fd, char **str) {
    if (fd < 0 || !str) return;

    size_t taille;
    if (read(fd, &taille, sizeof(size_t)) == -1) {
        handle_read_error("taille string");
    }

    *str = malloc_check(taille * sizeof(char));
    if (read(fd, *str, taille) == -1) {
        free(*str);
        handle_read_error("string");
    }
}

void read_us(int fd, unsigned short *us) {
    if (fd < 0 || !us) return;

    if (read(fd, us, sizeof(unsigned short)) == -1) {
        handle_read_error("unsigned short");
    }
}

void read_int(int fd, int *i) {
    if (fd < 0 || !i) return;

    if (read(fd, i, sizeof(int)) == -1) {
        handle_read_error("int");
    }
}

void read_cases(int fd, case_t *cases) {
    if (fd < 0 || !cases) return;

    if (read(fd, cases, sizeof(case_t) * NB_COL * NB_LIGNE) == -1) {
        handle_read_error("cases");
    }
}

void read_entity(int fd, entity_t *entity) {
    if (fd < 0 || !entity) return;

    read_int(fd, &entity->type);
    read_string(fd, &entity->name);
    read_int(fd, &entity->health);
    read_int(fd, &entity->armor);
    read_int(fd, &entity->strength);
    read_int(fd, &entity->move_speed);
    read_int(fd, &entity->hit_speed);
    read_int(fd, &entity->coord_x);
    read_int(fd, &entity->coord_y);
    read_int(fd, &entity->index);
    read_int(fd, &entity->can_attack);
}

void read_hero(int fd, hero_t *hero) {
    if (fd < 0 || !hero) return;

    read_int(fd, &hero->map_x);
    read_int(fd, &hero->map_y);
    read_int(fd, &hero->x_spawn);
    read_int(fd, &hero->y_spawn);
    read_int(fd, &hero->coord_x);
    read_int(fd, &hero->coord_y);
    read_int(fd, &hero->max_health);
    read_int(fd, &hero->health);
    read_int(fd, &hero->armor);
    read_int(fd, &hero->strength);
    read_int(fd, &hero->move_speed);
    read_int(fd, &hero->hit_speed);
    read_int(fd, &hero->xp);
    read_int(fd, &hero->nb_piece);
}

void read_map(int fd, map_t *map) {
    if (fd < 0 || !map) return;

    read_string(fd, &map->name);
    read_int(fd, &map->pos_x);
    read_int(fd, &map->pos_y);
    read_us(fd, &map->height);
    read_us(fd, &map->width);
    read_us(fd, &map->nb_entity);

    if(map->nb_entity > 0) {
        map->entities = malloc_check(sizeof(entity_t) * map->nb_entity);
        for (int i = 0; i < map->nb_entity; i++) {
            read_entity(fd, &map->entities[i]);
        }
    }

    map->cases = malloc_check(sizeof(case_t) * NB_COL * NB_LIGNE);
    read_cases(fd, map->cases);
}